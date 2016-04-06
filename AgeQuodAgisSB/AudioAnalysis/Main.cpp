#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#define _USE_MATH_DEFINES

#define N 64

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <math.h>
#include "kiss_fft.h"
#include "Storyboard.hpp"
#include "Sprite.hpp"

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

// Reads Wav info into a Wav class
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
	// Background setup
	Sprite* background = new Sprite("blank.png", Vector2(320, 240), Layer::Background);
	background->Color(0, 1000000, Color(0), Color(0));
	background->ScaleVector(0, 1000000, Vector2(1.366f, 0.768f), Vector2(1.366f, 0.768f));
	
	char* filePath = R"(X:\Music\void\Age quod agis\ageQuodAgisLeftMono.wav)";
	Wav song = loadWavFile(filePath);

	std::cout << song.header() << std::endl;

	float barWidth = 0.01f;
	std::vector<Sprite*> bars;
	for (int i = 0; i < N / 2; ++i) {
		Sprite* bar = new Sprite("blank.png", Vector2(12 * i, 300), Layer::Foreground, Origin::BottomCentre);
		bar->ScaleVector(0, 0, Vector2(barWidth, 0.01f), Vector2(barWidth, 0.01f));
		bar->Color(0, 0, Color(255), Color(255));
		bars.push_back(bar);
	}

	int progressRate = song.sampleRate / 10;
	for (int j = 0; j < song.size; j += progressRate) {
		float* input = (float*)malloc(N * sizeof(float));
		for (int i = 0; i < N; ++i) {
			input[i] = hann(song.data[i + j], i, N);
		}
		
		kiss_fft_cpx in[N], out[N];
		for (int i = 0; i < N; ++i) {
			in[i].r = input[i];
			in[i].i = 0.0f;
		}
		kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, NULL, NULL);
		kiss_fft(cfg, in, out);
		free(cfg);

		for (int i = 0; i < N / 2; ++i) {
			float magnitudeSquared = out[i].r * out[i].r + out[i].i * out[i].i;
			float dB = 10.0f * log10(magnitudeSquared);
			int endTime = (j / progressRate) * 100;
			bars[i]->ScaleVector(bars[i]->endTime, endTime, bars[i]->scaleVector, Vector2(barWidth, (dB / 96.0f) / 5.0f));
		}
	}

	Storyboard::Instance()->Write(R"(C:\Users\Wax Chug da Gwad\AppData\Local\osu!\Songs\409783 void - Age quod agis\void - Age quod agis (TheWeirdo9).osb)");
	std::cout << "Finished audio analysis" << std::endl;
	std::cin.get();
	
	return 0;
}