#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include <iostream>
#define NCURSES_NOMACROS
#include <ncurses.h>
#include <thread>
#include <string>

#include "map.h"

class Renderer {
public:
	Renderer();
	~Renderer();
	bool getState() const;
	int playerChoose(bool &choice);
	int enterName(std::string &name);

	int setFleet(MyMap &myMap, std::string &myName, std::string &theirName);	
	int myTurn(MyMap &myMap, TheirMap &theirMap, std::string &myName, std::string &theirName, int &shotX, int &shotY);
	int theirTurn(MyMap &myMap, TheirMap &theirMap, std::string &myName, std::string &theirName, bool &received);
	
	int endScreen(bool iwon);

	//Server
	int waitingForClient(const bool &accepted);
	
	//Client
	int enterIpAddress(std::string &ip);

	void errorScreen();
private:
	void drawLineInMap();
	void center();
	void setDrawPos(int x, int y);
	void drawBox(int x, int y, int w, int h, char ch);
	void drawRandomChars(int w, int h, int n, unsigned int seed, char ch);
	void drawLine(int x, int y, int len, bool hor, char ch);
	void drawChar(int x, int y, char ch);
	void printMsg(int x, int y, std::string msg);
	void drawTitle();

	bool finished;
	int width, height;
	int cX, cY;
	int drawPosX, drawPosY;
};


#endif
