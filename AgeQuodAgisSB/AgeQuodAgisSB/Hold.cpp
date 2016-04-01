#include "Hold.hpp"
#include <iostream>

Hold::Hold(int lane, int start, int end)
	: Note(lane, start) {
	this->end = end;
}