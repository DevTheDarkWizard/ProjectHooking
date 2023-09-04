#include "AttackModule.h"

bool isInvisible(char motion) {
	return (motion == 8 /*Dead*/ || motion == 11/*In Battle*/ || motion == 12 /*Invisible*/);
}

void AttackModule::onDisponse()
{
	std::lock_guard<std::mutex> lock(_entitiesMutex);

	for (auto&& it : _entities)
	{
		it.second->disponse();
		delete it.second;
	}

	_entities.clear();

	_battle->disponse();

	delete _battle;
}

void AttackModule::onSend(Packet& packet) 
{
	int type = packet.GetType();
	packet.Seek(8);

	if (Sender::Get()->getSocketMap()) {
		switch (type) {
		case 241://Change partner
		{
			break;
		}
		case 180://Refresh digimon slot
		{
			break;
		}
		case 179://Capture digimon
		{
			break;
		}
		case 129://Digivolve in _battle
		{
			_isWaitingAttack = false;
			break;
		}
		case 106://Exit _battle
		{

			break;
		}
		case 105://Send check ready
		{
			_isWaitingAttack = false;
			break;
		}
		case 102://Send Attack
		{

			break;
		}

		case 98://Start _battle confirmation
		{
			break;
		}
		case 97://Arrange _battle
		{
			break;
		}
		}
	}
	if (isWaitingSendPacket() == type) {
		setWaitingSendPacketID(0);
	}
}

