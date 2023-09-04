#include "PlayerModule.h"

void PlayerModule::setPosition(int x, int y, int z) {
	if (z > 0) _character->pos.z = z;
	if (_character->pos.z) {
		auto dim = _pathfinder.getMapDimensions(_character->pos.z);

		int width = std::get<0>(dim);
		int height = std::get<1>(dim);

		if (x >= 0 && x < width&& y >= 0 && y < height) {
			_character->pos.x = x;
			_character->pos.y = y;
		}
	}
}
void PlayerModule::moveTo(int x, int y) {
	if (canMove(x,y)) {
		callMovePlayer(x, y);
	}
	else
	{
		printf("Invalid position\n");
	}
	/*auto packet = pMovePlayer(x, y);
	Sender::Get()->Send(packet, SERVER_TYPE::MAP);*/
}

void PlayerModule::onDisponse() {
	_pathfinder.disponse();
	_character->disponse();
	delete _character;
}

void PlayerModule::update() {
	/*if (Sender::Get()->getSocketMap()) {
		
	}
	else if (isAtCharacterSelection()) {
		//printf("PLAYER IS IN CHARACTER SELECTION SCREEN\n");
		if (!isBusy() && !isWaitingRecvPacket()) {
			setWaitingRecvPacketID(22);
			//if (!_character->id) {
			//	printf("Character Not Found aborting.\n");
			//	disable();
			//	return;
			//}
			//else {
			//	setBusy(true);
			//
			//	setWaitingRecvPacketID(22);
			//
			//	int charNum = _character->slot;
			//
			//	if (charNum <= 0) {
			//		(*moveCursor)(120, 170);
			//		(*leftClick)(120, 170);
			//	}
			//	else if (charNum == 1) {
			//		(*moveCursor)(680, 170);
			//		(*leftClick)(680, 170);
			//	}
			//	else if (charNum == 2) {
			//		(*moveCursor)(120, 415);
			//		(*leftClick)(120, 415);
			//	}
			//	else if (charNum == 3) {
			//		(*moveCursor)(680, 415);
			//		(*leftClick)(680, 415);
			//	}
			//
			//	(*sleep)(100);
			//	(*moveCursor)(570, 570);
			//	(*leftClick)(570, 570);
			//}
		}
	}
	else if (isAtLogin()) {
		//printf("PLAYER IS IN LOGIN SCREEN\n");
		if (!isBusy() && !isWaitingRecvPacket()) {

			setBusy(true);

			setWaitingRecvPacketID(18);

			(*moveCursor)(395, 223);
			(*leftClick)(395, 223);
			(*sleep)(100);

			const char* username = "p1";
			const char* password = "p1";

			(*sendText)(username, strlen(username));

			(*moveCursor)(395, 223 + 46);
			(*leftClick)(395, 223 + 46);
			(*sleep)(100);

			(*sendText)(password, strlen(password));

			(*moveCursor)(418, 322);
			(*leftClick)(418, 322);
		}
	}
	else if (!isAtLogin() && !isAtCharacterSelection() && !isAtMap() && isAtInitialScreen()) {
		//printf("PLAYER IS IN INITIAL SCREEN\n");
		if (!isBusy() && !isWaitingRecvPacket()) {

			setBusy(true);

			setWaitingRecvPacketID(3);

			(*moveCursor)(610, 280);
			(*leftClick)(610, 280);
			(*sleep)(100);

			//430, 209 sv 1
			int server = 0;
			(*moveCursor)(430, 138 + server * 37);
			(*leftClick)(430, 138 + server * 37);
		}
	}*/
}
void PlayerModule::onSend(Packet& packet)
{
	//printf("SEND TIPO: %.2X TAMANHO: %d TOTAL: %d\n", packet.Reader().GetType(), packet.Reader().GetPacketLength(), (int)packet.Reader().GetTotalLength());
	int type = packet.GetType();
	packet.Seek(8);

	if (Sender::Get()->getSocketMap()) {
		switch (type) {
		case 221:
		{

			/*packet.Skip(4);

			_pathfinder.removeMap(_character->pos.z);

			_character->pos.z = packet.Reader().GetInt();
			_character->pos.x = packet.Reader().GetShort();
			_character->pos.y = packet.Reader().GetShort();

			_pathfinder.setMap(_character->pos.z);

			printf("MAP: %d\n", _character->pos.z);

			setUpdateMap(true);*/

			break;
		}
		case 4:
		{
			//setUpdateMap(true);

			//const char* charName = packet.Reader().GetString(20);
		}
		case 32:
		{
			short rnumber = packet.Reader().GetShort();

			short x = packet.Reader().GetShort();
			short y = packet.Reader().GetShort();
			
			setPosition(x, y);

			//printf("MOVE: %d,%d RN: %d\n", _character->pos.x, _character->pos.y, rnumber);

			break;
		}
		case 1:
		{
			const char* login = packet.Reader().GetString(20, 33);
			const char* charName = packet.Reader().GetString(20, 12);

			printf("Send Join map %s(%s)\n", login, charName);
			break;
		}
		}
	}
	else {
		switch (type) {
		case 22:
		{
			const char* login = packet.Reader().GetString(20, 8);
			const char* charName = packet.Reader().GetString(20, 29);

			printf("Trying join map %s(%s)\n", login, charName);
			break;
		}
		case 20://Check Warehouse first login
		{
			break;
		}
		case 3:
		{
			const char* version = packet.Reader().GetString(10,16);
			printf("Sending Game Version: %s\n", version);
			break;
		}
		}
	}
	if (isWaitingSendPacket() == type) {
		setWaitingSendPacketID(0);
		setBusy(false);
	}
}

