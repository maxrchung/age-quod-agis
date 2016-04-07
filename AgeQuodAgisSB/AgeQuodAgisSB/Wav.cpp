#include "Wav.hpp"
#include <sstream>

std::string Wav::header() {
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