void AttackModule::onRecv(Packet& packet)
{
	std::lock_guard<std::mutex> entityLock(_entitiesMutex);

	int type = packet.GetType();
	packet.Seek(8);

	if (Sender::Get()->getSocketMap()||true) {
		switch (type) {
		case 241://Change partner
		{
			break;
		}
		case 180://Refresh digimon slot
		{
			//printf("RECV: %d\n", type);
			const char* rid = packet.Reader().GetString(16,8);

			DigimonModel* digimon = _player->getDigimon(rid);
			if (digimon!= NULL) ReadDigimon(packet, *digimon);

			break;
		}
		case 179://Capture digimon
		{
			break;
		}
		case 178://Refresh digimon stats
		{
			//printf("RECV: %d\n", type);
			const char* rid = packet.Reader().GetString(16,8);

			DigimonModel* digimon = _player->getDigimon(rid);
			if(digimon != NULL) ReadDigimon(packet, *digimon);

			break;
		}
		case 177://Refresh tamer stats end battle
		{
			_player->getCharacter().rank = packet.Reader().GetByte();
			packet.Skip(1);
			_player->getCharacter().level = packet.Reader().GetShort();
			_player->getCharacter().fame = packet.Reader().GetInt();
			_player->getCharacter().exp = packet.Reader().GetInt();
			_player->getCharacter().mexp = packet.Reader().GetInt();
			_player->getCharacter().wins = packet.Reader().GetInt();
			_player->getCharacter().losses = packet.Reader().GetInt();
			_player->getCharacter().losses = _player->getCharacter().losses - _player->getCharacter().wins;
			_player->getCharacter().pvpPoints = packet.Reader().GetInt();
			
			//if (_isInBattle) {
				_lastAction = high_resolution_clock::now();
				_endBattleInterval = std::chrono::milliseconds(DELAY_AFTER_BATTLE);

				_battle->isPaused = false;
				_battle->isStared = false;
				_battle->winner = 0;
				_battle->rid = NULL;

				_isInBattle = false;
				_isWalking = false;
				_isChallanging = false;
				_target = NULL;

				_player->setWalkTo(0, 0, 0);
				_player->setBusy(false);
			//}

			break;
		}
		case 129://Digivolve in battle
		{
			const char* rid = packet.Reader().GetString(16, 12);

			packet.Seek(12);

			DigimonModel* digimon = _battle->getDigimon(2, rid);
			if (digimon == NULL) digimon = _battle->getDigimon(1, rid);
			if (digimon != NULL) ReadDigimon(packet, *digimon, true);

			packet.Seek(12);

			DigimonModel* digimonP = _player->getDigimon(rid);
			if (digimonP) ReadDigimon(packet, *digimonP);

			break;
		}
		case 112://Level up digimon
		{
			const char* rid = packet.Reader().GetString(16, 8);

			DigimonModel* digimon = _battle->getDigimon(2, rid);
			if (digimon == NULL) digimon = _battle->getDigimon(1, rid);
			if(digimon != NULL) ReadDigimon(packet, *digimon, true);

			packet.Seek(8);

			DigimonModel* digimonP = _player->getDigimon(rid);
			if (digimonP) ReadDigimon(packet, *digimonP);

			break;
		}
		case 110://Add exp
		{
			const char* rid = packet.Reader().GetString(16);
			int exp = packet.Reader().GetInt();
			int bits = packet.Reader().GetInt();

			DigimonModel* digimonP = _player->getDigimon(rid);
			if (digimonP != NULL) digimonP->exp += exp;

			_player->getCharacter().exp += exp;
			_player->getCharacter().bits += bits;

			break;
		}
		case 109://End battle
		{
			//printf("RECV: %d\n", type);
			if (packet.GetTotalLength() == 8) {
				_battle->winner = 1;
			}
			else {
				char result = packet.Reader().GetByte();
				_battle->winner = 2;
			}

			auto it = _entities.find(_target);

			if (it != _entities.end()) {
				it->second->motion = 8;
				//it->second->disponse();
				//delete it->second;
			}

			_battle->isStared = false;

			_target = NULL;

			break;
		}
		case 107://Refresh mob tp
		{
			const char* rid = packet.Reader().GetString(16);
			int tp = packet.Reader().GetInt();
			int mtp = packet.Reader().GetInt();

			if(_battle->yourTeam == 2) _battle->refreshTP(1, rid, tp, mtp);
			else _battle->refreshTP(2, rid, tp, mtp);

			break;
		}
		case 105://Continue _battle
		{
			//printf("RECV: %d\n", type);
			_battle->isPaused = false;
			break;
		}
		case 104://Pause _battle
		{
			//printf("RECV: %d\n", type);
			_battle->isPaused = true;
			break;
		}
		case 103://Attack
		{
			printf("RECV: %d\n", type);

			int teamOffset = ((64 * 5) * 2) + 8;
			int team = packet.Reader().GetInt(teamOffset);
			int skillId = packet.Reader().GetInt(teamOffset+4);
			int usedCards = packet.Reader().GetInt(teamOffset+8);

			for (int t = 2; t > 0; t--)
			{
				for (int i = 0; i < TEAM_SIZE; i++)
				{
					const char* rid = packet.Reader().GetString(16);

					if (!std::strlen(rid)) {
						packet.Skip(48);
						continue;
					}

					DigimonModel* digimon = _battle->getDigimon(team, rid);
					if (digimon == NULL) {
						printf("DIGIMON NOT FOUND: ");
						ShowPacket(rid, 16);
						packet.Skip(48);
						continue;
					}

					digimon->hp = packet.Reader().GetInt();
					digimon->vp = packet.Reader().GetInt();
					digimon->evp = packet.Reader().GetInt();

					if (digimon->id)
						printf("Digimon: %s (LV: %d) HP: %d / %d\n", digimon->name, digimon->level, digimon->hp, digimon->mhp);

					char attackType = packet.Reader().GetByte();

					packet.Skip(3);

					int card1 = packet.Reader().GetInt();
					int card2 = packet.Reader().GetInt();
					int card3 = packet.Reader().GetInt();

					int card1Amount = packet.Reader().GetInt();
					int card2Amount = packet.Reader().GetInt();
					int card3Amount = packet.Reader().GetInt();

					int activedTrait = packet.Reader().GetInt();

					packet.Skip(4);
				}
				if (team == 1) team = 2;
				else if (team == 2) team = 1;
			}

			break;
		}
		case 101://Retire from _battle
		{
			//printf("RECV: %d\n", type);
			bool retire = false;
			if (packet.GetTotalLength() > 8) {
				packet.Skip(4);
				retire = true;
			}

			const char* rid = packet.Reader().GetString(16, 8);
			
			DigimonModel* digimon = _player->getDigimon(rid);
			if (digimon != NULL) ReadDigimon(packet, *digimon, retire);

			_battle->removeDigimon(1, rid);
			_battle->removeDigimon(2, rid);

			break;
		}
		case 99://Battle Data
		{
			//printf("RECV: %d\n", type);

			for (int i = 0; i < TEAM_SIZE; i++) {
				DigimonModel* digimon = new DigimonModel();
				ReadDigimon(packet, *digimon);
				if (digimon->id) {
					printf("ENEMY: %s (LV:%d) ID: %d RID: ",
						digimon->name,
						digimon->level,
						digimon->id
					);
					ShowPacket(digimon->rid, 16);
					_battle->addDigimon(1, digimon);
				}
			}

			printf("\n");

			for (int i = 0; i < TEAM_SIZE; i++) {
				DigimonModel* digimon = new DigimonModel();
				ReadDigimon(packet, *digimon);
				if (digimon->id) {
					printf("ALLY: %s (LV:%d) ID: %d RID: ",
						digimon->name,
						digimon->level,
						digimon->id
					);
					ShowPacket(digimon->rid, 16);
					_battle->addDigimon(2, digimon);
				}
			}

			printf("\n");

			_battle->isStared = true;

			break;
		}
		case 98://Start _battle confirmation
		{
			printf("RECV: %d\n", type);
			const char* rid = packet.Reader().GetString(16);
			_battle->reset(rid);

			char enemyTeam = packet.Reader().GetByte();
			char yourTeam = packet.Reader().GetByte();

			_battle->setTeams(yourTeam, enemyTeam);
			_player->setWalkTo(0, 0, 0);

			_target = NULL;

			_isWaitingAttack = false;
			_player->setWalkTo(0, 0, 0);
			

			break;
		}
		case 97://Cannot fight
		{
			auto it = _entities.find(_target);

			if (it != _entities.end()) {
				it->second->motion = 8;
				//it->second->disponse();
				//delete it->second;
			}

			_target = NULL;

			_lastAction = high_resolution_clock::now();
			_endBattleInterval = std::chrono::milliseconds(DELAY_AFTER_BATTLE);
			_challangeInterval = std::chrono::milliseconds(DELAY_ARRANGE_BATTLE_FAIL);
			_isInBattle = false;
			_isWaitingAttack = false;
			_player->setWalkTo(0, 0, 0);
			_player->setBusy(false);

			printf("Fail to arrange battle!\n");
			break;
		}
		case 2:
		case 176://Change map
		{
			for (auto&& it : _entities)
			{
				it.second->disponse();
				delete it.second;
			}

			_entities.clear();

			if (_target && (_isWalking || _isInBattle || _isChallanging)) _player->setBusy(false);

			_target = NULL;

			_battle->reset();

			_isWalking = false;
			_isChallanging = false;
			_isInBattle = false;
			_isWaitingAttack = false;
			
			setWaitingRecvPacketID(0);

			break;
		}
		case 17://Refresh Mobs in Map
		{
			int id = packet.Reader().GetInt();
			const char* rid = packet.Reader().GetString(16);
			const char* name = packet.Reader().GetString(20);

			packet.Reader().GetInt();
			packet.Reader().GetInt();

			short model = packet.Reader().GetShort();
			short level = packet.Reader().GetShort();
			char motion = packet.Reader().GetByte();
			char dir = packet.Reader().GetByte();
			short x = packet.Reader().GetShort();
			short y = packet.Reader().GetShort();

			auto it = _entities.find(id);

			if (it != _entities.end())
			{
				if (isInvisible(motion)|| _lasttarget == id || !it->second->id || it->second->id != id || !std::strlen(it->second->rid)) {
					it->second->disponse();
					delete it->second;
					return;
				}

				it->second->pos.dir = dir;
				it->second->pos.x = x;
				it->second->pos.y = y;
				it->second->motion = motion;
			}
			else if(!isInvisible(motion) && id > 0 && std::strlen(rid) && std::strlen(name) && _lasttarget != id) {

				auto mob = new DigimonModel();

				mob->id = id;
				mob->rid = rid;
				mob->name = name;
				mob->model = model;
				mob->level = level;
				mob->motion = motion;

				mob->pos.dir = dir;
				mob->pos.x = x;
				mob->pos.y = y;

				_entities.emplace(id, mob);
			}

			break;
		}

		}
	}
	if (isWaitingRecvPacket() == type) {
		setWaitingRecvPacketID(0);
	}
}


