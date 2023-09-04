#ifndef PREMOVECARD_H
#define PREMOVECARD_H

#include "PacketData.h"

class pRemoveCard : public PacketData {
public:
	pRemoveCard(int slot, int type, int id, int handle, int amount)
		: PacketData(161, 32)
	{
		AddInt(slot);
		AddInt(0);//??
		AddInt(type);//card type 0 defensive 1 offensive
		AddInt(handle);
		AddInt(id);
		AddInt(amount);
	}
};

#endif
