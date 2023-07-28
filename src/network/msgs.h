#ifndef MSGS_H_INCLUDED
#define MSGS_H_INCLUDED

enum class ROT { UP, DOWN, RIGHT, LEFT };
enum class COLOR { RED, GREEN, BLUE, YELLOW };

struct Position {
	int x;
	int y;
	ROT r;
};


#endif
