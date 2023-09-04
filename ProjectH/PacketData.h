#ifndef PACKET_DATA_H
#define PACKET_DATA_H

#include "PacketPacker.h"

class PacketData : public PacketPacker
{
public:
	PacketData(short type, int length) :
		PacketPacker(type,length)
	{
		AddByte((char)0xCC);
		AddShort(type);
		AddByte(0);
		AddInt(length);
	}
};

#endif
