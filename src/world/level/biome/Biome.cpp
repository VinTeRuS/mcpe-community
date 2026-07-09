#include "BiomeInclude.h"

#include "../levelgen/feature/TreeFeature.h"
#include "../levelgen/feature/TallgrassFeature.h"

#include "../../entity/EntityTypes.h"
#include "../../entity/MobCategory.h"
#include "../../entity/EntityDefinition.h"
#include "../../level/tile/TallGrass.h"
#include "../../level/tile/Tile.h"
#include "../../../util/JsonLoader.h"

Biome* Biome::rainForest	 = NULL;
Biome* Biome::swampland		 = NULL;
Biome* Biome::seasonalForest = NULL;
Biome* Biome::forest		 = NULL;
Biome* Biome::savanna		 = NULL;
Biome* Biome::shrubland		 = NULL;
Biome* Biome::taiga			 = NULL;
Biome* Biome::desert		 = NULL;
Biome* Biome::plains		 = NULL;
Biome* Biome::iceDesert		 = NULL;
Biome* Biome::tundra		 = NULL;

/*static*/
Biome::MobList Biome::_emptyMobList;
int Biome::defaultTotalEnemyWeight = 0;
int Biome::defaultTotalFriendlyWeight = 0;

/*static*/
Biome* Biome::map[64*64];

Biome::Biome()
:	topMaterial(((Tile*)Tile::grass)->id),
	material(((Tile*)Tile::dirt)->id),
	leafColor(0x4EE031)
{
    _friendlies.insert(_friendlies.end(), MobSpawnerData(MobTypes::Sheep, 12, 2, 3));
    _friendlies.insert(_friendlies.end(), MobSpawnerData(MobTypes::Pig, 10, 1, 3));
    _friendlies.insert(_friendlies.end(), MobSpawnerData(MobTypes::Chicken, 10, 2, 4));
    _friendlies.insert(_friendlies.end(), MobSpawnerData(MobTypes::Cow, 8, 2, 3));

    _enemies.insert(_enemies.end(), MobSpawnerData(MobTypes::Spider, 8, 2, 3));
    _enemies.insert(_enemies.end(), MobSpawnerData(MobTypes::Zombie, 12, 2, 4));
    _enemies.insert(_enemies.end(), MobSpawnerData(MobTypes::Skeleton, 6, 1, 3));
    _enemies.insert(_enemies.end(), MobSpawnerData(MobTypes::Creeper, 4, 1, 1));
    //_enemies.insert(_enemies.end(), MobSpawnerData(Slime.class, 10, 4, 4));
    //_enemies.insert(_enemies.end(), MobSpawnerData(EnderMan.class, 1, 1, 4));

    // wolves are added to forests and taigas
    // _friendlies.insert(_friendlies.end(), new MobSpawnerData(Wolf.class, 2));

    //_waterFriendlies.insert(_waterFriendlies.end(), (new MobSpawnerData(Squid.class, 10, 4, 4));

	//
	// Sum up the weights
	//
	defaultTotalEnemyWeight = 0;
	for (MobList::const_iterator cit = _enemies.begin(); cit != _enemies.end(); ++cit)
		defaultTotalEnemyWeight += cit->randomWeight;

	defaultTotalFriendlyWeight = 0;
	for (MobList::const_iterator cit = _friendlies.begin(); cit != _friendlies.end(); ++cit)
		defaultTotalFriendlyWeight += cit->randomWeight;
}

Biome* Biome::setName( const std::string& name )
{
	this->name = name;
	return this;
}

Biome* Biome::setLeafColor( int leafColor )
{
	this->leafColor = leafColor;
	return this;
}

Biome* Biome::setColor( int color )
{
	this->color = color;
	return this;
}

Biome* Biome::setSnowCovered()
{
	this->snowCovered = true;
	return this;
}

Biome* Biome::clearMobs( bool friendlies /*= true*/, bool waterFriendlies /*= true*/, bool enemies /*= true*/ )
{
	if (friendlies) _friendlies.clear();
	if (waterFriendlies) _waterFriendlies.clear();
	if (enemies) _enemies.clear();
	return this;
}


