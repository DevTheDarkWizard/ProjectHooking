#ifndef PARRANGE_BATTLE_H
#define PARRANGE_BATTLE_H

#include "PacketData.h"

class pArrageBattle : public PacketData {
public:
	pArrageBattle(const char* rid, const int id)
		: PacketData(97, 28)
	{
		AddInt(1);//PVE Battle
		AddInt(id);
		AddString(rid,12);
	}
};

#endif
