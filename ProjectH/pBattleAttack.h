#ifndef PBATTLE_ATTACK_H
#define PBATTLE_ATTACK_H

#include "PacketData.h"

class pBattleAttack : public PacketData {
public:
	pBattleAttack(const char* rid,const char* arid, const char* vrid, int skillID, int skill, int time)
		: PacketData(102, 72)
	{
		AddString(rid, 16);
		AddString(arid, 16);
		AddString(vrid, 16);
		AddInt(2);
		AddInt(skillID);
		AddInt(skill);
		AddInt(time);
	}
};

#endif
