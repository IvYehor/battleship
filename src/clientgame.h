#ifndef CLIENTGAME_H_INCLUDED
#define CLIENTGAME_H_INCLUDED

#include "renderer.h"
#include "server.h"
#include "client.h"
#include "map.h"

class ClientGame {
public:
	ClientGame(Renderer &renderer_, Client &client_) : renderer(renderer_), client(client_) {}

	void Start();
	int exchangeNames();
	int setFleet();
	int myTurn();
	int theirTurn();
	void endScreen(bool iWon);
private:
	Renderer &renderer;
	Client &client;

	MyMap myMap;
	TheirMap theirMap;

	std::string myName;
	std::string theirName;
};

#endif
