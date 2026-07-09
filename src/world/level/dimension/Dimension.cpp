#include "Dimension.h"
#include "NormalDayCycleDimension.h"

//#include "../levelgen/SimpleLevelSource.h"
#include "../levelgen/RandomLevelSource.h"
#include "../Level.h"
#include "../biome/BiomeSource.h"
#include "../chunk/ChunkSource.h"
#include "../tile/Tile.h"
#include "../../../util/Mth.h"
#include "../../../util/JsonLoader.h"

std::vector<DimensionDefinition> Dimension::s_definitions;

Dimension::Dimension()
:	foggy(false),
	ultraWarm(false),
	hasCeiling(false),
	biomeSource(NULL),
	id(0),
	seaLevel(63),
	dimHeight(128),
	worldSize(256),
	natural(false)
{
}

Dimension::~Dimension()
{
	delete biomeSource;
}

void Dimension::init( Level* level )
{
	this->level = level;
	init();
	updateLightRamp();
}

void Dimension::init()
{
	biomeSource = new BiomeSource(level);
}

/*virtual*/
bool Dimension::isValidSpawn(int x, int z) {
    int topTile = level->getTopTile(x, z);

	if (topTile == Tile::invisible_bedrock->id)
		return false;

    //if (topTile != Tile::sand->id) return false;
	if (!Tile::tiles[topTile]->isSolidRender()) return false;

    return true;
}

float Dimension::getTimeOfDay(long time, float a) {
	return 1;
}

ChunkSource* Dimension::createRandomLevelSource() {
	return new RandomLevelSource(
		level,
		level->getSeed(),
		level->getLevelData()->getGeneratorVersion(),
		!level->isClientSide && level->getLevelData()->getSpawnMobs());
	//return new PerformanceTestChunkSource(level);
}


void Dimension::updateLightRamp()
{
	float ambientLight = 0.05f;
	for (int i = 0; i <= 15; /*Level::MAX_BRIGHTNESS;*/ i++) {
		float v = (1 - i / (float) (16 /*Level::MAX_BRIGHTNESS*/));
		// Boosted ambient lightning by ten times.
		brightnessRamp[i] = ((1 - v) / (v * 3 + 1)) * (1 - ambientLight) + ambientLight * 3;
	}
}

float* Dimension::getSunriseColor( float td, float a )
{
	float span = 0.4f;
	float tt = Mth::cos(td * Mth::PI * 2) - 0.0f;
	float mid = -0.0f;
	if (tt >= mid - span && tt <= mid + span) {
		float aa = ((tt - mid) / span) * 0.5f + 0.5f;
		float mix = 1 - (((1 - Mth::sin(aa * Mth::PI))) * 0.99f);
		mix = mix * mix;
		sunriseCol[0] = (aa * 0.3f + 0.7f);
		sunriseCol[1] = (aa * aa * 0.7f + 0.2f);
		sunriseCol[2] = (aa * aa * 0.0f + 0.2f);
		sunriseCol[3] = mix;
		return sunriseCol;
	}
	return NULL;
}

Vec3 Dimension::getFogColor( float td, float a )
{
	float br = Mth::cos(td * Mth::PI * 2) * 2 + 0.5f;
	if (br < 0.0f) br = 0.0f;
	if (br > 1.0f) br = 1.0f;

	float r = ((defaultFogColor >> 16) & 0xff) / 255.0f;
	float g = ((defaultFogColor >> 8) & 0xff) / 255.0f;
	float b = ((defaultFogColor) & 0xff) / 255.0f;
	r *= br * 0.94f + 0.06f;
	g *= br * 0.94f + 0.06f;
	b *= br * 0.91f + 0.09f;
	return Vec3(r, g, b);
	//return Vec3(0.752941f, 0.847059f, 1);
}

bool Dimension::mayRespawn()
{
	return true;
}

Dimension* Dimension::getNew( int id )
{
	if (id == NORMAL) return new Dimension();
	if (id == NORMAL_DAYCYCLE) return new NormalDayCycleDimension();
	return NULL;
}

