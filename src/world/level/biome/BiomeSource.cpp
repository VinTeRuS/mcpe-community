#include "BiomeSource.h"
#include "Biome.h"
#include "../Level.h"
#include "../ChunkPos.h"

const float BiomeSource::zoom = 2 * 1;

const float BiomeSource::tempScale = zoom / 80.0f;
const float BiomeSource::downfallScale = zoom / 40.0f;
const float BiomeSource::noiseScale = 1 / 4.0f;

BiomeSource::BiomeSource()
{
}

BiomeSource::BiomeSource( Level* level )
:	rndTemperature(level->getSeed() * 9871),
	rndDownfall(level->getSeed() * 39811),
	rndNoise(level->getSeed() * 543321)
{
	temperatureMap = new PerlinSimplexNoise(&rndTemperature, 4);
	downfallMap = new PerlinSimplexNoise(&rndDownfall, 4);
	noiseMap = new PerlinSimplexNoise(&rndNoise, 2);
}

BiomeSource::~BiomeSource() {
	LOGI("Deleting biome maps...\n");
	delete temperatureMap;
	delete downfallMap;
	delete noiseMap;
}

Biome* BiomeSource::getBiome( const ChunkPos& chunk )
{
	return getBiome(chunk.x << 4, chunk.z << 4);
}

Biome* BiomeSource::getBiome( int x, int z )
{
	return getBiomeBlock(x, z, 1, 1)[0];
}

//float BiomeSource::getTemperature( int x, int z )
//{
//	temperatures = temperatureMap->getRegion(temperatures, x, z, 1, 1, tempScale, tempScale, 0.5f);
//	return temperatures[0];
//}

Biome** BiomeSource::getBiomeBlock( int x, int z, int w, int h )
{
	Biome** out = new Biome*[w * h];
	getBiomeBlockInternal(out, x, z, w, h);
	return out;
}

Biome** BiomeSource::getBiomeBlockInternal( Biome** biomes__, int x, int z, int w, int h )
{
	float* temperatures_ = temperatureMap->getRegion(0, x, z, w, w, tempScale, tempScale, 0.25f);
	float* downfalls_ = downfallMap->getRegion(0, x, z, w, w, downfallScale, downfallScale, 0.3333f);
	float* noises_ = noiseMap->getRegion(0, x, z, w, w, noiseScale, noiseScale, 0.588f);

	int pp = 0;
	for (int yy = 0; yy < w; yy++) {
		for (int xx = 0; xx < h; xx++) {
			float noise = (noises_[pp] * 1.1f + 0.5f);

			float split2 = 0.01f;
			float split1 = 1 - split2;
			float temperature = (temperatures_[pp] * 0.15f + 0.7f) * split1 + noise * split2;
			split2 = 0.002f;
			split1 = 1 - split2;
			float downfall = (downfalls_[pp] * 0.15f + 0.5f) * split1 + noise * split2;
			temperature = 1 - ((1 - temperature) * (1 - temperature));
			if (temperature < 0) temperature = 0;
			if (downfall < 0) downfall = 0;
			if (temperature > 1) temperature = 1;
			if (downfall > 1) downfall = 1;

			biomes__[pp++] = Biome::getBiome(temperature, downfall);
		}
	}

	delete[] temperatures_;
	delete[] downfalls_;
	delete[] noises_;

	return biomes__;
}

float* BiomeSource::getTemperatureBlock( int x, int z, int w, int h )
{
	float* temperatures_ = temperatureMap->getRegion(0, x, z, w, h, tempScale, tempScale, 0.25f);
	float* noises_ = noiseMap->getRegion(0, x, z, w, h, noiseScale, noiseScale, 0.588f);

	int pp = 0;
	for (int yy = 0; yy < w; yy++) {
		for (int xx = 0; xx < h; xx++) {
			float noise = (noises_[pp] * 1.1f + 0.5f);

			float split2 = 0.01f;
			float split1 = 1 - split2;
			float temperature = (temperatures_[pp] * 0.15f + 0.7f) * split1 + noise * split2;
			temperature = 1 - ((1 - temperature) * (1 - temperature));

			if (temperature < 0) temperature = 0;
			if (temperature > 1) temperature = 1;

			temperatures_[pp] = temperature;
			pp++;
		}
	}

	delete[] noises_;
	return temperatures_;
}

float* BiomeSource::getDownfallBlock( int x, int z, int w, int h )
{
	float* downfalls_ = downfallMap->getRegion(0, x, z, w, h, downfallScale, downfallScale, 0.3333f);
	float* noises_ = noiseMap->getRegion(0, x, z, w, h, noiseScale, noiseScale, 0.588f);

	int pp = 0;
	for (int yy = 0; yy < w; yy++) {
		for (int xx = 0; xx < h; xx++) {
			float noise = (noises_[pp] * 1.1f + 0.5f);

			float split2 = 0.002f;
			float split1 = 1 - split2;
			float downfall = (downfalls_[pp] * 0.15f + 0.5f) * split1 + noise * split2;

			if (downfall < 0) downfall = 0;
			if (downfall > 1) downfall = 1;

			downfalls_[pp] = downfall;
			pp++;
		}
	}

	delete[] noises_;
	return downfalls_;
}
