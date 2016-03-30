#ifndef HOLD_HPP
#define HOLD_HPP

#include "Note.hpp"

class Hold : public Note {
public:
	Hold(int lane, int start, int end);
	int end;
};

#endif//HOLD_HPP