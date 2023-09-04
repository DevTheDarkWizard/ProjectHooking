#ifndef PMOVE_PLAYER_H
#define PMOVE_PLAYER_H

#include "PacketData.h"

class pMovePlayer : public PacketData {
public:
	pMovePlayer(short x, short y)
		: PacketData(32, 14)
	{
		AddShort(5);
		AddShort(x);
		AddShort(y);
	}
};

#endif
