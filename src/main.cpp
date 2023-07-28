#include <iostream>
#include <cstdlib>
#include <utility>
#include <vector>
#include <string>
#include <array>

#define NCURSES_NOMACROS
#include <ncurses.h>

#include <thread>

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include "renderer.h"
#include "map.h"
#include "connection.h"
#include "server.h"
#include "client.h"
#include "message.h"
#include "servergame.h"
#include "clientgame.h"

int main(int argc, char** argv) {
	Renderer renderer;
	
	bool isClient = false;
	renderer.playerChoose(isClient);

	asio::io_context context;
	asio::io_context::work idleWork(context);

	std::thread contextTh([&context](){ context.run(); });
	
	if(isClient) {
		std::string ip;

		renderer.enterIpAddress(ip); 
		// Empty input error
		
		Client client(ip, 8080, context);
		client.Start();

		while(!client.isConnected());

		ClientGame clientGame(renderer, client);
		clientGame.Start();

		while(1);//-------------------
	}	
	else {	
		Server server(8080, context);
		server.Start();
		
		bool accepted = false;
		renderer.waitingForClient(accepted);

		while(!server.isConnected());
		accepted = true;
		
		ServerGame serverGame{renderer, server};
		serverGame.Start();

		while(1); //----------------
	}
	if(contextTh.joinable())
		contextTh.join();	
}
