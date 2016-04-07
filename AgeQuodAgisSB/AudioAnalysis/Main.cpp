#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _USE_MATH_DEFINES

#define N 4096
#define B 32
#define BperN 4096/32

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <math.h>
#include "kiss_fft.h"
#include "Storyboard.hpp"
#include "Sprite.hpp"
#include <vector>

// WAV Resources
// Guide: http://rogerchansdigitalworld.blogspot.com/2010/05/how-to-read-wav-format-file-in-c.html
// WAV format: http://soundfile.sapp.org/doc/WaveFormat/wav-sound-format.gif
class Wav {
public:
	char chunkId[5];
	unsigned long chunkSize;
	char format[5];
	char subchunk1Id[5];
	unsigned long subchunk1Size;
	short audioFormat;
	short numChannels;
	unsigned long sampleRate;
	unsigned long byteRate;
	short blockAlign;
	short bitsPerSample;
	char subchunk2Id[5];
	unsigned long subchunk2Size;
	unsigned long size;
	short* data;

	std::string header() {
		std::stringstream ss;
		ss << "chunkId: " << chunkId << std::endl;
		ss << "chunkSize: " << chunkSize << std::endl;
		ss << "format: " << format << std::endl;
		ss << "subchunk1Id: " << subchunk1Id << std::endl;
		ss << "subchunk1Size: " << subchunk1Size << std::endl;
		ss << "audioFormat: " << audioFormat << std::endl;
		ss << "numChannels: " << numChannels << std::endl;
		ss << "sampleRate: " << sampleRate << std::endl;
		ss << "byteRate: " << byteRate << std::endl;
		ss << "blockAlign: " << blockAlign << std::endl;
		ss << "bitsPerSample: " << bitsPerSample << std::endl;
		ss << "subchunk2Id: " << subchunk2Id << std::endl;
		ss << "subchunk2Size: " << subchunk2Size << std::endl;
		ss << "size: " << size << std::endl;

		return ss.str();
	}
};

// Reads Wav info in to a Wav class
Wav loadWavFile(char* fname) {
	FILE* fp = fopen(fname, "rb");
	Wav wav;

	fread(wav.chunkId, sizeof(char), 4, fp);
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
	fread(&wav.subchunk2Size, sizeof(unsigned long), 1, fp);
	wav.size = wav.subchunk2Size / sizeof(short);
	wav.data = (short*)malloc(wav.subchunk2Size);
	fread(wav.data, sizeof(short), wav.size, fp);

	fclose(fp);

	return wav;
}

// Hann function
float hann(short in, int index, int size) {
	float value = 2.0f * M_PI * index;
	value = value / (size - 1.0f);
	value = cos(value);
	value = 1 - value;
	value = value * 0.5f;
	value = value * in;
	return value;
}

