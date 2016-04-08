#ifndef SPECTRUM_HPP
#define SPECTRUM_HPP

// Have to do this because I can't seem to be able to set an array's length 
// with a variable, even if I specify it as const. This is used to evaluate 
// how many samples you should take at a time for each window, and this is 
// also the size of the FFT arrays
#define WINSIZE 8192

#include <string>
#include "Sprite.hpp"
#include <vector>
#include "Wav.hpp"

class Spectrum {
public:
	static Spectrum* Instance();
	void Generate(const std::string& songPath);
	std::vector<Sprite*> bars;
private:
	Spectrum() {};
	Spectrum(const Spectrum&) {};
	Spectrum& operator=(const Spectrum&) {};
	static Spectrum* instance;

	Wav LoadWavFile(const char* fname);
	float Hann(short in, int index, int size);
	void CalculateFrequencyBands();
	void SetupBars();
	void TakeSnapshots();

	Wav song;

	// Power to measure frequencies by
	float freqPower = powf(2.0f, 0.25f);
	// Target frequencies we want to shoot for
	float freqBandStart = 20.0f;
	float freqBandEnd = 20000.0f;
	std::vector<float> freqBands;
	// Actual indices corresponding to the WINSIZE
	std::vector<float> freqBandIndices;

	std::string barFileName = "blank.png";
	float barMinWidth = 0.01f;
	float barMinHeight = 0.005f;
	float barScaleFactor = 0.002f;
	float barGradient = 0.25f;
	float barBuffer = 75.0f;
	// How fast do you want to take snapshots, in milliseconds
	float snapshotRate = 100.0f;

};

#endif//SPECTRUM_HPP