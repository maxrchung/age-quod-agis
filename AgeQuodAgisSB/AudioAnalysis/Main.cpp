#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

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

		return ss.str();
	}
};

Wav loadWavFile(char* fname) {
	FILE* fp = fopen(fname, "rb");
	Wav wavData;

	fread(wavData.chunkId, sizeof(char), 4, fp);
	wavData.chunkId[4] = '\0';
	fread(&wavData.chunkSize, sizeof(unsigned long), 1, fp);
	fread(wavData.format, sizeof(char), 4, fp);
	wavData.format[4] = '\0';

	fread(wavData.subchunk1Id, sizeof(char), 4, fp);
	wavData.subchunk1Id[4] = '\0';
	fread(&wavData.subchunk1Size, sizeof(unsigned long), 1, fp);
	fread(&wavData.audioFormat, sizeof(short), 1, fp);
	fread(&wavData.numChannels, sizeof(short), 1, fp);
	fread(&wavData.sampleRate, sizeof(unsigned long), 1, fp);
	fread(&wavData.byteRate, sizeof(unsigned long), 1, fp);
	fread(&wavData.blockAlign, sizeof(short), 1, fp);
	fread(&wavData.bitsPerSample, sizeof(short), 1, fp);

	fread(wavData.subchunk2Id, sizeof(char), 4, fp);
	wavData.subchunk2Id[4] = '\0';
	fread(&wavData.subchunk2Size, sizeof(unsigned long), 1, fp);
	wavData.subchunk2Size /= sizeof(short);
	wavData.data = (short*)malloc(wavData.subchunk2Size);
	fread(wavData.data, sizeof(short), wavData.subchunk2Size, fp);

	fclose(fp);

	return wavData;
}

int main() {
	char* filePath = R"(X:\Music\void\Age quod agis\ageQuodAgis.wav)";

	Wav song = loadWavFile(filePath);
	std::cout << "There are " << song.subchunk2Size / 2 << " samples in this WAV file." << std::endl;
	std::cout << song.header() << std::endl;
	std::cin.get();
	return 0;
}