#ifndef PADDCARD_H
#define PADDCARD_H

#include "PacketData.h"

class pAddCard : public PacketData {
public:
	pAddCard(int slot, int type,int id, int handle,int amount)
		: PacketData(160, 32)
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
