#pragma once
#include <iostream>

struct EntityPosition {
	short x;
	short y;
	short z;
	char dir;
	EntityPosition() :
		EntityPosition(0, 0, 0, 0) {}

	EntityPosition(short x, short y, short z) :
		x(x), y(y), z(z), dir(0) {}
	EntityPosition(short x, short y, short z, char dir) :
		x(x), y(y), z(z), dir(dir) {}
};

struct EntityModel {
	int type;
	int id;
	short model;
	const char* rid;
	const char* name;
	int amount;

	EntityModel()  :
		id(0), model(0), name(NULL), type(0), amount(0), rid(NULL) {}
	EntityModel(int type, int amount) :
		id(0), model(0), name(NULL), type(type), amount(amount), rid(NULL) {}
	EntityModel(char model, int type, int amount) :
		id(0), model(model), name(NULL), type(type), amount(amount), rid(NULL) {}
	EntityModel(char model, const char* name, int type, int amount) :
		id(0), model(model), name((char*)name), type(type), amount(amount), rid(NULL) {}

	void disponseBase() {
		//delete[] rid;
		//delete[] name;
	}
};