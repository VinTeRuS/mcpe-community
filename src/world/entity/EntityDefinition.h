#ifndef NET_MINECRAFT_WORLD_ENTITY_ENTITYDEFINITION_H
#define NET_MINECRAFT_WORLD_ENTITY_ENTITYDEFINITION_H

#include <string>
#include <vector>
#include <unordered_map>

struct EntityDrop {
	std::string item;
	int min = 1;
	int max = 1;
	float chance = 1.0f;
};

struct EntityDefinition {
	std::string nameId;
	int numericId = -1;
	std::string className;
	std::string category;

	int health = 10;
	float runSpeed = 0.7f;
	float width = 0.6f;
	float height = 1.8f;
	int attackDamage = 0;
	int armorValue = 0;

	std::string texture;
	bool fireImmune = false;
	bool fallDamageImmune = false;
	bool climbs = false;

	struct {
		std::string ambient;
		std::string hurt;
		std::string death;
	} sounds;

	struct {
		std::string mainHand;
	} equipment;

	std::vector<EntityDrop> drops;

	static void applyDefinitions();
	static const EntityDefinition* getDefinition(int numericId);

private:
	static std::unordered_map<int, EntityDefinition> s_definitions;
};

#endif