/*static*/
void Biome::recalc()
{
	for (int a = 0; a < 64; a++) {
		for (int b = 0; b < 64; b++) {
			map[a + b * 64] = _getBiome(a / 63.0f, b / 63.0f);
		}
	}

	Biome::desert->topMaterial = Biome::desert->material = (char) Tile::sand->id;
	Biome::iceDesert->topMaterial = Biome::iceDesert->material = (char) Tile::sand->id;
}

/*static*/
void Biome::initBiomes() {
	rainForest		= (new RainforestBiome())->setColor(0x08FA36)->setName("Rainforest")->setLeafColor(0x1FF458);
	swampland		= (new SwampBiome())->setColor(0x07F9B2)->setName("Swampland")->setLeafColor(0x8BAF48);
	seasonalForest	= (new Biome())->setColor(0x9BE023)->setName("Seasonal Forest");
	forest			= (new ForestBiome())->setColor(0x056621)->setName("Forest")->setLeafColor(0x4EBA31);
	savanna			= (new FlatBiome())->setColor(0xD9E023)->setName("Savanna");
	shrubland		= (new Biome())->setColor(0xA1AD20)->setName("Shrubland");
	taiga			= (new TaigaBiome())->setColor(0x2EB153)->setName("Taiga")->setSnowCovered()->setLeafColor(0x7BB731);
	desert			= (new FlatBiome())->setColor(0xFA9418)->clearMobs(true, true, false)->setName("Desert");
	plains			= (new FlatBiome())->setColor(0xFFD910)->setName("Plains");
	iceDesert		= (new FlatBiome())->setColor(0xFFED93)->clearMobs(true, false, false)->setName("Ice Desert")->setSnowCovered()->setLeafColor(0xC4D339);
	tundra			= (new Biome())->setColor(0x57EBF9)->setName("Tundra")->setSnowCovered()->setLeafColor(0xC4D339);
	
	rainForest->nameId = "minecraft:rainforest";
	swampland->nameId = "minecraft:swampland";
	seasonalForest->nameId = "minecraft:seasonal_forest";
	forest->nameId = "minecraft:forest";
	savanna->nameId = "minecraft:savanna";
	shrubland->nameId = "minecraft:shrubland";
	taiga->nameId = "minecraft:taiga";
	desert->nameId = "minecraft:desert";
	plains->nameId = "minecraft:plains";
	iceDesert->nameId = "minecraft:ice_desert";
	tundra->nameId = "minecraft:tundra";

	recalc();
	applyDefinitions();
}
/*static*/
void Biome::teardownBiomes() {
	delete rainForest;		rainForest= NULL;
	delete swampland;		swampland = NULL;
	delete seasonalForest;	seasonalForest = NULL;
	delete forest;			forest    = NULL;
	delete savanna;			savanna   = NULL;
	delete shrubland;		shrubland = NULL;
	delete taiga;			taiga     = NULL;
	delete desert;			desert    = NULL;
	delete plains;			plains    = NULL;
	delete iceDesert;		iceDesert = NULL;
	delete tundra;			tundra	  = NULL;
}

Feature* Biome::getTreeFeature( Random* random )
{
	if (random->nextInt(10) == 0) {
		//return /*new*/ BasicTree();
	}
	return new TreeFeature(false);
}
Feature* Biome::getGrassFeature( Random* random ) {
	return new TallgrassFeature(Tile::tallgrass->id, TallGrass::TALL_GRASS);
}

Biome* Biome::getBiome( float temperature, float downfall )
{
	int a = (int) (temperature * 63);
	int b = (int) (downfall * 63);

	//printf("Getting biome: %s\n", map[a + b * 64]->name.c_str());

	return map[a + b * 64];
}

Biome* Biome::_getBiome( float temperature, float downfall )
{
	downfall *= (temperature);
	if (temperature < 0.10f) {
		return Biome::tundra;
	} else if (downfall < 0.20f) {
		if (temperature < 0.50f) {
			return Biome::tundra;
		} else if (temperature < 0.95f) {
			return Biome::savanna;
		} else {
			return Biome::desert;
		}
	} else if (downfall > 0.5f && temperature < 0.7f) {
		return Biome::swampland;
	} else if (temperature < 0.50f) {
		return Biome::taiga;
	} else if (temperature < 0.97f) {
		if (downfall < 0.35f) {
			return Biome::shrubland;
		} else {
			return Biome::forest;
		}
	} else {
		if (downfall < 0.45f) {
			return Biome::plains;
		} else if (downfall < 0.90f) {
			return Biome::seasonalForest;
		} else {
			return Biome::rainForest;
		}
	}
}

