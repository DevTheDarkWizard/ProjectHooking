#ifndef PGETCARDDROP_H
#define PGETCARDDROP_H

#include "PacketData.h"

class pGetCardDrop : public PacketData {
public:
	pGetCardDrop(int id, int owner, int handle, int amount)
		: PacketData(144, 64)
	{
		AddByte(1);
		AddByte(3);
		AddByte(0);
		AddByte(0);
		AddInt(0x16);//Column 1
		AddInt(0x91);//Slot 1
		AddInt(0);//Column 2
		AddInt(0);//Slot 2
		AddInt(handle);
		AddInt(owner);
		AddInt(id);
		AddInt(amount);
		AddInt(0);
		AddInt(0);
		AddInt(0);
		AddInt(0);
		AddInt(0);
	}
};

#endif