int main() {
	// See http://www.engineeringtoolbox.com/octave-bands-frequency-limits-d_1602.html
	std::vector<Vector2> freqBands = {
		//Vector2(11.2f, 14.1f),
		Vector2(14.1f, 17.8f),
		Vector2(17.8f, 22.4f),
		Vector2(22.4f, 28.2f),
		Vector2(28.2f, 35.5f),
		Vector2(35.5f, 44.7f),
		Vector2(44.7f, 56.2f),
		Vector2(56.2f, 70.8f),
		Vector2(70.8f, 89.1f),
		Vector2(89.1f, 112.0f),
		Vector2(112.0f, 141.0f),
		Vector2(141.0f, 178.0f),
		Vector2(178.0f, 224.0f),
		Vector2(224.0f, 282.0f),
		Vector2(282.0f, 355.0f),
		Vector2(355.0f, 447.0f),
		Vector2(447.0f, 562.0f),
		Vector2(562.0f, 708.0f),
		Vector2(708.0f, 891.0f),
		Vector2(891.0f, 1122.0f),
		Vector2(1122.0f, 1413.0f),
		Vector2(1413.0f, 1778.0f),
		Vector2(1778.0f, 2239.0f),
		Vector2(2239.0f, 2818.0f),
		Vector2(2818.0f, 3548.0f),
		Vector2(3548.0f, 4467.0f),
		Vector2(4467.0f, 5623.0f),
		Vector2(5623.0f, 7079.0f),
		Vector2(7079.0f, 8913.0f),
		Vector2(8913.0f, 11220.0f),
		Vector2(11220.0f, 14130.0f),
		Vector2(14130.0f, 17780.0f),
		Vector2(17780.0f, 22390.0f)
	};

	std::vector<Vector2> freqBandIndices;
	float freqConstant = 44100.0f / N;
	for (int i = 0; i < 32; ++i) {
		int startBand = freqBands[i].x;
		int startIndex = startBand / freqConstant;
		int endBand = freqBands[i].y;
		int endIndex = endBand / freqConstant;

		freqBandIndices.push_back(Vector2(startIndex, endIndex));
	}

	// Background setup
	Sprite* background = new Sprite("blank.png", Vector2(320, 240), Layer::Background);
	background->Color(0, 1000000, Color(0), Color(0));
	background->ScaleVector(0, 1000000, Vector2(1.366f, 0.768f), Vector2(1.366f, 0.768f));
	
	// Load WAV
	char* filePath = R"(X:\Music\void\Age quod agis\ageQuodAgisLeftMono.wav)";
	Wav song = loadWavFile(filePath);
	std::cout << song.header() << std::endl;

	// Set up bars
	float barWidth = 0.01f;
	std::vector<Sprite*> bars;
	for (int i = 0; i < B; ++i) {
		Sprite* bar = new Sprite("blank.png", Vector2(12 * i, 300), Layer::Foreground, Origin::BottomCentre);
		bar->ScaleVector(0, 0, Vector2(barWidth, 0.0f), Vector2(barWidth, 0.0f));
		bar->Color(0, 0, Color(255), Color(255));
		bars.push_back(bar);
	}

	// Progress at a rate of 100ms
	// Sample rate: 44100, thus we progress at 4410 samples for each 100ms
	int progressRate = song.sampleRate / 10;
	for (int j = progressRate; j < song.size / 2; j += progressRate) {
		std::cout << "Processing: " << j << std::endl;

		// I chose to grab only 4096 samples because that's a better power of 2 value
		// Grab 4096 samples and window the values
		float* input = (float*)malloc(N * sizeof(float));
		for (int i = 0; i < N; ++i) {
			float data = song.data[i + j - (N / 2)];
			input[i] = hann(data, i, N) / 32768.0f;
		}
		
		// Setup FFT and apply FFT to input
		kiss_fft_cpx in[N], out[N];
		for (int i = 0; i < N; ++i) {
			in[i].r = input[i];
			in[i].i = 0.0f;
		}
		kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, NULL, NULL);
		kiss_fft(cfg, in, out);
		free(cfg);

		// Narrow down out[N] to only 32 bins
		for (int i = 0; i < B; ++i) {
			// Find the max within a few indices of out[N]
			float maxMagSquared = 0.0f;
			for (int b = (int)freqBandIndices[i].x; b <= (int)freqBandIndices[i].y; ++b) {
				float magnitudeSquared = out[b].r * out[b].r + out[b].i * out[b].i;
				if (magnitudeSquared > maxMagSquared) {
					maxMagSquared = magnitudeSquared;
				}
			}

			//float dB = 10.0f * log10f(maxMagSquared);
			//float dBscale = (dB + 96.33f) / 96.33f;
			//if (dBscale < 0.0f) {
			//	dBscale = 0.0f;
			//}
			//
			//float endScale = dBscale * 0.1f;
			float loggedMax = 10.0f * log10f(maxMagSquared);
			float endScale = loggedMax * 0.005f;
			if (endScale < 0.0f) {
				endScale = 0.0f;
			}
			int endTime = (j / progressRate) * 100;
			bars[i]->ScaleVector(bars[i]->endTime, endTime, bars[i]->scaleVector, Vector2(barWidth, endScale));
		}
	}

	// Write to storyboard
	Storyboard::Instance()->Write(R"(C:\Users\Wax Chug da Gwad\AppData\Local\osu!\Songs\409783 void - Age quod agis\void - Age quod agis (TheWeirdo9).osb)");
	std::cout << "Finished audio analysis" << std::endl;
	std::cin.get();
	
	return 0;
}