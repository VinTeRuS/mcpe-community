#ifndef NET_MINECRAFT_WORLD_LEVEL_BIOME__BiomeSource_H__
#define NET_MINECRAFT_WORLD_LEVEL_BIOME__BiomeSource_H__

//package net.minecraft.world.level.biome;

#include "../../../util/Random.h"
#include "../levelgen/synth/PerlinNoise.h"

typedef PerlinNoise PerlinSimplexNoise;
class Level;
class Biome;
class ChunkPos;

class BiomeSource
{
protected:
	BiomeSource();
public:
    BiomeSource(Level* level);
	virtual ~BiomeSource();

    virtual Biome* getBiome(const ChunkPos& chunk);
    virtual Biome* getBiome(int x, int z);

	virtual float* getTemperatureBlock(int x, int z, int w, int h);
	virtual float* getDownfallBlock(int x, int z, int w, int h);
	virtual Biome** getBiomeBlock(int x, int z, int w, int h);

private:
	Biome** getBiomeBlockInternal(Biome** biomes, int x, int z, int w, int h);

	PerlinSimplexNoise* temperatureMap;
	PerlinSimplexNoise* downfallMap;
	PerlinSimplexNoise* noiseMap;

	Random rndTemperature;
	Random rndDownfall;
	Random rndNoise;

	static const float zoom;

	static const float tempScale;
	static const float downfallScale;
	static const float noiseScale;
};

#endif /*NET_MINECRAFT_WORLD_LEVEL_BIOME__BiomeSource_H__*/
