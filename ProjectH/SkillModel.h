#pragma once

struct SkillModel {
	int id;
	int level;
	int levelreq;
	char at;
	char range;

	SkillModel() :
		id(0), level(0), levelreq(0), at(0), range(0) {}
};

