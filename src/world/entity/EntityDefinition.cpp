#include "EntityDefinition.h"
#include "../../util/JsonLoader.h"
#include <cstdio>

std::unordered_map<int, EntityDefinition> EntityDefinition::s_definitions;

void EntityDefinition::applyDefinitions() {
	auto& loader = JsonLoader::singleton();
	int loaded = 0;
	loader.loadDir("minecraft", "entities", [&loaded](const std::string& path, const json& j) {
		auto items = j.is_array() ? j : nlohmann::json::array({j});
		for (auto& item : items) {
			EntityDefinition def;

			def.nameId = item.value("id", "");
			def.numericId = item.value("numeric_id", -1);
			def.className = item.value("class", "");
			def.category = item.value("category", "");

			auto props = item.value("properties", nlohmann::json::object());
			def.health = props.value("health", 10);
			def.runSpeed = props.value("run_speed", 0.7f);
			def.width = props.value("width", 0.6f);
			def.height = props.value("height", 1.8f);
			def.attackDamage = props.value("attack_damage", 0);
			def.armorValue = props.value("armor_value", 0);

			def.texture = item.value("texture", "");
			def.fireImmune = item.value("fire_immune", false);
			def.fallDamageImmune = item.value("fall_damage_immune", false);
			def.climbs = item.value("climbs", false);

			auto sounds = item.value("sounds", nlohmann::json::object());
			def.sounds.ambient = sounds.value("ambient", "");
			def.sounds.hurt = sounds.value("hurt", "");
			def.sounds.death = sounds.value("death", "");

			auto equip = item.value("equipment", nlohmann::json::object());
			def.equipment.mainHand = equip.value("main_hand", "");

			auto drops = item.value("drops", nlohmann::json::array());
			for (auto& d : drops) {
				EntityDrop drop;
				drop.item = d.value("item", "");
				drop.min = d.value("min", 1);
				drop.max = d.value("max", 1);
				drop.chance = d.value("chance", 1.0f);
				def.drops.push_back(drop);
			}

			if (def.numericId >= 0) {
				s_definitions[def.numericId] = def;
				loaded++;
			}
		}
	});
	printf("EntityDefinition::applyDefinitions: loaded %d entity definitions\n", loaded);
}

/*static*/
void EntityDefinition::handleJsonDefinition(const std::string& modId, const json& data) {
    (void)modId;
    EntityDefinition def;

    def.nameId = data.value("id", "");
    def.numericId = data.value("numeric_id", -1);
    def.className = data.value("class", "");
    def.category = data.value("category", "");

    auto props = data.value("properties", nlohmann::json::object());
    def.health = props.value("health", 10);
    def.runSpeed = props.value("run_speed", 0.7f);
    def.width = props.value("width", 0.6f);
    def.height = props.value("height", 1.8f);
    def.attackDamage = props.value("attack_damage", 0);
    def.armorValue = props.value("armor_value", 0);

    def.texture = data.value("texture", "");
    def.fireImmune = data.value("fire_immune", false);
    def.fallDamageImmune = data.value("fall_damage_immune", false);
    def.climbs = data.value("climbs", false);

    auto sounds = data.value("sounds", nlohmann::json::object());
    def.sounds.ambient = sounds.value("ambient", "");
    def.sounds.hurt = sounds.value("hurt", "");
    def.sounds.death = sounds.value("death", "");

    auto equip = data.value("equipment", nlohmann::json::object());
    def.equipment.mainHand = equip.value("main_hand", "");

    auto drops = data.value("drops", nlohmann::json::array());
    for (auto& d : drops) {
        EntityDrop drop;
        drop.item = d.value("item", "");
        drop.min = d.value("min", 1);
        drop.max = d.value("max", 1);
        drop.chance = d.value("chance", 1.0f);
        def.drops.push_back(drop);
    }

    if (def.numericId >= 0) {
        s_definitions[def.numericId] = def;
    }
}

const EntityDefinition* EntityDefinition::getDefinition(int numericId) {
	auto it = s_definitions.find(numericId);
	if (it != s_definitions.end())
		return &it->second;
	return nullptr;
}
