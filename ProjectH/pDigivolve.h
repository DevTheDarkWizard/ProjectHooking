#ifndef PDIGIVOLVE_H
#define PDIGIVOLVE_H

#include "PacketData.h"

class pDigivolve : public PacketData {
public:
	pDigivolve(int stage, int id)
		: PacketData(129, 16)
	{
		AddInt(stage);
		AddInt(id);
	}
};

#endif
