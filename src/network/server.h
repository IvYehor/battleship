#ifndef BATTLESHIP_SERVER_INCLUDED
#define BATTLESHIP_SERVER_INCLUDED

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include <iostream>
#include <array>
#include <thread>
#include "message.h"
#include "connection.h"

class Server : public Connection {
public:
	Server(uint16_t port, asio::io_context &context_) : endpoint(asio::ip::tcp::v4(), port), acceptor(context_, endpoint), 
		Connection(context_) {}
	
	~Server() {}

	void Start() override {
		acceptor.async_accept(
			[this](const asio::error_code &error, asio::ip::tcp::socket socket_) {
				if(error) {
					return;
				}
				socket = asio::ip::tcp::socket(std::move(socket_));
			}
		);
	}
	

private:
	asio::ip::tcp::endpoint endpoint;
	asio::ip::tcp::acceptor acceptor;
};

#endif

