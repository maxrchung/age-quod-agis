#include "Spectrum.hpp"

Spectrum* Spectrum::instance = NULL;

Spectrum* Spectrum::Instance() {
	if (!instance) {
		instance = new Spectrum();
	}
	return instance;
}

void Spectrum::Generate(std::string& songPath) {

}