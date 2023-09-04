#pragma once

#include "Common.hpp"
#include "Packet.hpp"
#include "CharacterModel.h"
#include "DigimonModel.h"

#define TEAM_SIZE 5

class BattleModel {
	std::mutex _lockMutex;
	std::map<int, DigimonModel*> _teamA;
	std::map<int, DigimonModel*> _teamB;

public:
	bool isPaused;
	bool isStared;
	char winner;
	const char* rid;
	
	char yourTeam;
	char enemyTeam;

public:
	void reset(const char* newRID = NULL)
	{
		std::lock_guard<std::mutex> entityLock(_lockMutex);

		rid = newRID;
		isPaused = false;
		winner = 0;
		
		for (auto&& it : _teamA) {
			it.second->disponse();
			delete it.second;
		}

		for (auto&& it : _teamB) {
			it.second->disponse();
			delete it.second;
		}

		_teamA.clear();
		_teamB.clear();
	}

	void disponse() 
	{
		std::lock_guard<std::mutex> entityLock(_lockMutex);

		for (auto&& it : _teamA) {
			it.second->disponse();
			delete it.second;
		}

		for (auto&& it : _teamB) {
			it.second->disponse();
			delete &it.second;
		}

		_teamA.clear();
		_teamB.clear();
	}

	void setRid(const char* newRID)
	{
		rid = newRID;
	}

	void setTeams(char you, char enemy) {
		yourTeam = you;
		enemyTeam = enemy;
	}

	DigimonModel** getLessHP(int team) {
		std::lock_guard<std::mutex> entityLock(_lockMutex);

		DigimonModel** result = NULL;
		int currentHP = MAXINT;

		std::map<int, DigimonModel*>* _team = NULL;
		if (team == 1) _team = &_teamA;
		else if (team == 2) _team = &_teamB;
		
		if (_team != NULL) {
			for (auto&& it : *_team) {
				if (std::strlen(it.second->rid) && it.second->id && it.second->hp > 0 && it.second->hp < currentHP) {
					currentHP = it.second->hp;
					result = &it.second;
				}
			}
		}

		return result;
	}

	void refreshTP(int team, const char* rid, int tp, int mtp) {
		std::lock_guard<std::mutex> entityLock(_lockMutex);

		std::map<int, DigimonModel*>* _team = NULL;
		if (team == 1) _team = &_teamA;
		else if (team == 2) _team = &_teamB;

		if (_team != NULL) {
			for (auto&& it : *_team) {
				if (std::strlen(it.second->rid) && !memcmp(it.second->rid, rid, 16)) {
					it.second->tp = tp;
					it.second->mtp = mtp;
					return;
				}
			}
		}
	}

	void addDigimon(int team, DigimonModel* d) {
		std::lock_guard<std::mutex> entityLock(_lockMutex);

		std::map<int, DigimonModel*>* _team = NULL;
		if (team == 1) _team = &_teamA;
		else if (team == 2) _team = &_teamB;

		if (_team != NULL && d != NULL && std::strlen(d->rid)) {
			_team->emplace(d->id, d);
		}
	}

	DigimonModel* getDigimon(int team, const char* rid) {
		std::lock_guard<std::mutex> entityLock(_lockMutex);

		std::map<int, DigimonModel*>* _team = NULL;
		if (team == 1) _team = &_teamA;
		else if (team == 2) _team = &_teamB;

		if (_team != NULL) {
			for (auto&& it : *_team) {
				if (!memcmp(it.second->rid, rid, 16)) {
					return it.second;
				}
			}
		}

		return NULL;
	}


	void removeDigimon(int team, const char* rid) {
		std::lock_guard<std::mutex> entityLock(_lockMutex);

		std::map<int, DigimonModel*>* _team = NULL;
		if (team == 1) _team = &_teamA;
		else if (team == 2) _team = &_teamB;

		if (_team != NULL) {
			if (_team != NULL) {
				for (auto&& it : *_team) {
					if (!memcmp(it.second->rid, rid, 16)) {
						_team->erase(it.second->id);
						return;
					}
				}
			}
		}
	}
};