void PlayerModule::onRecv(Packet& packet)
{
	int type = packet.GetType();
	packet.Seek(8);

	if (true || Sender::Get()->getSocketMap()) {
		switch (type) {
		case 197://Refresh Bonus exp
		{
			break;
		}
		case 180://Refresh Digimon Slot
		{
			break;
		}
		case 178://Refresh Digimon Stats
		{
			break;
		}
		case 176://Change map
		{
			_pathfinder.removeMap(_character->pos.z);

			int z = packet.Reader().GetInt();
			int x = packet.Reader().GetShort();
			int y = packet.Reader().GetShort();

			_character->pos.x = x;
			_character->pos.y = y;
			_character->pos.z = z;

			_pathfinder.setMap(z);

			//setPosition(x,y,z);

			printf("MAP2: %d X: %d Y: %d\n", _character->pos.z, _character->pos.x, _character->pos.y);

			break;
		}
		case 115://Refresh Buffs
		{
			break;
		}
		case 100://Join Chat
		{
			break;
		}
		case 20://Move Player
		{
			//ShowPacket(packet.Reader().GetBytes(), packet.Reader().GetTotalLength());
			/*packet.Skip(4);

			const char* playerRID = packet.Reader().GetString(16);
			const char* playerName = packet.Reader().GetString(20);

			packet.Skip(1);

			if (!memcmp(playerRID,_character->rid, 16))
			{
				_character->model = packet.Reader().GetByte();
				_character->rank = packet.Reader().GetShort();
				_character->level = packet.Reader().GetShort();
				_character->stat = packet.Reader().GetByte();

				_character->pos.dir = packet.Reader().GetByte();
				_character->pos.x = packet.Reader().GetShort();
				_character->pos.y = packet.Reader().GetShort();

				packet.Skip(4);

				_character->partner.rid = packet.Reader().GetString(16);
				_character->partner.model = packet.Reader().GetShort();
				_character->partner.name = packet.Reader().GetString(20);

				packet.Skip(12);

				_character->partner.level = packet.Reader().GetShort();

				//printf("Level: %d, Pos: %d,%d Partner: %s, Level: %d\n", _character->level, _character->pos.x, _character->pos.y, _character->partner.name, _character->partner.level);
			}
			else {
				CharacterModel player;
				
				player.name = playerName;
				player.rid = playerRID;

				player.model = packet.Reader().GetByte();
				player.rank = packet.Reader().GetShort();
				player.level = packet.Reader().GetShort();
				player.stat = packet.Reader().GetByte();

				player.pos.dir = packet.Reader().GetByte();
				player.pos.x = packet.Reader().GetShort();
				player.pos.y = packet.Reader().GetShort();

				packet.Skip(4);

				player.partner.rid = packet.Reader().GetString(16);
				player.partner.model = packet.Reader().GetShort();
				player.partner.name = packet.Reader().GetString(20);

				packet.Skip(12);

				player.partner.level = packet.Reader().GetShort();
				player.inParty = packet.Reader().GetShort();
				packet.Skip(2);

				player.etc[1].id = packet.Reader().GetInt();
				player.pet.id = packet.Reader().GetInt();

				for (int i = 0; i < (int)std::size(player.cloths); i++) {
					player.cloths[i].id = packet.Reader().GetShort();
				}

				player.stat = packet.Reader().GetByte();
				packet.Skip(1);

				player.guildName = packet.Reader().GetString(20);

				packet.Skip(1);
				player.transformation = packet.Reader().GetShort();

				//printf("Tamer: %s (LV: %d), Pos: %d,%d Partner: %s (LV: %d)\n", player.name, player.level, player.pos.x, player.pos.y, player.partner.name, player.partner.level);
			}
			
			//printf("MOVE: %d,%d\n", _character->pos.x, _character->pos.y);*/
			break;
		}
		case 17://Refresh Mobs in Map
		{
			/*DigimonModel mob;
			
			mob.id = packet.Reader().GetInt();
			mob.rid = packet.Reader().GetString(16);
			mob.name = packet.Reader().GetString(20);

			packet.Skip(8);

			mob.model = packet.Reader().GetShort();
			mob.level = packet.Reader().GetShort();
			mob.motion = packet.Reader().GetByte();
			   
			mob.pos.dir = packet.Reader().GetByte();
			mob.pos.x = packet.Reader().GetShort();
			mob.pos.y = packet.Reader().GetShort();*/

			//printf("MOB: %s (LV: %d), Pos: %d,%d, Dist: %f\n", mob.name, mob.level, mob.pos.x, mob.pos.y, distance);
			
			break;
		}
		case 16://Refresh Dummy on Map
		{
			break;
		}
		case 5://Refresh Mission
		{
			break;
		}
		case 4:
		{
			setBusy(true);
			setUpdateMap(true);
			break;
		}
		case 3://Player Inventory
		{
			packet.Skip(8);

			for (int i = 0; i < (int)std::size(_character->cards); i++) {
				int active = packet.Reader().GetInt();
				if (active) {
					_character->cards[i]->handle = packet.Reader().GetInt();
					_character->cards[i]->id = packet.Reader().GetInt();
					_character->cards[i]->amount = packet.Reader().GetInt();
				}
				else packet.Skip(12);
			}

			for (int i = 0; i < (int)std::size(_character->inventory); i++) {
				int active = packet.Reader().GetInt();
				if (active) {
					_character->inventory[i]->handle = packet.Reader().GetInt();
					_character->inventory[i]->id = packet.Reader().GetInt();
					_character->inventory[i]->amount = packet.Reader().GetInt();
				}
				else packet.Skip(12);
			}

			break;
		}
		case 2://Player Spawn
		{
			//setUpdateMap(true);

			//ShowPacket(packet.Reader().GetBytes(), packet.Reader().GetTotalLength());

			_character->id = packet.Reader().GetInt();
			_character->rid = (char*)packet.Reader().GetString(16);
			_character->name = (char*)packet.Reader().GetString(20);

			packet.Skip(11);

			_character->model = packet.Reader().GetByte();
			_character->rank = packet.Reader().GetShort();
			_character->level = packet.Reader().GetShort();

			_character->fame = packet.Reader().GetInt();
			_character->exp = packet.Reader().GetInt();
			_character->mexp = packet.Reader().GetInt();
			_character->bits = packet.Reader().GetInt();
			_character->wins = packet.Reader().GetInt();
			_character->losses = packet.Reader().GetInt();
			_character->losses = (_character->losses - _character->wins);

			packet.Skip(6);

			_character->pos.z = packet.Reader().GetShort();

			for (int i = 0; i < (int)std::size(_character->crests); i++) {
				_character->crests[i]->id = packet.Reader().GetInt();
				_character->crests[i]->amount = packet.Reader().GetInt();
				packet.Skip(4);
				if (i < 2) packet.Skip(4);
			}

			for (int i = 0; i < (int)std::size(_character->eggs); i++) {
				packet.Skip(4);
				_character->eggs[i]->id = packet.Reader().GetInt();
				_character->eggs[i]->amount = packet.Reader().GetInt();
				if (i < 2) packet.Skip(4);
			}

			for (int i = 0; i < (int)std::size(_character->slotCards); i++) {
				packet.Skip(8);
				_character->slotCards[i]->id = packet.Reader().GetInt();
				_character->slotCards[i]->amount = packet.Reader().GetInt();
			}

			for (int i = 0; i < (int)std::size(_character->digimons); i++) {
				ReadDigimon(packet, *_character->digimons[i]);
			}

			_character->pos.x = packet.Reader().GetShort();
			_character->pos.y = packet.Reader().GetShort();

			packet.Skip(4);

			for (int i = 0; i < (int)std::size(_character->etc); i++) {
				packet.Skip(8);
				_character->etc[i]->id = packet.Reader().GetInt();
				_character->etc[i]->amount = packet.Reader().GetInt();
			}

			_character->pet.id = packet.Reader().GetInt();
			_character->pet.hp = packet.Reader().GetInt();

			packet.Skip(4);

			for (int i = 0; i < (int)std::size(_character->cloths); i++) {
				packet.Skip(4);
				_character->cloths[i]->id = packet.Reader().GetInt();
				_character->cloths[i]->amount = packet.Reader().GetInt();
				packet.Skip(4);
			}

			for (int i = 0; i < (int)std::size(_character->accessories); i++) {
				packet.Skip(4);
				_character->accessories[i]->id = packet.Reader().GetInt();
				_character->accessories[i]->amount = packet.Reader().GetInt();
				if (i < 2) packet.Skip(4);
			}

			_character->medals = packet.Reader().GetShort();
			_character->handShake = packet.Reader().GetShort();
			_character->time = packet.Reader().GetInt();

			_character->stat = packet.Reader().GetByte();

			packet.Skip(2);

			_character->pos.z = packet.Reader().GetByte();

			printf("MAP: %d, X: %d Y: %d\n", _character->pos.z, _character->pos.x, _character->pos.y);

			_pathfinder.setMap(_character->pos.z);

			_character->pvpPoints = packet.Reader().GetInt();

			for (int i = 0; i < (int)std::size(_character->buffs); i++) {
				_character->buffs[i]->id = packet.Reader().GetByte();
				_character->buffs[i]->type = packet.Reader().GetByte();
				packet.Skip(2);
				_character->buffs[i]->amount = packet.Reader().GetInt();
			}

			//printf("Pos: %d,%d MAPA ID: %d\n", _character->pos.x, _character->pos.y,_character->pos.z);
			break;
		}
		}
	}
	else if(!_character->id) {
		switch (type) {
		case 20://Check Warehouse first login
		{
			break;
		}
		case 18://Receive Characters
		{
			packet.Skip(4);

			CharacterModel chars[4];

			printf("Characters:\n\n");

			for (int i = 0; i < 4; i++) {
				chars[i].id = packet.Reader().GetInt();
				chars[i].model = packet.Reader().GetByte();
				chars[i].name = (char*)packet.Reader().GetString(20);

				packet.Skip(1);

				chars[i].level = packet.Reader().GetShort();
				chars[i].pos.x = packet.Reader().GetShort();
				chars[i].pos.y = packet.Reader().GetShort();

				for (int x = 0; x < (int)std::size(chars[i].cloths); x++) {
					chars[i].cloths[x]->id = packet.Reader().GetShort();
				}

				chars[i].digimons[0]->model = packet.Reader().GetShort();
				chars[i].digimons[0]->level = packet.Reader().GetShort();
				chars[i].digimons[0]->name = (char*)packet.Reader().GetString(20);

				packet.Skip(2);

				chars[i].losses = packet.Reader().GetInt();
				chars[i].wins = packet.Reader().GetInt();
				chars[i].losses = chars[i].losses - chars[i].wins;
				chars[i].slot = packet.Reader().GetByte();
				chars[i].pos.z = packet.Reader().GetByte();//Map

				packet.Skip(2);

				chars[i].randomNumber = packet.Reader().GetInt();
				chars[i].handShake = packet.Reader().GetInt();
				chars[i].stat = packet.Reader().GetInt();

				if (chars[i].id) {
					//if(!_character->id) _character = chars[i];
					printf("Tamer: %s (LV: %d), Partner: %s (LV: %d)\n",
						chars[i].name, chars[i].level,
						chars[i].digimons[0]->name, chars[i].digimons[0]->level);
					//printf("slot:%d\n", chars[i].slot);
					//printf("id:%d\n", chars[i].id);
					//printf("model:%d\n", chars[i].model);
					//printf("name:%s\n", chars[i].name);
					//printf("level:%d\n", chars[i].level);
					//printf("x,y,z:%d,%d,%d\n", chars[i].pos.x, chars[i].pos.y, chars[i].pos.z);
					//printf("partner model:%d\n", chars[i].model);
					//printf("partner level:%d\n", chars[i].level);
					//printf("partner name:%s\n", chars[i].name);
					//printf("losses:%d\n", chars[i].losses);
					//printf("wins:%d\n", chars[i].wins);
					//printf("stat:%d\n\n", chars[i].stat);
				}
			}

			printf("\n");
			break;
		}
		case 4:
		{
			int resultStatus = packet.Reader().GetInt(8);

			if (resultStatus == 2) {
				//OK
				printf("Login OK\n");
			}
			else {
				//Senha incorreta, Usuario banido
				printf("Something wrong with login!\n");
				//disable();
			}
			break;
		}
		case 3: 
		{
			const char* version = packet.Reader().GetString(10, 16);
			printf("Server Version: %s\n", version);
			break;
		}
		}
	}

	if (isWaitingRecvPacket() == type) {
		setWaitingRecvPacketID(0);
		setBusy(false);
	}
	//printf("RECV TIPO: %.2X TAMANHO: %d TOTAL: %d\n", packet.Reader().GetType(), packet.Reader().GetPacketLength(), (int)packet.Reader().GetTotalLength());
}