void AttackModule::arrangeBattle(const char* rid, const int id)
{
	auto packet = pArrageBattle(rid, id);
	Sender::Get()->Send(packet.Data(),packet.Size(), SERVER_TYPE::MAP);
}

void AttackModule::attack(const char* bRID, const char* aRID, const char* vRID, const int skillID, const int skill)
{
	int time = getUnixTime() - _player->getCharacter().time;
	auto packet = pBattleAttack(bRID, aRID, vRID, skillID, skill, time);

	//ShowPacket(packet.Data(), packet.Size());

	Sender::Get()->Send(packet.Data(), packet.Size(), SERVER_TYPE::MAP);
}

void AttackModule::digivolve(int stage, int id)
{
	auto packet = pDigivolve(stage , id);
	Sender::Get()->Send(packet.Data(), packet.Size(), SERVER_TYPE::MAP);
}

void AttackModule::update()
{
	if (Sender::Get()->getSocketMap()) {
		if (_player->getGetLoot() || _player->getUpdateMap() || _player->isInLoading()) return;

		std::tuple<int, int, int> walk = _player->getWalkTo();
		int x = std::get<0>(walk);
		int y = std::get<1>(walk);

		if (x && y) return;

		if (!_player->isBusy())
		{
			std::lock_guard<std::mutex> entityLock(_entitiesMutex);

			printf("BUSY: %d WALKING: %d CHALLANGING: %d BATTLE: %d TARGET: %d WAITINGATTACK: %d\n",
				_player->isBusy(), _isWalking, _isChallanging, _isInBattle, _target, _isWaitingAttack);
			printf("BATTLE STARTED: %d ISPAUSE: %d WINNER: %d\n", _battle->isStared, !_battle->isPaused, _battle->winner);

			if (std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - _lastAction) > _endBattleInterval)
			{
				if (_endBattleInterval.count() > 0) _endBattleInterval = std::chrono::milliseconds(0);

				double dist = MAX_DISTANCE;

				_target = NULL;

				for (auto&& it : _entities)
				{
					double currentDist = _player->getDistance(it.second->pos);

					//printf("MOB: %d Pos: %d,%d Dist: %d\n", it.first, it.second->pos.x, it.second->pos.y, currentDist);

					if (currentDist >= MAX_VIEW_DISTANCE || _lasttarget == it.second->id|| isInvisible(it.second->motion) || !it.second->id || !std::strlen(it.second->rid) || it.second->id == _player->getPartner()->id) {
						//printf("REMOVING: %d\n", it.first);
						it.second->disponse();
						delete it.second;
					}
					else if (currentDist >= 0 && currentDist < dist)
					{
						dist = currentDist;
						_target = it.second->id;
					}
				}


				if (_target && dist < MAX_VIEW_DISTANCE) {
					//printf("TARGET: %d Pos: %d,%d Dist: %d\n", _target, _entities[_target]->pos.x, _entities[_target]->pos.y, dist);
					_player->setBusy(true);
					_isWalking = true;
					//_lasttarget = _target;
				}

			}
		}

		if (_isInBattle) {
			/*printf("START: %d PAUSE:%d WINNER: %d\n",
				_battle->isStared, _battle->isPaused, _battle->winner
			);*/
			if (std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - _lastAction) > _attackInterval)
			{
				if (_battle->isStared && !_battle->isPaused && _battle->winner == 0 && !_isWaitingAttack) {
					std::tuple<int, int> digimon1TP = getDigimon1TP();
					//printf("Digimon 1 TP: %d\n", ((std::get<0>(digimon1TP) * 100) / std::get<1>(digimon1TP)));
					if (std::get<0>(digimon1TP) >= std::get<1>(digimon1TP))
					{
						DigimonModel* attacker = _player->getPartner();
						DigimonModel* victim = NULL;

						if (attacker->unlockedStages > 2 && attacker->stage < attacker->unlockedStages) {
							int stage = 0;
							if (attacker->unlockedStages == 5 && attacker->evp >= 80) stage = 5;
							else if (attacker->unlockedStages >= 4 && attacker->evp >= 40) stage = 4;
							else if (attacker->unlockedStages >= 3 && attacker->evp >= 20) stage = 3;

							if (stage) {

								_isWaitingAttack = true;

								digivolve(attacker->unlockedStages, attacker->id);

								_lastAction = high_resolution_clock::now();
								return;
							}
						}

						if (_battle->yourTeam == 2) victim = *_battle->getLessHP(1);
						else victim = *_battle->getLessHP(2);

						if (victim != NULL) {

							int skillID = attacker->skills[1].id;
							int selectSkill = 2;
							int reqVP = (attacker->stage == 1 ? 5 : 0) + ((attacker->stage + selectSkill) * 5) + ((attacker->stage - 1) * 5) + 11;
							
							if (attacker->stage == 1 || attacker->vp < reqVP || attacker->level < attacker->skills[1].levelreq) {
								selectSkill = 1;
								skillID = attacker->skills[0].id;
								reqVP = (attacker->stage == 1 ? 5 : 0) + ((attacker->stage + selectSkill) * 5) + ((attacker->stage - 1) * 5) + 11;
								if (attacker->vp < reqVP || attacker->level < attacker->skills[0].levelreq) {
									selectSkill = 0;
									skillID = 0;
								}
							}

							const char* bRID = _battle->rid;
							const char* aRID = attacker->rid;
							const char* vRID = victim->rid;

							printf("Attacker: %s (LV: %d)\nVictim: %s (LV: %d) RID: ",
								attacker->name, attacker->level,
								victim->name, victim->level
							);

							//ShowPacket(vRID, 16);

							_isWaitingAttack = true;

							attack(bRID, aRID, vRID, skillID, selectSkill);

							_lastAction = high_resolution_clock::now();
							return;
						}
						else {
							printf("No one to attack!\n");
						}
					}
				}
			}
		}

		if (_isChallanging)
		{
			if (std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - _lastAction) > _challangeInterval)
			{
				if (_challangeInterval.count() != DELAY_CHALLANGE) _challangeInterval = std::chrono::milliseconds(DELAY_CHALLANGE);

				auto it = _entities.find(_target);

				if (it == _entities.end()|| 
					it != _entities.end() && (it->second->id == _player->getPartner()->id || isInvisible(it->second->motion) || !it->second->id ||!std::strlen(it->second->rid))) {
					_target = NULL;
					_isChallanging = false;
					_player->setBusy(false);
					return;
				}

				double currentDist = _player->getDistance(it->second->pos);
				
				if (currentDist <= BATTLE_DISTANCE)
				{
					if (!_player->isWalking()) {
						if (!_battle->isStared) {
							printf("TARGET: %d, MOTION: %d\n", _target, it->second->motion);
							printf("Arrange Battle with %s ID: %d RID:\n", it->second->name, it->second->id);
							ShowPacket(it->second->rid, 16);

							_lastAction = high_resolution_clock::now();
							_isChallanging = false;
							_isInBattle = true;

							arrangeBattle(it->second->rid, it->second->id);
						}
						else {
							_isChallanging = false;
							_isInBattle = true;
						}
					}
				}
				else
				{
					_target = NULL;
					_isChallanging = false;
					_player->setBusy(false);
				}
			}
		}

		if (_isWalking)
		{
			if (std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - _lastAction) > _walkInterval)
			{
				auto it = _entities.find(_target);
				if (it == _entities.end() || 
					it != _entities.end() && (it->second->id == _player->getPartner()->id || !it->second->id || !std::strlen(it->second->rid) || isInvisible(it->second->motion))) {
					_target = NULL;
					_isChallanging = false;
					_player->setBusy(false);
					return;
				}

				double currentDist = _player->getDistance(it->second->pos);

				if (currentDist < MAX_VIEW_DISTANCE)
				{
					if (currentDist <= BATTLE_DISTANCE)
					{
						_isChallanging = true;
						_isWalking = false;
					}
					else
					{
						if (!_player->isWalking()) {
							int randX = randNumber(-1, 1);
							int randY = randNumber(-1, 1);

							if (!randX && !randY) {
								randX = randNumber(0, 1) ? 1 : -1;
								randY = randNumber(0, 1) ? 1 : -1;
							}

							if (!_player->canMove(it->second->pos.x + randX, it->second->pos.y + randY)) {
								randX = 0;
								randY = 0;
							}
							

							if (_player->canMove(it->second->pos.x + randX, it->second->pos.y + randY)) {
								if (currentDist > (MAX_VIEW_DISTANCE/2)+3) {
									_player->setWalkTo(it->second->pos.x + randX, it->second->pos.y + randY, 0);
									_isWalking = false;
									_player->setBusy(false);
								}
								else _player->moveTo(it->second->pos.x + randX, it->second->pos.y + randY);
							}
						}
					}

					_lastAction = high_resolution_clock::now();
				}
				else
				{
					_target = NULL;
					_isWalking = false;
					_player->setBusy(false);
				}
			}
		}
	}
}

