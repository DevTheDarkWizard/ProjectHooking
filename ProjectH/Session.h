#ifndef SESSION_H
#define SESSION_H

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <inttypes.h>

#pragma comment( lib, "Ws2_32.lib" )

class Session
{
public:
	Session() { }

	inline void SetSocket(int socket) { _socket = socket; }
	inline int GetSocket() { return _socket; }

private:
	int _socket;
};

#endif