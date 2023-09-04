#include "Sender.h"

Sender* Sender::_instance = nullptr;

void Sender::Send(const char* packet, size_t size, SERVER_TYPE type)
{
	SendPacket(packet, size, type);
}
void Sender::Send(Packet& data, SERVER_TYPE type)
{
	SendPacket(data.Reader().Data(), data.GetTotalLength(), type);

	//data->disponse();
	//delete data;
}

void Sender::Send(PacketPacker& data, SERVER_TYPE type)
{
	SendPacket(data.Data(), data.Size(), type);
	//delete data;
}

void Sender::SendPacket(const char* packet, size_t size, SERVER_TYPE type)
{
	//std::lock_guard<std::mutex> lock(_sendMutext);

	int sendSocket = _sendSocketLogin;
	if (type == SERVER_TYPE::MAP) sendSocket = _sendSocketMap;
	if (type == SERVER_TYPE::CHAT) sendSocket = _sendSocketChat;
	if (type == SERVER_TYPE::BATTLE) sendSocket = _sendSocketBattle;

	if (sendSocket) {
		(*Hooked_Send)(sendSocket, packet, size, 0);
	}
}