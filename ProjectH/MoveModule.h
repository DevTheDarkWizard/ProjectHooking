#ifndef MOVE_MODULE_H
#define MOVE_MODULE_H

#include <mutex>
#include <tuple>
#include <chrono>

#include "Module.hpp"
#include "PlayerModule.h"
#include "Sender.h"
#include "Packets.h"

class MoveModule : public Module
{
	using high_resolution_clock = std::chrono::high_resolution_clock;

public:
	MoveModule(PlayerModule* player) :
		Module(true),
		_walkInterval(DEFAULT_WALKTIME),
		_player(player), _isWalking(false)
	{}

	void update() override;
	void onSend(Packet& packet) override;
	void onRecv(Packet& packet) override;
	void onDisponse() override;

	inline bool isWalking() { return _player->isWalking(); }

private:
	void move(int x, int y);

private:
	PlayerModule* _player;

	bool _isReadingMap = false;
	bool _isWalking = false;
	int _lastDistance;
	int _targetMap = 0;

	std::tuple<int, int> _currentMove;
	std::tuple<int, int> _targetMove;
	std::vector<std::tuple<int, int>> _movePoints;
	std::mutex _mapMutex;

	high_resolution_clock::time_point _lastAction;
	std::chrono::milliseconds _walkInterval;
};

#endif

