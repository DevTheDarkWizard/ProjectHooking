#pragma once
#include "Common.hpp"
#include "SkillModel.h"

struct DigimonModel : public EntityModel {
	int wins;
	int losses;
	char motion;
	char stage;
	char attribute;
	short level;
	int exp;
	int mexp;
	int str;
	int dex;
	int con;
	int inte;
	int hp;
	int mhp;
	int vp;
	int mvp;
	int evp;
	int mevp;
	int att;
	int def;
	int tp;
	int mtp;
	int battlelevel;
	short statpoints;
	short skillpoints;
	char traits[4];
	char unlockedStages;
	char mutant;
	int mutantTime;
	SkillModel skills[2];
	EntityPosition pos;

	DigimonModel() :
		EntityModel(),
		pos(0, 0, 0),
		level(0), exp(0), mexp(0), tp(0), mtp(0), statpoints(0), skillpoints(0),
		wins(0), losses(0), motion(0), stage(0), attribute(0),
		hp(0),mhp(0),vp(0),mvp(0),evp(0),mevp(0),att(0),def(0),battlelevel(0),
		unlockedStages(0),str(0),dex(0),con(0),inte(0),mutant(0),mutantTime(0) {
		memset(&traits, 0, std::size(traits));
	}

	void disponse() {
		disponseBase();
	}
};

