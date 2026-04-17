#ifndef NET_MINECRAFT_WORLD_INVENTORY__CreativeInventory_H__
#define NET_MINECRAFT_WORLD_INVENTORY__CreativeInventory_H__

#include "FillingContainer.h"
#include "../item/ItemInstance.h"

class CreativeInventory : public FillingContainer {
public:
	typedef FillingContainer super;

	CreativeInventory();
	~CreativeInventory();

	void populateCreativeItems();
};

#endif
