#ifndef SENDER_H
#define SENDER_H

#include <mutex>

#include "Packet.hpp"
#include "Common.hpp"

class Sender
{
private:

	Sender() :
		_sendSocketLogin(0), _sendSocketMap(0), _sendSocketChat(0), _sendSocketBattle(0){}
public:
	static Sender* Get() {
		if (!_instance) {
			_instance = new Sender();
		}
		return _instance;
	}

	inline void setSendSocketLogin(int sendSocket) { _sendSocketLogin = sendSocket; }
	inline void setSendSocketMap(int sendSocket) { _sendSocketMap = sendSocket; }
	inline void setSendSocketChat(int sendSocket) { _sendSocketChat = sendSocket; }
	inline void setSendSocketBattle(int sendSocket) { _sendSocketBattle = sendSocket; }

	void Send(const char* pData, size_t size, SERVER_TYPE type);
	void Send(Packet& data, SERVER_TYPE type);
	void Send(PacketPacker& data, SERVER_TYPE type);

	inline std::mutex& acquire() { return _sendMutext; }
	inline int getSocketLogin() { return _sendSocketLogin; }
	inline int getSocketMap() { return _sendSocketMap; }
	inline int getSocketChat() { return _sendSocketChat; }
	inline int getSocketBattle() { return _sendSocketBattle; }

private:
	static Sender* _instance;
	int _sendSocketLogin;
	int _sendSocketMap;
	int _sendSocketChat;
	int _sendSocketBattle;
	std::mutex _sendMutext;

	void SendPacket(const char* pData, size_t size, SERVER_TYPE type);
};

#endif