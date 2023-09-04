#ifndef LOOT_MODULE_H
#define LOOT_MODULE_H

#include <mutex>
#include <tuple>
#include <chrono>

#include "Module.hpp"
#include "PlayerModule.h"
#include "Sender.h"
#include "Packets.h"

class LootModule : public Module
{
	using high_resolution_clock = std::chrono::high_resolution_clock;

public:
	LootModule(PlayerModule* player) :
		Module(true),
		_target(NULL),
		_walkInterval(150),
		_player(player), _isWalking(false)
	{}

	void update() override;
	void onSend(Packet& packet) override;
	void onRecv(Packet& packet) override;
	void onDisponse() override;

	inline bool isWalking() { return _player->isWalking(); }

private:
	void get(unsigned int id);

private:
	PlayerModule* _player;

	unsigned int _target;

	bool _isWalking = false;

	std::mutex _entitiesMutex;

	std::map<unsigned int, ItemModel*> _entities;

	std::mutex _lootMutex;
	high_resolution_clock::time_point _lastAction;
	std::chrono::milliseconds _walkInterval;
};

#endif

