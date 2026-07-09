#!/usr/bin/env python3
"""Generate biome JSON definition files from Biome.cpp initBiomes() analysis."""

import json, os

BIOMES_DIR = 'data/minecraft/biomes'
os.makedirs(BIOMES_DIR, exist_ok=True)

# Mapped from known entity IDs
ENTITY_ID_MAP = {
    'MobTypes::Sheep': 'minecraft:sheep',
    'MobTypes::Pig': 'minecraft:pig',
    'MobTypes::Chicken': 'minecraft:chicken',
    'MobTypes::Cow': 'minecraft:cow',
    'MobTypes::Spider': 'minecraft:spider',
    'MobTypes::Zombie': 'minecraft:zombie',
    'MobTypes::Skeleton': 'minecraft:skeleton',
    'MobTypes::Creeper': 'minecraft:creeper',
}

WEIGHT_MAP = {'MobTypes::Sheep': 12, 'MobTypes::Pig': 10, 'MobTypes::Chicken': 10, 'MobTypes::Cow': 8,
              'MobTypes::Spider': 8, 'MobTypes::Zombie': 12, 'MobTypes::Skeleton': 6, 'MobTypes::Creeper': 4}
MIN_MAP = {'MobTypes::Sheep': 2, 'MobTypes::Pig': 1, 'MobTypes::Chicken': 2, 'MobTypes::Cow': 2,
           'MobTypes::Spider': 2, 'MobTypes::Zombie': 2, 'MobTypes::Skeleton': 1, 'MobTypes::Creeper': 1}
MAX_MAP = {'MobTypes::Sheep': 3, 'MobTypes::Pig': 3, 'MobTypes::Chicken': 4, 'MobTypes::Cow': 3,
           'MobTypes::Spider': 3, 'MobTypes::Zombie': 4, 'MobTypes::Skeleton': 3, 'MobTypes::Creeper': 1}

class BiomeDef:
    def __init__(self, name_id, class_name, color, leaf_color, snow_covered, top_material, material, clear_friendlies, clear_water, clear_enemies, tree_feature_type):
        self.name_id = name_id
        self.class_name = class_name
        self.color = color
        self.leaf_color = leaf_color
        self.snow_covered = snow_covered
        self.top_material = top_material
        self.material = material
        self.clear_friendlies = clear_friendlies
        self.clear_water = clear_water
        self.clear_enemies = clear_enemies
        self.tree_feature_type = tree_feature_type

# All 11 biomes extracted from initBiomes()
biomes = [
    BiomeDef('rainforest', 'RainforestBiome', 0x08FA36, 0x1FF458, False, 'minecraft:grass', 'minecraft:dirt', False, False, False, 'oak'),
    BiomeDef('swampland', 'SwampBiome', 0x07F9B2, 0x8BAF48, False, 'minecraft:grass', 'minecraft:dirt', False, False, False, 'oak'),
    BiomeDef('seasonal_forest', 'Biome', 0x9BE023, 0x4EE031, False, 'minecraft:grass', 'minecraft:dirt', False, False, False, 'oak'),
    BiomeDef('forest', 'ForestBiome', 0x056621, 0x4EBA31, False, 'minecraft:grass', 'minecraft:dirt', False, False, False, 'birch_oak'),
    BiomeDef('savanna', 'FlatBiome', 0xD9E023, 0x4EE031, False, 'minecraft:grass', 'minecraft:dirt', False, False, False, 'none'),
    BiomeDef('shrubland', 'Biome', 0xA1AD20, 0x4EE031, False, 'minecraft:grass', 'minecraft:dirt', False, False, False, 'oak'),
    BiomeDef('taiga', 'TaigaBiome', 0x2EB153, 0x7BB731, True, 'minecraft:grass', 'minecraft:dirt', False, False, False, 'pine_spruce'),
    BiomeDef('desert', 'FlatBiome', 0xFA9418, 0x4EE031, False, 'minecraft:sand', 'minecraft:sand', True, True, False, 'none'),
    BiomeDef('plains', 'FlatBiome', 0xFFD910, 0x4EE031, False, 'minecraft:grass', 'minecraft:dirt', False, False, False, 'none'),
    BiomeDef('ice_desert', 'FlatBiome', 0xFFED93, 0xC4D339, True, 'minecraft:sand', 'minecraft:sand', True, False, False, 'none'),
    BiomeDef('tundra', 'Biome', 0x57EBF9, 0xC4D339, True, 'minecraft:grass', 'minecraft:dirt', False, False, False, 'oak'),
]

def default_mobs():
    return [
        {'entity': 'minecraft:sheep', 'weight': 12, 'min_count': 2, 'max_count': 3},
        {'entity': 'minecraft:pig', 'weight': 10, 'min_count': 1, 'max_count': 3},
        {'entity': 'minecraft:chicken', 'weight': 10, 'min_count': 2, 'max_count': 4},
        {'entity': 'minecraft:cow', 'weight': 8, 'min_count': 2, 'max_count': 3},
    ]

def default_enemies():
    return [
        {'entity': 'minecraft:spider', 'weight': 8, 'min_count': 2, 'max_count': 3},
        {'entity': 'minecraft:zombie', 'weight': 12, 'min_count': 2, 'max_count': 4},
        {'entity': 'minecraft:skeleton', 'weight': 6, 'min_count': 1, 'max_count': 3},
        {'entity': 'minecraft:creeper', 'weight': 4, 'min_count': 1, 'max_count': 1},
    ]

for b in biomes:
    j = {
        'format_version': '1',
        'namespace': 'minecraft',
        'biomes': [
            {
                'id': f'minecraft:{b.name_id}',
                'class': b.class_name,
                'properties': {
                    'color': b.color,
                    'leaf_color': b.leaf_color,
                    'snow_covered': b.snow_covered,
                    'top_material': b.top_material,
                    'material': b.material,
                    'tree_type': b.tree_feature_type,
                },
                'spawning': {
                    'creatures': [] if b.clear_friendlies else default_mobs(),
                    'monsters': [] if b.clear_enemies else default_enemies(),
                    'water_creatures': [] if b.clear_water else [],
                }
            }
        ]
    }

    fname = f'{BIOMES_DIR}/{b.name_id}.json'
    with open(fname, 'w') as f:
        json.dump(j, f, indent=2)
    print(f'Wrote {fname}')

print('Done')
