// To get by errors for C-style file I/O
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _USE_MATH_DEFINES

#include "Spectrum.hpp"

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

void CalculateFrequencies() {
	for (int )
}

// Hann function
float Spectrum::Hann(short in, int index, int size) {
	float value = 2.0f * M_PI * index;
	value = value / (size - 1.0f);
	value = cos(value);
	value = 1 - value;
	value = value * 0.5f;
	value = value * in;
	return value;
}

void Spectrum::Generate(const std::string& songPath) {
	// Load in WAV file info, header and data included
	song = LoadWavFile(songPath.c_str());

	CalculateFrequencies();
}