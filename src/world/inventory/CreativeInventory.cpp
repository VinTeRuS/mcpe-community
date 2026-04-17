#include "CreativeInventory.h"
#include "../item/Item.h"
#include "../level/tile/Tile.h"

CreativeInventory::CreativeInventory()
: super(0, 0, ContainerType::INVENTORY, false) {
}

CreativeInventory::~CreativeInventory() {
}

void CreativeInventory::populateCreativeItems() {
	items.clear();

	for(int i = 0; i < 512; i++) {
		Item* item = Item::items[i];
		if(item != NULL) {
			ItemInstance* instance = new ItemInstance(item, 1, 0);
			addItem(instance);
		}
	}

	for(int i = 1; i < 256; i++) {
		Tile* tile = Tile::tiles[i];
		if(tile != NULL) {
			ItemInstance* instance = new ItemInstance(tile, 1, 0);
			addItem(instance);
		}
	}
}
