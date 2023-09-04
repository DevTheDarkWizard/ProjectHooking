#pragma once

#include "Common.hpp"

struct ItemModel : public EntityModel {
	int handle;
	int owner;
	EntityPosition pos;

	ItemModel() : 
		EntityModel(0,0,0),
		pos(0, 0, 0), handle(0), owner(0) {}

	void disponse() {
		disponseBase();
	}
};