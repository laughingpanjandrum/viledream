#pragma once

#include "gamedata.h"
#include "combat.h"
#include "magic.h"
#include "messages.h"



//	Using items

void applyConsumableEffect(gamedataPtr gdata, itemData::itid id);
void playerTryUseConsumable(gamedataPtr gdata, const int idx);
void openConsumableSelectMenu(gamedataPtr gdata);

void selectItemToThrow(gamedataPtr gdata, itemPtr it);
void openThrownSelectMenu(gamedataPtr gdata);

void tryActivateFromInventory(gamedataPtr gdata, const int idx, ItemCategory cat);


//	Inventory management

void loseItem(gamedataPtr gdata, itemPtr it, vector<itemPtr>* ilist);
void expendItem(gamedataPtr gdata, itemPtr it, vector<itemPtr>* ilist);

void cycleInventoryCategory(gamedataPtr gdata, const int vec);
void openInventory(gamedataPtr gdata);

const vector<itemPtr>* getItemListForCategory(gamedataPtr gdata, ItemCategory cat);


//	Equipment management

void playerEquipItem(gamedataPtr gdata, itemPtr it);
void playerEquipAsAlt(gamedataPtr gdata, itemPtr it);

void playerSwapWeapons(gamedataPtr gdata);

void selectItemToUnequip(gamedataPtr gdata, const int idx);
void selectItemToEquip(gamedataPtr gdata, const int idx);
void selectEquipmentSlot(gamedataPtr gdata, const int idx);
void openEquipmentMenu(gamedataPtr gdata, bool allow_modify = false);

void initializeEquipmentStorage(gamedataPtr gdata);
vector<itemPtr>* getEquipmentList(gamedataPtr gdata, const ItemCategory cat);


//	Picking stuff up

bool tryStackItems(gamedataPtr gdata, itemPtr base, itemPtr it);
void addToItemList(gamedataPtr gdata, itemPtr it, vector<itemPtr>* ilist);
void addToInventory(gamedataPtr gdata, itemPtr it);

void pickupItem(gamedataPtr gdata, itemPtr it, char and_do_with);

void tryAutopickup(gamedataPtr gdata, const intpair pt);
void tryPickupItem(gamedataPtr gdata);