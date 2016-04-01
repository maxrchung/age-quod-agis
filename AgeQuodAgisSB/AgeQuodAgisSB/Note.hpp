#ifndef NOTE_HPP
#define NOTE_HPP

class Note {
public:
	Note(int lane, int start);
	int lane;
	int start;
	int end;
};

#endif//NOTE_HPP