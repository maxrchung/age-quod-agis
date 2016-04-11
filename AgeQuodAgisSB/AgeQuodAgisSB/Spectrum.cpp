// To get by errors for C-style file I/O
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _USE_MATH_DEFINES

#include "Spectrum.hpp"
#include <iostream>
#include "kiss_fft.h"

Spectrum* Spectrum::instance = NULL;

Spectrum* Spectrum::Instance() {
	if (!instance) {
		instance = new Spectrum();
	}
	return instance;
}

// Grabs info from a WAV file and puts it into a class
// Uses C-style because of the guide I referenced off of: http://rogerchansdigitalworld.blogspot.com/2010/05/how-to-read-wav-format-file-in-c.html
// WAV format: http://soundfile.sapp.org/doc/WaveFormat/wav-sound-format.gif
Wav Spectrum::LoadWavFile(const char* fname) {
	FILE* fp = fopen(fname, "rb");
	Wav wav;

	fread(wav.chunkId, sizeof(char), 4, fp);
	// Without the '\0', printing to console will try and print out later characters
	wav.chunkId[4] = '\0';
	fread(&wav.chunkSize, sizeof(unsigned long), 1, fp);
	fread(wav.format, sizeof(char), 4, fp);
	wav.format[4] = '\0';

	fread(wav.subchunk1Id, sizeof(char), 4, fp);
	wav.subchunk1Id[4] = '\0';
	fread(&wav.subchunk1Size, sizeof(unsigned long), 1, fp);
	fread(&wav.audioFormat, sizeof(short), 1, fp);
	fread(&wav.numChannels, sizeof(short), 1, fp);
	fread(&wav.sampleRate, sizeof(unsigned long), 1, fp);
	fread(&wav.byteRate, sizeof(unsigned long), 1, fp);
	fread(&wav.blockAlign, sizeof(short), 1, fp);
	fread(&wav.bitsPerSample, sizeof(short), 1, fp);

	fread(wav.subchunk2Id, sizeof(char), 4, fp);
	wav.subchunk2Id[4] = '\0';
	// subchunk2Size returns the amount of WAV data in bytes
	fread(&wav.subchunk2Size, sizeof(unsigned long), 1, fp);
	// size is the actual number of shorts
	wav.size = wav.subchunk2Size / sizeof(short);
	wav.data = (short*)malloc(wav.subchunk2Size);
	fread(wav.data, sizeof(short), wav.size, fp);

	fclose(fp);

	return wav;
}

void Spectrum::CalculateFrequencyBands() {
	// Find target frequencies we want to look for
	for (float freq = freqBandStart; freq <= freqBandEnd; freq *= freqPower) {
		freqBands.push_back(freq);
	}

	// Find the corresponding indices reflecting the freqs after FFT.
	// FFT divides up the output into linear bins of frequencies. Each
	// value inside freqBandIndices represents the starting bin you should
	// look in. If you are not the last freqBand, see the next index to
	// determine the end range of bins you should search in.
	// The nth FFT bin is n * sampleRate / WINSIZE
	float freqConstant = (float) song.sampleRate / WINSIZE;
	for (int i = 0; i < freqBands.size(); ++i) {
		int freqBand = freqBands[i];
		int freqBandIndex = freqBand / freqConstant;
		freqBandIndices.push_back(freqBandIndex);
	}
}

void Spectrum::SetupBars() {
	for (int i = 0; i < freqBands.size(); ++i) {
		Sprite* bar = new Sprite(barFileName, midpoint, Layer::Foreground, Origin::BottomCentre);
		bar->ScaleVector(songStartOffset, songStartOffset, Vector2(barMinWidth, barMinHeight), Vector2(barMinWidth, barMinHeight));

		// Fades the bars like a gradient. The most satured colors are on the side
		// and the center is faded out
		// Need to have (int) casts here because you get some bizarro values if you try and muck
		// with size()'s unsigned int results
		float slide = i - (int)freqBands.size() / 2;
		float slideNormalized = slide / ((int)freqBands.size() / 2);
		float opaqueness = barGradientMax - barGradientMin;
		float opacity = fabs(slideNormalized * opaqueness) + barGradientMin;
		bar->Fade(songStartOffset, songStart, 0.0f, opacity);
		bar->Fade(songEnd, songEndOffset, opacity, 0.0f);

		// Move bar into correct position/rotation
		float radianDiv = 2 * M_PI / freqBands.size();
		float rotation = radianDiv * i;
		// So that lower and highest frequencies are on the top and reversed
		float rotationOffset = -rotation - M_PI / 2;
		float xPos = cos(rotationOffset) * barBuffer + midpoint.x;
		float yPos = sin(rotationOffset) * barBuffer + midpoint.y;
		Vector2 position(xPos, yPos);
		bar->Move(songStartOffset, songStartOffset, position, position);

		// Account for difference in coordinate systems
		float rotationCorrection = rotationOffset + M_PI / 2;
		bar->Rotate(songStartOffset, songStartOffset, rotationCorrection, rotationCorrection);

		// Coloring, the same as centerpiece and particles
		bar->Color(songStartOffset, songStartOffset, frontColorMax, frontColorMax);

		bars.push_back(bar);
	}
}