std::tuple<int,int> AttackModule::getDigimon1TP() {
	DWORD tp = (DWORD)(baseAddress + digimon1TPAddress);
	DWORD mtp = (DWORD)(baseAddress + digimon1MTPAddress);
	return std::tuple<int,int>(*(int*)tp,*(int*)mtp);
}
std::tuple<int, int> AttackModule::getDigimon2TP() {
	DWORD tp = (DWORD)(baseAddress + digimon2TPAddress);
	DWORD mtp = (DWORD)(baseAddress + digimon2TPAddress);
	return std::tuple<int, int>(*(int*)tp, *(int*)mtp);
}
std::tuple<int, int> AttackModule::getDigimon3TP() {
	DWORD tp = (DWORD)(baseAddress + digimon3TPAddress);
	DWORD mtp = (DWORD)(baseAddress + digimon3TPAddress);
	return std::tuple<int, int>(*(int*)tp, *(int*)mtp);
}
std::tuple<int, int> AttackModule::getDigimon4TP() {
	DWORD tp = (DWORD)(baseAddress + digimon4TPAddress);
	DWORD mtp = (DWORD)(baseAddress + digimon4TPAddress);
	return std::tuple<int, int>(*(int*)tp, *(int*)mtp);
}
std::tuple<int, int> AttackModule::getDigimon5TP() {
	DWORD tp = (DWORD)(baseAddress + digimon5TPAddress);
	DWORD mtp = (DWORD)(baseAddress + digimon5TPAddress);
	return std::tuple<int, int>(*(int*)tp, *(int*)mtp);
}
