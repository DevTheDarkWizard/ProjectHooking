#ifndef PLAYER_MODULE_H
#define PLAYER_MODULE_H

#include "Module.hpp"
#include "Sender.h"
#include "CharacterModel.h"
#include "Pathfinder.hpp"

#define TEAM_SIZE 5
#define DEFAULT_WALKTIME 0
#define MAX_VIEW_DISTANCE 12
#define MAX_DISTANCE 20
#define BATTLE_DISTANCE 2
#define GET_DISTANCE 2
#define DELAY_ATTACK 100
#define DELAY_WALK 300
#define DELAY_CHALLANGE 300
#define DELAY_AFTER_BATTLE 2100
#define DELAY_ARRANGE_BATTLE_FAIL 2100

class PlayerModule : public Module {
private:
	bool _cancelAction;
	bool _isBusy;
	bool _loginFailed;
	bool _updateMap; 
	bool _getLoot;
	char _battleTeam;
	int _waitForRecvPacketID;
	int _waitForSendPacketID;
	std::tuple<int, int, int> _walkTo;
	CharacterModel* _character;
	Pathfinder _pathfinder;

public:

	PlayerModule() :
		Module(1, 1), _walkTo(0,0,0),
		_isBusy(false),_battleTeam(0),
		_character(new CharacterModel()),
		_waitForRecvPacketID(0),_waitForSendPacketID(0) {
		_isEnabled = true;
	}

	virtual void update() override;
	virtual void onSend(Packet& packet) override;
	virtual void onRecv(Packet& packet) override;
	virtual void onDisponse() override;

	void moveTo(int x, int y);

	inline DigimonModel* getDigimon(int slot) { return _character->digimons[slot]; }
	inline DigimonModel* getPartner() { return _character->digimons[0]; }
	DigimonModel* getDigimon(const char* rid) { 
		for (int i = 0; i < 5; i++) {
			if (_character->digimons[i]->rid != NULL && !memcmp(_character->digimons[i]->rid, rid, 16)) {
				return _character->digimons[i];
			}
		}
		return NULL;
	}
	inline EntityPosition& getPosition() { return _character->pos; }
	inline CharacterModel& getCharacter() { return *_character; }
	inline Pathfinder& getPathfinder() { return _pathfinder; }
	inline bool canMove(int x, int y) { return _pathfinder.checkCanMove(x, y); }

	inline void setWalkTo(int x, int y, int z) { _walkTo = std::tuple<int, int, int>(x, y, z); }
	inline std::tuple<int, int, int> getWalkTo() { return _walkTo; }

	inline void setUpdateMap(bool value) { _updateMap = value; }
	inline bool getUpdateMap() { return _updateMap; }

	inline void setGetLoot(bool value) { _getLoot = value; }
	inline bool getGetLoot() { return _getLoot; }

	inline void setBattleTeam(char value) { _battleTeam = value; }
	inline bool getBattleTeam() { return _battleTeam; }

	void setPosition(int x, int y, int z = 0);
	inline void setBusy(bool busy) { _isBusy = busy; }
	inline bool isBusy() { return _isBusy; }

	inline double getDistance(int x, int y) { return distance(_character->pos.x,_character->pos.y,x,y); }
	inline double getDistance(EntityPosition& other) { return distance(_character->pos.x,_character->pos.y, other.x, other.y); }
	inline bool getCancelAction() { return _cancelAction; }
	inline void setCancelAction(bool value) { _cancelAction = value; }
	
	bool isInLoading();
	bool isWalking();
	bool isAtInitialScreen();
	bool isAtLogin();
	bool isAtCharacterSelection();
	bool isAtMap();

private:
	inline void setWaitingRecvPacketID(int id) { _waitForRecvPacketID = id; }
	inline int isWaitingRecvPacket() { return _waitForRecvPacketID; }

	inline void setWaitingSendPacketID(int id) { _waitForSendPacketID = id; }
	inline int isWaitingSendPacket() { return _waitForSendPacketID; }
};

#endif