/*static*/
void Dimension::applyDefinitions() {
	auto& loader = JsonLoader::singleton();

	s_definitions.clear();
	int count = 0;

	loader.loadDir("minecraft", "dimensions", [&](const std::string& path, const json& data) {
		auto list = data["dimensions"];
		if (!list.is_array()) return;
		for (auto& j : list) {
			DimensionDefinition def;
			def.nameId = j.value("id", "");
			if (def.nameId.empty()) continue;

			std::string dimClass = j.value("class", "normal");
			if (dimClass == "normal_day_cycle") def.dimClass = Dimension::NORMAL_DAYCYCLE;
			else if (dimClass == "normal") def.dimClass = Dimension::NORMAL;
			else if (dimClass == "nether") def.dimClass = Dimension::NETHER;
			else if (dimClass == "end") def.dimClass = Dimension::END;
			else def.dimClass = Dimension::NORMAL;

			auto& props = j["properties"];
			if (props.is_object()) {
				if (props.contains("height") && props["height"].is_number())
					def.height = props["height"];
				if (props.contains("sea_level") && props["sea_level"].is_number())
					def.seaLevel = props["sea_level"];
				if (props.contains("world_size") && props["world_size"].is_number())
					def.worldSize = props["world_size"];
				if (props.contains("fog_color") && props["fog_color"].is_number())
					def.fogColor = props["fog_color"];
				if (props.contains("cloud_height") && props["cloud_height"].is_number())
					def.cloudHeight = props["cloud_height"];
				if (props.contains("natural") && props["natural"].is_boolean())
					def.natural = props["natural"];
				if (props.contains("foggy") && props["foggy"].is_boolean())
					def.foggy = props["foggy"];
				if (props.contains("ultra_warm") && props["ultra_warm"].is_boolean())
					def.ultraWarm = props["ultra_warm"];
				if (props.contains("has_ceiling") && props["has_ceiling"].is_boolean())
					def.hasCeiling = props["has_ceiling"];
				if (props.contains("respawn") && props["respawn"].is_boolean())
					def.respawn = props["respawn"];
			}

			s_definitions.push_back(def);
			count++;
		}
	});

	printf("Dimension::applyDefinitions: loaded %d dimension definitions\n", count);
}

/*static*/
void Dimension::handleJsonDefinition(const std::string& modId, const json& data) {
    (void)modId;
    auto list = data["dimensions"];
    if (!list.is_array()) return;

    for (auto& j : list) {
        DimensionDefinition def;
        def.nameId = j.value("id", "");
        if (def.nameId.empty()) continue;

        std::string dimClass = j.value("class", "normal");
        if (dimClass == "normal_day_cycle") def.dimClass = Dimension::NORMAL_DAYCYCLE;
        else if (dimClass == "normal") def.dimClass = Dimension::NORMAL;
        else if (dimClass == "nether") def.dimClass = Dimension::NETHER;
        else if (dimClass == "end") def.dimClass = Dimension::END;
        else def.dimClass = Dimension::NORMAL;

        auto& props = j["properties"];
        if (props.is_object()) {
            if (props.contains("height") && props["height"].is_number())
                def.height = props["height"];
            if (props.contains("sea_level") && props["sea_level"].is_number())
                def.seaLevel = props["sea_level"];
            if (props.contains("world_size") && props["world_size"].is_number())
                def.worldSize = props["world_size"];
            if (props.contains("fog_color") && props["fog_color"].is_number())
                def.fogColor = props["fog_color"];
            if (props.contains("cloud_height") && props["cloud_height"].is_number())
                def.cloudHeight = props["cloud_height"];
            if (props.contains("natural") && props["natural"].is_boolean())
                def.natural = props["natural"];
            if (props.contains("foggy") && props["foggy"].is_boolean())
                def.foggy = props["foggy"];
            if (props.contains("ultra_warm") && props["ultra_warm"].is_boolean())
                def.ultraWarm = props["ultra_warm"];
            if (props.contains("has_ceiling") && props["has_ceiling"].is_boolean())
                def.hasCeiling = props["has_ceiling"];
            if (props.contains("respawn") && props["respawn"].is_boolean())
                def.respawn = props["respawn"];
        }

        s_definitions.push_back(def);
    }
}

/*static*/
const DimensionDefinition* Dimension::getDefinition(const std::string& nameId) {
	for (auto& d : s_definitions)
		if (d.nameId == nameId) return &d;
	return nullptr;
}

/*static*/
const std::vector<DimensionDefinition>& Dimension::allDefinitions() {
	return s_definitions;
}

//
// DimensionFactory
//
#include "../storage/LevelData.h"
Dimension* DimensionFactory::createDefaultDimension(LevelData* data )
{
	// Try named definitions first
	auto* def = Dimension::getDefinition("minecraft:overworld");
	if (def) {
		Dimension* dim = Dimension::getNew(def->dimClass);
		if (dim) {
			dim->nameId = def->nameId;
			dim->seaLevel = def->seaLevel;
			dim->dimHeight = def->height;
			dim->worldSize = def->worldSize;
			dim->natural = def->natural;
			dim->foggy = def->foggy;
			dim->ultraWarm = def->ultraWarm;
			dim->hasCeiling = def->hasCeiling;
		}
		return dim;
	}
	return Dimension::getNew(Dimension::NORMAL_DAYCYCLE);
}
