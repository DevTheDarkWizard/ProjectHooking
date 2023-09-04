#include "LootModule.h"

void LootModule::onDisponse()
{
	std::lock_guard<std::mutex> lock(_entitiesMutex);

	for (auto&& it : _entities)
	{
		it.second->disponse();
		delete it.second;
	}

	_entities.clear();
}

void LootModule::onSend(Packet& packet)
{
}

void LootModule::onRecv(Packet& packet)
{
	int type = packet.GetType();
	packet.Seek(8);

	if (Sender::Get()->getSocketMap()) {
		switch (type) {
		case 145:
		case 144: 
		{
			std::lock_guard<std::mutex> entityLock(_entitiesMutex);

			unsigned int handle = (unsigned int)packet.Reader().GetInt(36);

			printf("TYPE: %d HANDLE: %d\n",type, handle);

			ShowPacket(packet.Data(), packet.GetTotalLength());

			auto it = _entities.find(handle);

			if (it != _entities.end())
			{
				it->second->disponse();
				delete it->second;
			}

			_target = NULL;

			break;
		}
		case 19:
		case 18:
		{
			std::lock_guard<std::mutex> entityLock(_entitiesMutex);

			unsigned int handle = packet.Reader().GetInt();
			unsigned int owner = packet.Reader().GetInt();
			unsigned int id = packet.Reader().GetInt();
			unsigned int amount = packet.Reader().GetInt();
			unsigned short x = packet.Reader().GetShort();
			unsigned short y = packet.Reader().GetShort();

			if (owner == _player->getCharacter().id || owner == 0) {
				
				//printf("Owner: %d PID: %d\n", owner, _player->getCharacter().id);

				auto it = _entities.find(handle);

				ShowPacket(packet.Data(), packet.GetTotalLength());

				if (it != _entities.end())
				{
					//printf("Existe!\n");
					if (!it->second->handle || it->second->handle != handle) {
						it->second->disponse();
						delete it->second;
						return;
					}

					it->second->pos.x = x;
					it->second->pos.y = y;

				}
				else if (id > 0 && handle > 0) {

					//printf("Existe!\n");

					auto drop = new ItemModel();

					drop->id = id;
					drop->handle = handle;
					drop->owner = owner;
					drop->amount = amount;
					drop->pos.x = x;
					drop->pos.y = y;
					drop->type = (type == 19 ? 1 : 2);

					_entities.emplace(handle, drop);
				}
			}

			break;
		}
		case 2:
		case 4:
		case 176://Change map
		{
			std::lock_guard<std::mutex> entityLock(_entitiesMutex);

			for (auto&& it : _entities)
			{
				it.second->disponse();
				delete it.second;
			}

			_entities.clear();

			if (_target && _isWalking) _player->setBusy(false);

			_target = NULL;

			_isWalking = false;
			break;
		}
		}
	}
}


void LootModule::get(unsigned int handle)
{
	auto it = _entities.find(handle);

	if (it != _entities.end()) {
		if (it->second->type == 1) {
			auto packet = pGetItemDrop(it->second->id, it->second->owner, it->second->handle, it->second->amount);
			Sender::Get()->Send(packet.Data(), packet.Size(), SERVER_TYPE::MAP);
		}
		else {
			auto packet = pGetCardDrop(it->second->id, it->second->owner, it->second->handle, it->second->amount);
			Sender::Get()->Send(packet.Data(), packet.Size(), SERVER_TYPE::MAP);
		}
	}
}

void LootModule::update()
{
	if (Sender::Get()->getSocketMap()) {
		if (_player->getUpdateMap() || _player->isInLoading()) return;

		if (!_player->isBusy())
		{
			std::lock_guard<std::mutex> entityLock(_entitiesMutex);

			if (!_entities.empty()) {
				int dist = MAX_DISTANCE;

				_target = NULL;

				for (auto&& it : _entities)
				{
					int currentDist = (int)_player->getDistance(it.second->pos);

					printf("ITEM: %d Pos: %d,%d Dist: %d\n", it.first, it.second->pos.x, it.second->pos.y, currentDist);

					if (currentDist >= MAX_VIEW_DISTANCE) {
						//printf("REMOVING: %d\n", it.first);
						it.second->disponse();
						delete it.second;
					}
					else if (currentDist >= 0 && currentDist < dist)
					{
						dist = currentDist;
						_target = it.second->handle;
					}
				}

				if (_target && dist < MAX_VIEW_DISTANCE) {
					_player->setGetLoot(true);
					_player->setBusy(true);
					_isWalking = true;
					//printf("GO WALK!\n");
				}
			} else _player->setGetLoot(false);
		}
	}

	if (_isWalking)
	{
		if (std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - _lastAction) > _walkInterval)
		{
			std::lock_guard<std::mutex> entityLock(_entitiesMutex);

			auto it = _entities.find(_target);
			if (it == _entities.end()) {
				printf("NULL!\n");
				_target = NULL;
				_player->setBusy(false);
				return;
			}

			double currentDist = _player->getDistance(it->second->pos);

			if (currentDist < MAX_VIEW_DISTANCE - 1)
			{
				if (currentDist <= GET_DISTANCE)
				{
					printf("GET!\n");
					get(it->second->handle);
					_isWalking = false;
					_player->setBusy(false);
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
							_player->moveTo(it->second->pos.x + randX, it->second->pos.y + randY);
							//printf("WALK TO!\n");
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
