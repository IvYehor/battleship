#ifndef MESSAGE_H_INCLUDED
#define MESSAGE_H_INCLUDED

#include <cstddef>
#include <string>

template <class T>
struct Message {
	Message() : data(), data_length(sizeof(T)) {}
	Message(size_t data_length_) : data(), data_length(data_length_) {}
	Message(const T& data_, size_t data_length_) : data(data_), data_length(data_length_) {}

	T data;
	size_t data_length;
};

#endif
