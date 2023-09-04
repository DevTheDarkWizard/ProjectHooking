#pragma once

#include "Common.hpp"

struct BuffModel : public EntityModel {
	int time;

	BuffModel() :
		EntityModel(0, NULL, 1, 1, 0, 0, 0), time(0) {}

	void disponse() {
		deleteEntity();
	}
};