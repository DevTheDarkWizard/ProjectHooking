#pragma once

#include <vector>

#include "Common.hpp"
#include "DigimonModel.h"
#include "ItemModel.h"
#include "PetModel.h"

struct CharacterModel : public EntityModel {
	std::vector<ItemModel*> buffs;//6
	std::vector<ItemModel*> cloths;//7
	std::vector<ItemModel*> crests;//3
	std::vector<ItemModel*> eggs;//3
	std::vector<ItemModel*> accessories;//3
	std::vector<ItemModel*> etc;//2
	std::vector<ItemModel*> inventory;//24
	std::vector<ItemModel*> cards;//24
	std::vector<ItemModel*> slotCards;//6
	std::vector<DigimonModel*> digimons;
	std::vector<int> archive;
	int wins;
	int losses;
	int pvpPoints;
	short rank;
	char stat;
	char slot;
	short inParty;
	short level;
	int fame;
	int exp;
	int mexp;
	int bits;
	short medals;
	int time;
	short transformation;
	int handShake;
	int randomNumber;
	const char* guildName;
	PetModel pet;
	EntityPosition pos;

	CharacterModel():
		pos(0, 0, 0),
		wins(0), losses(0), rank(0), stat(0),slot(0),
		level(0),fame(0), exp(0), mexp(0), bits(0), medals(0),
		inParty(0), transformation(0), time(0),
		handShake(0), randomNumber(0), guildName(NULL) {
		archive = std::vector<int>(10);
		for (int i = 0; i < 5; i++) digimons.push_back(new DigimonModel());
		for (int i = 0; i < 6; i++) buffs.push_back(new ItemModel());
		for (int i = 0; i < 7; i++) cloths.push_back(new ItemModel());
		for (int i = 0; i < 3; i++) crests.push_back(new ItemModel());
		for (int i = 0; i < 3; i++) eggs.push_back(new ItemModel());
		for (int i = 0; i < 3; i++) accessories.push_back(new ItemModel());
		for (int i = 0; i < 2; i++) etc.push_back(new ItemModel());
		for (int i = 0; i < 24; i++) inventory.push_back(new ItemModel());
		for (int i = 0; i < 24; i++) cards.push_back(new ItemModel());
		for (int i = 0; i < 6; i++) slotCards.push_back(new ItemModel());
	}

	void disponse() {
		disponseBase();
		for (int i = 0; i < (int)std::size(digimons); i++) { digimons[i]->disponse(); delete digimons[i]; }
		for (int i = 0; i < (int)std::size(buffs); i++) { buffs[i]->disponse(); delete buffs[i]; }
		for (int i = 0; i < (int)std::size(cloths); i++) { cloths[i]->disponse(); delete cloths[i]; }
		for (int i = 0; i < (int)std::size(crests); i++) { crests[i]->disponse(); delete crests[i]; }
		for (int i = 0; i < (int)std::size(eggs); i++) { eggs[i]->disponse(); delete eggs[i]; }
		for (int i = 0; i < (int)std::size(accessories); i++) { accessories[i]->disponse(); delete accessories[i]; }
		for (int i = 0; i < (int)std::size(etc); i++) { etc[i]->disponse(); delete etc[i]; }
		for (int i = 0; i < (int)std::size(inventory); i++) { inventory[i]->disponse(); delete inventory[i]; }
		for (int i = 0; i < (int)std::size(cards); i++) { cards[i]->disponse(); delete cards[i]; }
		for (int i = 0; i < (int)std::size(slotCards); i++) { slotCards[i]->disponse(); delete slotCards[i]; }
		archive.clear();
		digimons.clear();
		buffs.clear();
		cloths.clear();
		crests.clear();
		eggs.clear();
		accessories.clear();
		etc.clear();
		inventory.clear();
		cards.clear();
		slotCards.clear();
	}
};

