#ifndef BATTLESHIP_CLIENT_INCLUDED
#define BATTLESHIP_CLIENT_INCLUDED

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include <iostream>
#include <array>
#include "message.h"
#include "connection.h"

class Client : public Connection{
public:
	Client(std::string address, unsigned short port, asio::io_context &context_) : Connection(context_) ,
		endpoint(asio::ip::address::from_string(address), port) {}

	~Client() {}

	void Start() override {
		socket.open(asio::ip::tcp::v4());
		socket.connect(endpoint);

		/*asio::async_connect(socket, endpoint, 
			[this](std::error_code ec, asio::ip::tcp::endpoint){
				if(ec) {
					std::cout << "Async connect error: " << ec.message() << '\n';
					return;
				}
				socket = asio::ip::tcp::socket(std::move(socket_));
			});*/
	}
	
private:
	asio::ip::tcp::endpoint endpoint;
};

#endif
