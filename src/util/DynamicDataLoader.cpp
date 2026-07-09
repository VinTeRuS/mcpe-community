#include "DynamicDataLoader.h"
#include "JsonLoader.h"
#include "../world/level/tile/Tile.h"
#include "../world/item/Item.h"
#include "../world/entity/EntityDefinition.h"
#include "../world/item/crafting/Recipes.h"
#include "../world/level/biome/Biome.h"
#include "../world/level/dimension/Dimension.h"

void DynamicDataLoader::addHandler(const std::string& type, Handler handler) {
    m_handlers[type] = std::move(handler);
}

void DynamicDataLoader::dispatch(const std::string& modId, const json& data) {
    if (data.is_array()) {
        for (auto& item : data)
            dispatch(modId, item);
        return;
    }
    if (!data.is_object())
        return;

    std::string type = data.value("type", "");
    if (type.empty())
        return;

    auto it = m_handlers.find(type);
    if (it != m_handlers.end()) {
        it->second(modId, data);
    } else {
        fprintf(stderr, "DynamicDataLoader: unknown type '%s' in mod '%s'\n",
                type.c_str(), modId.c_str());
    }
}

void DynamicDataLoader::initialize() {
    addHandler("tile", [](const std::string& modId, const json& data) {
        Tile::handleJsonDefinition(modId, data);
    });
    addHandler("item", [](const std::string& modId, const json& data) {
        Item::handleJsonDefinition(modId, data);
    });
    addHandler("entity", [](const std::string& modId, const json& data) {
        EntityDefinition::handleJsonDefinition(modId, data);
    });
    addHandler("recipe", [](const std::string& modId, const json& data) {
        Recipes::handleJsonDefinition(modId, data);
    });
    addHandler("biome", [](const std::string& modId, const json& data) {
        Biome::handleJsonDefinition(modId, data);
    });
    addHandler("dimension", [](const std::string& modId, const json& data) {
        Dimension::handleJsonDefinition(modId, data);
    });
}

DynamicDataLoader& DynamicDataLoader::singleton() {
    static DynamicDataLoader instance;
    return instance;
}
