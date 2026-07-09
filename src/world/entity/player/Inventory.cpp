#include "Inventory.h"
#include "Player.h"
#include "../../level/material/Material.h"
#include "../../level/tile/Tile.h"
#include "../../item/Item.h"
#include "../../item/DyePowderItem.h"

Inventory::Inventory( Player* player, bool creativeMode )
:   super(	36 + Inventory::MAX_SELECTION_SIZE,
			MAX_SELECTION_SIZE,
			ContainerType::INVENTORY,
			creativeMode),
	player(player),
	selected(0)
{
	setupDefault();
	compressLinkedSlotList(0);
}

Inventory::~Inventory() {
}

ItemInstance* Inventory::getSelected() {
	return getLinked(selected);
}

void Inventory::selectSlot( int slot ) {
	if (slot < MAX_SELECTION_SIZE && slot >= 0)
		selected = slot;
}

bool Inventory::moveToSelectedSlot( int inventorySlot, bool propagate ) {
	return linkSlot(selected, inventorySlot, propagate);
}

int Inventory::getSelectionSize() {
	return MAX_SELECTION_SIZE;
}

void Inventory::setupDefault() {
	clearInventory();
	int Sel[MAX_SELECTION_SIZE] = {0};
	int selCount = 0;

	if (_isCreative) {
		// Add all tiles with creativeGroup
		for (int i = 1; i < 256; i++) {
			Tile* tile = Tile::tiles[i];
			if (tile && !tile->creativeGroup.empty()) {
				int slot = addItem(new ItemInstance(tile));
				if (selCount < 9) Sel[selCount++] = slot;
			}
		}

		// Add all items with creativeGroup
		for (int i = 256; i < 512; i++) {
			Item* item = Item::items[i];
			if (item && !item->creativeGroup.empty()) {
				int slot = addItem(new ItemInstance(item));
				if (selCount < 9) Sel[selCount++] = slot;
			}
		}
	} else {
#if defined(WIN32)
		addItem(new ItemInstance(Item::ironIngot, 64));
		addItem(new ItemInstance(Item::ironIngot, 34));
		addItem(new ItemInstance(Tile::stonecutterBench));
		addItem(new ItemInstance(Tile::workBench));
		addItem(new ItemInstance(Tile::furnace));
		addItem(new ItemInstance(Tile::wood, 54));
		addItem(new ItemInstance(Item::stick, 14));
		addItem(new ItemInstance(Item::coal, 31));
		addItem(new ItemInstance(Tile::sand, 6));
		addItem(new ItemInstance(Item::dye_powder, 23, DyePowderItem::PURPLE));
#endif
	}

	for (unsigned int i = 0; i < items.size(); ++i) {
		ItemInstance* item = items[i];

		if (i < MAX_SELECTION_SIZE) {
			if (item)
				LOGE("Error: Should not have items on slot %i\n", i);

			items[i] = NULL;
			continue;
		}

		if (item && _isCreative)
			item->count = 5;
	}

	for (int i = 0; i < MAX_SELECTION_SIZE; ++i) {
		linkedSlots[i] = LinkedSlot(Sel[i]);
	}
}

void Inventory::clearInventoryWithDefault()
{
	clearInventory();
	setupDefault();
}

void Inventory::backupInventory()
{
	for(ItemInstance* item : items) {
		if(item != NULL) {
			backupItems.push_back(new ItemInstance(*item));
		} else {
			backupItems.push_back(NULL);
		}
	}
}

void Inventory::restoreInventory()
{
	clearInventory();
	for(ItemInstance* item : backupItems) {
		if(item != NULL) {
			addItem(new ItemInstance(*item));
		} else {
			addItem(NULL);
		}
	}
	for(ItemInstance* item : backupItems) {
		delete item;
	}
	backupItems.clear();
}

void Inventory::setCreativeMode(bool creative)
{
	if(creative && !_isCreative) {
		backupInventory();
	}
	_isCreative = creative;
	clearInventoryWithDefault();
	if(!creative) {
		restoreInventory();
	}
}

int Inventory::getAttackDamage( Entity* entity )
{
	ItemInstance* item = getSelected();
	if (item != NULL) return item->getAttackDamage(entity);
	return 1;
}

bool Inventory::canDestroy( Tile* tile )
{
	if (tile->material->isAlwaysDestroyable()) return true;

	ItemInstance* item = getSelected();
	if (item != NULL) return item->canDestroySpecial(tile);
	return false;
}

float Inventory::getDestroySpeed( Tile* tile )
{
	ItemInstance* item = getSelected();
	if (item && item->id >= 256) {
		return Item::items[item->id]->getDestroySpeed(NULL, tile);
	}
	return 1.0f;
}

bool Inventory::moveToSelectionSlot( int selectionSlot, int inventorySlot, bool propagate ) {
	return linkSlot(selectionSlot, inventorySlot, propagate);
}

bool Inventory::moveToEmptySelectionSlot( int inventorySlot ) {
	return linkEmptySlot(inventorySlot);
}

void Inventory::doDrop( ItemInstance* item, bool randomly )
{
	player->drop(item, randomly);
}

bool Inventory::stillValid(Player* player) {
	if (this->player->removed) return false;
	if (player->distanceToSqr(this->player) > 8 * 8) return false;
	return true;
}

bool Inventory::add( ItemInstance* item ){
	if (_isCreative || player->hasFakeInventory)
		return true;

	return super::add(item);
}

bool Inventory::removeItem( const ItemInstance* samePtr ) {
	for (int i = MAX_SELECTION_SIZE; i < (int)items.size(); ++i) {
		if (items[i] == samePtr) {
			clearSlot(i);
			return true;
		}
	}
	return false;
}