float Biome::adjustScale( float scale )
{
	return scale;
}

float Biome::adjustDepth( float depth )
{
	return depth;
}

int Biome::getSkyColor( float temp )
{
//	temp /= 3.f;
//	if (temp < -1) temp = -1;
//	if (temp > 1) temp = 1;
	return 0x80808080;
	//return Color.getHSBColor(224 / 360.0f - temp * 0.05f, 0.50f + temp * 0.1f, 1.0f).getRGB();
}

Biome::MobList& Biome::getMobs(const MobCategory& category)
{
    if (&category == &MobCategory::monster)
		return _enemies;
    if (&category == &MobCategory::creature)
		return _friendlies;
    if (&category == &MobCategory::waterCreature)
		return _waterFriendlies;

	LOGE("Unknown MobCategory!");
	return _emptyMobList;
}

float Biome::getCreatureProbability() {
    return 0.08f;
}

/*static*/
void Biome::applyDefinitions() {
	auto& loader = JsonLoader::singleton();

	// Build tile name->ID lookup (prefer lower ID on name collision)
	std::unordered_map<std::string, int> tileNames;
	for (int i = 0; i < 256; i++) {
		if (Tile::tiles[i]) {
			const std::string& nid = Tile::tiles[i]->getNameId();
			if (!nid.empty()) {
				auto it = tileNames.find(nid);
				if (it == tileNames.end() || i < it->second)
					tileNames[nid] = i;
			}
		}
	}

	// Build entity name->ID lookup
	std::unordered_map<std::string, int> entityNames;
	for (int i = 0; i < 256; i++) {
		auto* def = EntityDefinition::getDefinition(i);
		if (def) entityNames[def->nameId] = i;
	}

	// All biome pointers for matching
	Biome* allBiomes[] = {
		rainForest, swampland, seasonalForest, forest, savanna,
		shrubland, taiga, desert, plains, iceDesert, tundra
	};

	int count = 0;
	loader.loadDir("minecraft", "biomes", [&](const std::string& path, const json& data) {
		auto list = data["biomes"];
		if (!list.is_array()) return;
		for (auto& j : list) {
			std::string id = j.value("id", "");
			if (id.empty()) continue;

			Biome* biome = nullptr;
			for (auto* b : allBiomes) {
				if (b && b->nameId == id) { biome = b; break; }
			}
			if (!biome) {
				printf("Biome::applyDefinitions: unknown biome '%s'\n", id.c_str());
				continue;
			}
			count++;

			auto& props = j["properties"];
			if (props.contains("color") && props["color"].is_number())
				biome->color = props["color"];
			if (props.contains("leaf_color") && props["leaf_color"].is_number())
				biome->leafColor = props["leaf_color"];
			if (props.contains("snow_covered") && props["snow_covered"].is_boolean())
				biome->snowCovered = props["snow_covered"];

			std::string topMat = props.value("top_material", "");
			if (!topMat.empty()) {
				auto it = tileNames.find(topMat);
				if (it != tileNames.end()) biome->topMaterial = (unsigned char)it->second;
			}
			std::string mat = props.value("material", "");
			if (!mat.empty()) {
				auto it = tileNames.find(mat);
				if (it != tileNames.end()) biome->material = (unsigned char)it->second;
			}

			// Apply mob spawn lists
			auto& spawn = j["spawning"];
			if (spawn.is_object()) {
				auto parseMobs = [&](const std::string& key, MobList& list) {
					list.clear();
					auto& arr = spawn[key];
					if (arr.is_array()) {
						for (auto& m : arr) {
							std::string entityId = m.value("entity", "");
							auto eit = entityNames.find(entityId);
							if (eit != entityNames.end()) {
								int weight = m.value("weight", 10);
								int min = m.value("min_count", 1);
								int max = m.value("max_count", 1);
								list.push_back(MobSpawnerData(eit->second, weight, min, max));
							}
						}
					}
				};
				parseMobs("creatures", biome->_friendlies);
				parseMobs("monsters", biome->_enemies);
				parseMobs("water_creatures", biome->_waterFriendlies);
			}
		}
	});

	printf("Biome::applyDefinitions: loaded %d biome definitions\n", count);
}
