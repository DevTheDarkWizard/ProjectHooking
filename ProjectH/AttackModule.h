#ifndef ATTACK_MODULE_H
#define ATTACK_MODULE_H

#include <mutex>
#include <chrono>
#include <map>
#include <vector>
#include <algorithm>

#include "Module.hpp"
#include "PlayerModule.h"
#include "DigimonModel.h"
#include "BattleModel.h"
#include "Sender.h"
#include "Packets.h"

class AttackModule : public Module
{
	using high_resolution_clock = std::chrono::high_resolution_clock;
	int _waitForRecvPacketID;
	int _waitForSendPacketID;

public:
	AttackModule(PlayerModule* player) :
		Module(true),
		_walkInterval(DELAY_WALK),
		_attackInterval(DELAY_ATTACK),
		_challangeInterval(DELAY_CHALLANGE),
		_endBattleInterval(DELAY_AFTER_BATTLE),
		_lastAction(high_resolution_clock::now()),
		_isInBattle(false),
		_isWalking(false),
		_isChallanging(false),
		_isWaitingAttack(false),
		_waitForRecvPacketID(0), _waitForSendPacketID(0),
		_player(player),
		_battle(new BattleModel())
	{}

	void update() override;
	void onSend(Packet& packet) override;
	void onRecv(Packet& packet) override;
	void onDisponse() override;

private:
	void arrangeBattle(const char* rid, const int id);
	void attack(const char* bRID, const char* aRID, const char* vRID, const int skillID, const int skill);
	void digivolve(int stage, int id);

	inline void setWaitingRecvPacketID(int id) { _waitForRecvPacketID = id; }
	inline int isWaitingRecvPacket() { return _waitForRecvPacketID; }

	inline void setWaitingSendPacketID(int id) { _waitForSendPacketID = id; }
	inline int isWaitingSendPacket() { return _waitForSendPacketID; }
private:
	PlayerModule* _player;
	BattleModel* _battle;

	bool _isInBattle;
	bool _isWalking;
	bool _isChallanging;
	bool _isWaitingAttack;

	unsigned int _target;
	unsigned int _lasttarget;
	
	std::mutex _entitiesMutex;

	std::map<unsigned int, DigimonModel*> _entities;

	high_resolution_clock::time_point _lastAction;
	std::chrono::milliseconds _walkInterval;
	std::chrono::milliseconds _attackInterval;
	std::chrono::milliseconds _endBattleInterval;
	std::chrono::milliseconds _challangeInterval;

	std::tuple<int,int> getDigimon1TP();
	std::tuple<int,int> getDigimon2TP();
	std::tuple<int,int> getDigimon3TP();
	std::tuple<int,int> getDigimon4TP();
	std::tuple<int,int> getDigimon5TP();
};

#endif

