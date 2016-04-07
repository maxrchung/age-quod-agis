#ifndef SPECTRUM_HPP
#define SPECTRUM_HPP

#include <string>

class Spectrum {
public:
	static Spectrum* Instance();
	void Generate(std::string& songPath);
private:
	Spectrum() {};
	Spectrum(const Spectrum&) {};
	Spectrum& operator=(const Spectrum&) {};
	static Spectrum* instance;
};

#endif//SPECTRUM_HPP