#ifndef MODULE_H
#define MODULE_H

#include "Packet.hpp"
#include "Common.hpp"
#include <windows.h>

class Module : public EntityModel {
protected:
	bool _isEnabled;

public:
	Module(bool enabled = false) : _isEnabled(enabled),
		EntityModel(0, NULL, 0, 0) {}

	Module(int type, int amount) :
		_isEnabled(false),
		EntityModel(0, NULL, type, amount) {}

	Module(char model, int type, int amount) :
		_isEnabled(false),
		EntityModel(model, NULL, type, amount) {}
	Module(char model, const char* name, int type, int amount) :
		_isEnabled(false),
		EntityModel(model, name, type, amount) {}

	Module(int type, int amount, short x, short y, short z) :
		_isEnabled(false),
		EntityModel(0, NULL, type, amount) {}

	Module(char model, int type, int amount, short x, short y, short z) :
		_isEnabled(false),
		EntityModel(model, NULL, type, amount) {}

	Module(char model, const char* name, int type, int amount, short x, short y, short z) :
		_isEnabled(false),
		EntityModel(model, name, type, amount) {}

	inline void enable() { _isEnabled = true; }
	inline void disable() { _isEnabled = false; }
	inline void toggle() { _isEnabled = !_isEnabled; }
	inline bool isEnabled() { return _isEnabled; }

	virtual void update() {}
	virtual void onSend(Packet& packet) = 0;
	virtual void onRecv(Packet& packet) = 0;
	virtual void onDisponse() = 0;
};

#endif