bool PlayerModule::isInLoading() {
	DWORD dynamicAddr = (DWORD)(baseAddress + inLoadingAddress);
	return (*(BYTE*)dynamicAddr == 1);
}

bool PlayerModule::isWalking() {
	DWORD dynamicAddr = (DWORD)0xAD73C4;
	return (*(BYTE*)dynamicAddr == 1);
}

bool PlayerModule::isAtInitialScreen() {
	DWORD dynamicAddr = *(DWORD*)(baseAddress + 0x00065814);
	DWORD pointer1 = (DWORD)(dynamicAddr + 0x18);
	return (*(BYTE*)pointer1 == 0x01);
}

bool PlayerModule::isAtMap() {
	DWORD dynamicAddr = (DWORD)(baseAddress + 0x24CF6D);
	return (*(BYTE*)dynamicAddr == 0x01);
}

bool PlayerModule::isAtCharacterSelection()
{
	DWORD dynamicAddr = *(DWORD*)(baseAddress + 0x0003BD20);
	DWORD pointer1 = (DWORD)(dynamicAddr + 0x2C2);
	return (*(BYTE*)pointer1 == 0x01);
}

bool PlayerModule::isAtLogin()
{
	DWORD dynamicAddr = *(DWORD*)(baseAddress + 0x00F3DB80);
	DWORD pointer1 = *(DWORD*)(dynamicAddr + 0x7A8);
	DWORD pointer2 = (DWORD)(pointer1 + 0x24C);
	return (*(BYTE*)pointer2 != 0x00);
}
