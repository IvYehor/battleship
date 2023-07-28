#ifndef SERVERGAME_H_INCLUDED
#define SERVERGAME_H_INCLUDED

#include "renderer.h"
#include "server.h"
#include "client.h"
#include "map.h"

class ServerGame {
public:
	ServerGame(Renderer &renderer_, Server &server_) : renderer(renderer_), server(server_) {}

	void Start();
	int exchangeNames();
	int setFleet();
	int myTurn();
	int theirTurn();
	void endScreen(bool iWon);
private:
	Renderer &renderer;
	Server &server;

	MyMap myMap;
	TheirMap theirMap;

	std::string myName;
	std::string theirName;
};

#endif