// Hann function
float Spectrum::Hann(short in, int index, int size) {
	// Doing this split up shenanigans to show steps
	float value = 2.0f * M_PI * index;
	value = value / (size - 1.0f);
	value = cos(value);
	value = 1 - value;
	value = value * 0.5f;
	value = value * in;
	return value;
}

// Does the big bad job of running through samples,
// computing FFT and scaling bars
void Spectrum::TakeSnapshots() {
	float snapshotsPerSec = 1000.0f / snapshotRate;
	// A little on the distinction between snapshots and progress:
	// Snapshot rate describes how often we're taking snapshots, and
	// progress rate describes how many respective samples we need to
	// progress through to match the snapshot rate
	float progressRate = song.sampleRate / snapshotsPerSec;

	// Find start and end points we take samples from
	int startProgress = songStartOffset / 1000.0f * song.sampleRate;
	int endProgress = songEndOffset / 1000.0f * song.sampleRate;
	for(float p = startProgress; p < endProgress * debugSize; p += progressRate) {
		std::cout << "Processing sample at: " << p << std::endl;

		// To make future index calculations easier
		// One option is to do away with float indices entirely, but
		// I want to avoid this because I feel like it might mess up
		// calculations if progressRate is not a whole number
		int i = (int) p;

		// Grab WINSIZE amount of samples and apply Hann function to them
		float* input = (float*)malloc(WINSIZE * sizeof(float));
		for (int j = 0; j < WINSIZE; ++j) {
			// Taking data half below and after makes sure that our window
			// is centered on where we want to take our snapshot
			float data = song.data[j + i - (WINSIZE / 2)];
			// Make sure to divide!! FFT takes input from -1 to 1 but WAV
			// gives short values between -32000~ to 32000~
			input[j] = Hann(data, j, WINSIZE) / 32768.0f;
		}

		// Setup FFT and apply FFT to input
		// I'm using the complex version of kiss fft, there's a faster/optimized
		// real value only version of it, but this works for now so I'm not eager
		// to switch over
		kiss_fft_cpx in[WINSIZE], out[WINSIZE];
		for (int j = 0; j < WINSIZE; ++j) {
			in[j].r = input[j];
			// Make sure to set imaginary to 0.0f
			in[j].i = 0.0f;
		}
		kiss_fft_cfg cfg = kiss_fft_alloc(WINSIZE, 0, NULL, NULL);
		kiss_fft(cfg, in, out);
		free(cfg);

		// Condense FFT output into frequency band information
		for (int j = 0; j < freqBands.size(); ++j) {
			// Find bin indices
			int startBin = freqBandIndices[j];
			int endBin;
			if (j == freqBands.size() - 1) {
				endBin = WINSIZE / 2;
			}
			else {
				endBin = freqBandIndices[j + 1];
			}

			// Track the max value out of the respective bins
			// You don't have to use max, but a resource I referenced used it: https://github.com/zardoru/osutk/blob/master/tools/musnalisys.py
			// Special thanks to Mr. Zardoru btw, helped point me in the right direction
			// to fix some frequency issues
			float maxMagSquared = 0.0f;
			for (int k = startBin; k <= endBin; ++k) {
				float magSquared = out[k].r * out[k].r + out[k].i * out[k].i;
				if (magSquared > maxMagSquared) {
					maxMagSquared = magSquared;
				}
			}

			// Scale the magnitude to a more reasonable log/dB scale
			float loggedMax = 10.0f * log10f(maxMagSquared);

			float endScale = loggedMax * barScaleFactor;
			// log10 can return from negative infinity to 0, so this clamps negative values
			if (endScale < barMinHeight) {
				endScale = barMinHeight;
			}

			int endTime = (p / progressRate) * snapshotRate;
			int startTime = ((p - progressRate) / progressRate) * snapshotRate;
			// I may have to rethink Sprite.endTime in the future because a lot of other variables can increase it.
			// I'm not sure what's the best way to handle it, but I probably won't touch the library until after 
			// this SB is done
			bars[j]->ScaleVector(startTime, endTime, bars[j]->scaleVector, Vector2(barMinWidth, endScale));
		}
	}
}

void Spectrum::Generate(const std::string& songPath) {
	// Load in WAV file info, header and data included
	song = LoadWavFile(songPath.c_str());
	CalculateFrequencyBands();
	SetupBars();
	TakeSnapshots();
}