#include "MoveModule.h"

void MoveModule::onDisponse()
{
	_movePoints.clear();
	_isWalking = false;
}

void MoveModule::onSend(Packet& packet)
{
}

void MoveModule::onRecv(Packet& packet)
{
	int type = packet.GetType();
	packet.Seek(8);

	if (Sender::Get()->getSocketMap()) {
		switch (type) {
		case 2:
		case 4:
		case 176://Change map
		{
			_isWalking = false;
			_lastDistance = 0;
			_movePoints.clear();
			break;
		}
		}
	}
}


void MoveModule::move(int x, int y)
{
	_player->moveTo(x, y);

	/*if (lastDist > 13) {
		auto packet = pMovePlayer(x, y);
		Sender::Get()->Send(packet, SERVER_TYPE::MAP);
	}*/
}

void MoveModule::update()
{
	if (Sender::Get()->getSocketMap()) {
		if (_player->getUpdateMap() && !_player->isInLoading() && !_isReadingMap) {
			std::lock_guard<std::mutex> entityLock(_mapMutex);

			_isReadingMap = true;
			_player->setUpdateMap(false);

			printf("Trying to update map!\n");

			

			const char * dataHeaderP = (const char*)mapFileAddress+7;

			unsigned char* fileData = ReadFile(getCurrentPath() + "\\Map\\" + dataHeaderP);

			if (fileData) {
				MapModel* map = new MapModel();
				map->ReadData((void*)&fileData);
				delete[] fileData;
				//const void * dataHeaderP = (const void*)mapFileAddress;
				//const void* dataP = (const void*)(baseAddress + mapDataAddress);
				//map->ReadData(&dataHeaderP,&dataHeaderP);

				//printf("LOAD MAP: %d\n", _player->getCharacter().pos.z);
				_player->getPathfinder().addMap(map);
			}
			else {
				printf("FAIL TO LOAD MAP!\n");
			}
			_isReadingMap = false;

			_player->setBusy(false);
		}
		if (_player->getGetLoot()) return;
		if (!_player->isBusy())
		{
			if(_walkInterval != std::chrono::milliseconds(DEFAULT_WALKTIME))
				_walkInterval = std::chrono::milliseconds(DEFAULT_WALKTIME);

			auto walkTo = _player->getWalkTo();

			int x = std::get<0>(walkTo);
			int y = std::get<1>(walkTo);
			int z = std::get<2>(walkTo);

			if (x && y) {
				if (!_player->canMove(x, y)) {
					printf("INVALID MOVE POINT x: %d y: %d\n", x, y);
					_player->setWalkTo(0, 0, 0);
					return;
				}

				_movePoints = _player->getPathfinder().find(_player->getCharacter().pos.x, _player->getCharacter().pos.y, x, y);
				
				if (_movePoints.size()) {
					_player->setBusy(true);
					_isWalking = true;
					_targetMove = std::tuple<int, int>(x, y);
					_targetMap = z;

					_currentMove = _movePoints.front();
					_player->getPathfinder().Log();
				}
				else
				{
					printf("INVALID MOVE POINT\n");
					_player->setWalkTo(0, 0, 0);
				}
			}
		}

		if (_isWalking)
		{
			if (_player->getCancelAction()) {
				_isWalking = false;
				_lastDistance = 0;
				_targetMap = 0;
				_targetMove = std::tuple<int, int>();
				_currentMove = std::tuple<int, int>();
				_movePoints.clear();
				_player->setWalkTo(0, 0, 0);
				_player->setCancelAction(false);
				_player->setBusy(false);
				return;
			}

			//if (std::chrono::duration_cast<std::chrono::milliseconds>(high_resolution_clock::now() - _lastAction) > _walkInterval)
			//{
				int x = std::get<0>(_currentMove);
				int y = std::get<1>(_currentMove);

				_lastDistance = (int)distance(_player->getCharacter().pos.x, _player->getCharacter().pos.y, x, y);

				std::tuple<int, int> last = _movePoints.back();

				//DEIXAR EM 4, MAIS QUE ISSO ELE TELEPORTA
				while (_lastDistance < 4 && _movePoints.size() > 1) {
					_movePoints.erase(_movePoints.begin());
					_currentMove = _movePoints.front();
					x = std::get<0>(_currentMove);
					y = std::get<1>(_currentMove);
					_lastDistance = (int)distance(_player->getCharacter().pos.x, _player->getCharacter().pos.y, x, y);
				}

				if (_movePoints.size() && !isWalking())
				{
					_currentMove = _movePoints.front();

					if (_currentMove != std::tuple<int,int>(0,0)) {

						x = std::get<0>(_currentMove);
						y = std::get<1>(_currentMove);

						move(x, y);

						//_walkInterval = std::chrono::milliseconds(100 + (50 * (int)floor(_lastDistance)));

						_movePoints.erase(_movePoints.begin());

						_lastAction = high_resolution_clock::now();
					}
				}
				else if(!_movePoints.size())
				{
					_isWalking = false;
					_lastDistance = 0;
					_targetMap = 0;
					_targetMove = std::tuple<int, int>();
					_currentMove = std::tuple<int, int>();

					_player->setWalkTo(0, 0, 0);
					_player->setCancelAction(false);
					_player->setBusy(false);
				}
			//}
		}
	}
}

