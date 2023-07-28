#ifndef _CONNECTION_H_INCLUDED
#define _CONNECTION_H_INCLUDED

#define ASIO_STANDALONE
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include <iostream>
#include <array>
#include <thread>
#include "message.h"

class Connection {
public:
	Connection(asio::io_context &context_) : context(context_), socket(context) {}
	
	virtual ~Connection() {
		Stop();
	}
	
	virtual void Start() = 0;
	
	void Stop() {
		if (isConnected())
			asio::post(context, [this](){ socket.close(); });
	}

	bool isConnected() {
		return socket.is_open();
	}


	template<class T>
	int ReadArray(Message<T> &msg) {	
		if(!isConnected())
			return -1;
		
		std::array<size_t, 1> size_t_array;
		asio::error_code ec;
		socket.read_some(asio::buffer(size_t_array), ec);

		if(ec)
			throw asio::system_error(ec);

		if(msg.data_length != size_t_array[0] && msg.data_length != 0)
			return -1;


		if(msg.data_length != 0) {
			socket.read_some(asio::buffer(msg.data, msg.data_length), ec);
		} else {
			msg.data.resize(size_t_array[0]);
			msg.data_length = size_t_array[0];
			socket.read_some(asio::buffer(msg.data, msg.data_length), ec);
		}

		if(ec)
			throw asio::system_error(ec);

		return 0;
	}
	
	template<class T>
	int ReadArrayAsync(Message<T> &msg, int &mState) {	
		if(!isConnected())
			return -1;
		
		mState = 0;

		std::thread t([this, &msg, &mState](){

			std::array<size_t, 1> size_t_array;
			asio::error_code ec;
			socket.read_some(asio::buffer(size_t_array), ec);
	
			if(ec)
				throw asio::system_error(ec);

			if(msg.data_length != size_t_array[0] && msg.data_length != 0) { 
				mState = -1;
				return;
			}

			if(msg.data_length != 0) {
				socket.read_some(asio::buffer(msg.data, msg.data_length), ec);
			} else {
				msg.data.resize(size_t_array[0]);
				msg.data_length = size_t_array[0];
				socket.read_some(asio::buffer(msg.data, msg.data_length), ec);
			}

			if(ec)
				throw asio::system_error(ec);
			
			mState = 1;
		
		});

		t.detach();

		return 0;
	}

	template<class T>
	int ReadPOD(Message<T> &msg) {
		if(!isConnected())
			return -1;

		std::array<size_t, 1> size_t_array;
		asio::error_code ec;
		socket.read_some(asio::buffer(size_t_array), ec);
		
		if(ec)
			throw asio::system_error(ec);
	
		if(msg.data_length != size_t_array[0])
			return -1;
			
		std::array<T, 1> data_array;
		socket.read_some(asio::buffer(data_array, msg.data_length), ec);
		msg.data = data_array[0];
		
		if(ec)
			throw asio::system_error(ec);
		
		return 0;
	}
	
	template<class T>
	int ReadPODAsync(Message<T> &msg, int &mState) {
		if(!isConnected())
			return -1;

		mState = 0;

		std::thread t([this, &msg, &mState](){

			std::array<size_t, 1> size_t_array;
			asio::error_code ec;
			socket.read_some(asio::buffer(size_t_array), ec);
	
			if(ec)
				throw asio::system_error(ec);
			
			if(msg.data_length != size_t_array[0]) {
				mState = -1;
				return;
			}
			
			std::array<T, 1> data_array;
			socket.read_some(asio::buffer(data_array, msg.data_length), ec);
			msg.data = data_array[0];
			
			if(ec)
				throw asio::system_error(ec);

			mState = 1;
		});

		t.detach();

		return 0;
	}
	
	template<class T>
	int WriteArray(Message<T> &msg) {
		if(!isConnected())
			return -1;

		std::array<size_t, 1> size_t_array{msg.data_length};
		socket.write_some(asio::buffer(size_t_array, sizeof(size_t)));
		
		socket.write_some(asio::buffer(msg.data, msg.data_length));
		return 0;
	}

	template<class T>
	int WritePOD(Message<T> &msg) {
		if(!isConnected())
			return -1;

		std::array<size_t, 1> size_t_array{msg.data_length};
		socket.write_some(asio::buffer(size_t_array, sizeof(size_t)));
		
		std::array<T, 1> data_array{msg.data};
		socket.write_some(asio::buffer(data_array, msg.data_length));
		return 0;
	}

protected:
	asio::io_context &context;
	asio::ip::tcp::socket socket;
};


#endif
