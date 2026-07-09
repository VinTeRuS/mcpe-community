#ifndef NET_MINECRAFT_WORLD_LEVEL_DIMENSION__Dimension_H__
#define NET_MINECRAFT_WORLD_LEVEL_DIMENSION__Dimension_H__

//package net.minecraft.world.level.dimension;

#include <string>
#include <vector>
#include "../../phys/Vec3.h"

class Level;
class BiomeSource;
class ChunkSource;

struct DimensionDefinition {
	std::string nameId;
	int dimClass; // 0=NORMAL, 10=NORMAL_DAYCYCLE, etc
	int height = 128;
	int seaLevel = 63;
	int worldSize = 256;
	int fogColor = 0x80daff;
	int cloudHeight = 108;
	bool natural = false;
	bool foggy = false;
	bool ultraWarm = false;
	bool hasCeiling = false;
	bool respawn = true;
};

class Dimension
{
public:
	static const int NORMAL = 0;
	static const int NORMAL_DAYCYCLE = 10;
	static const int NETHER = 1;
	static const int END = 2;

	Dimension();
	virtual ~Dimension();
    virtual void init(Level* level);

	//@fix @port The caller is responsible for this ChunkSource, I presume
    virtual ChunkSource* createRandomLevelSource();

    virtual bool isValidSpawn(int x, int z);
	virtual bool isNaturalDimension() {
		return natural;
	}

    virtual float getTimeOfDay(long time, float a);
    virtual float* getSunriseColor(float td, float a);
    virtual Vec3 getFogColor(float td, float a);

    virtual bool mayRespawn();

	// @fix @port Caller is responsible (+ move this to a "factory method" outside?)
	// @NOTE: RIGHT NOW, Level deletes the dimension.
    static Dimension* getNew(int id);
	static void applyDefinitions();
	static const DimensionDefinition* getDefinition(const std::string& nameId);
	static const std::vector<DimensionDefinition>& allDefinitions();

protected:
	virtual void updateLightRamp();
	virtual void init();

public:
	Level* level;
	BiomeSource* biomeSource;
	bool foggy;
	bool ultraWarm;
	bool hasCeiling;
	float brightnessRamp[16];//Level::MAX_BRIGHTNESS + 1];
	int id;
	std::string nameId;
	int seaLevel;
	int dimHeight;
	int worldSize;
	bool natural;
protected:
	static const long defaultFogColor = 0x80daff;
	float sunriseCol[4];
private:
	static std::vector<DimensionDefinition> s_definitions;
};

class LevelData;
class DimensionFactory
{
public:
	static Dimension* createDefaultDimension(LevelData* data);
};

#endif /*NET_MINECRAFT_WORLD_LEVEL_DIMENSION__Dimension_H__*/
