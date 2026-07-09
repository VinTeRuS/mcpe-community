#!/usr/bin/env python3
"""Generate entity JSON definition files from known data."""

import json

entities = [
    {
        "id": "minecraft:chicken",
        "numeric_id": 10,
        "class": "Chicken",
        "category": "creature",
        "properties": {"health": 4, "run_speed": 0.7, "width": 0.3, "height": 0.7},
        "texture": "mob/chicken.png",
        "fall_damage_immune": True,
        "sounds": {"ambient": "mob.chicken", "hurt": "mob.chickenhurt", "death": "mob.chickenhurt"},
        "drops": [{"item": "minecraft:feather", "min": 0, "max": 2}, {"item": "minecraft:chicken_raw", "min": 1, "max": 1}],
    },
    {
        "id": "minecraft:cow",
        "numeric_id": 11,
        "class": "Cow",
        "category": "creature",
        "properties": {"health": 10, "run_speed": 0.7, "width": 0.9, "height": 1.3},
        "texture": "mob/cow.png",
        "sounds": {"ambient": "mob.cow", "hurt": "mob.cowhurt", "death": "mob.cowhurt"},
        "drops": [{"item": "minecraft:leather", "min": 0, "max": 2}, {"item": "minecraft:beef_raw", "min": 1, "max": 3}],
    },
    {
        "id": "minecraft:pig",
        "numeric_id": 12,
        "class": "Pig",
        "category": "creature",
        "properties": {"health": 10, "run_speed": 0.7, "width": 0.9, "height": 0.9},
        "texture": "mob/pig.png",
        "sounds": {"ambient": "mob.pig", "hurt": "mob.pig", "death": "mob.pigdeath"},
        "drops": [{"item": "minecraft:porkchop_raw", "min": 0, "max": 2}],
    },
    {
        "id": "minecraft:sheep",
        "numeric_id": 13,
        "class": "Sheep",
        "category": "creature",
        "properties": {"health": 8, "run_speed": 0.7, "width": 0.9, "height": 1.3},
        "texture": "mob/sheep.png",
        "sounds": {"ambient": "mob.sheep", "hurt": "mob.sheep", "death": "mob.sheep"},
        "drops": [{"item": "minecraft:cloth", "min": 1, "max": 1}],
    },
    {
        "id": "minecraft:zombie",
        "numeric_id": 32,
        "class": "Zombie",
        "category": "monster",
        "properties": {"health": 12, "run_speed": 0.5, "width": 0.6, "height": 1.8, "attack_damage": 4, "armor_value": 2},
        "texture": "mob/zombie.png",
        "sounds": {"ambient": "mob.zombie", "hurt": "mob.zombiehurt", "death": "mob.zombiedeath"},
        "drops": [{"item": "minecraft:feather", "min": 0, "max": 1, "chance": 0.25}],
    },
    {
        "id": "minecraft:creeper",
        "numeric_id": 33,
        "class": "Creeper",
        "category": "monster",
        "properties": {"health": 16, "run_speed": 0.7, "width": 0.6, "height": 1.8, "attack_damage": 2},
        "texture": "mob/creeper.png",
        "sounds": {"ambient": "mob.creeper", "hurt": "mob.creeper", "death": "mob.creeperdeath"},
        "drops": [{"item": "minecraft:sulphur", "min": 0, "max": 2}],
    },
    {
        "id": "minecraft:skeleton",
        "numeric_id": 34,
        "class": "Skeleton",
        "category": "monster",
        "properties": {"health": 10, "run_speed": 0.7, "width": 0.6, "height": 1.8, "attack_damage": 2},
        "texture": "mob/skeleton.png",
        "sounds": {"ambient": "mob.skeletonhurt", "hurt": "mob.skeletonhurt", "death": "mob.skeletonhurt"},
        "drops": [{"item": "minecraft:arrow", "min": 0, "max": 2}, {"item": "minecraft:bone", "min": 0, "max": 2}],
        "equipment": {"main_hand": "minecraft:bow"},
    },
    {
        "id": "minecraft:spider",
        "numeric_id": 35,
        "class": "Spider",
        "category": "monster",
        "properties": {"health": 8, "run_speed": 0.5, "width": 1.4, "height": 0.9, "attack_damage": 2},
        "texture": "mob/spider.png",
        "climbs": True,
        "sounds": {"ambient": "mob.spider", "hurt": "mob.spider", "death": "mob.spiderdeath"},
        "drops": [{"item": "minecraft:string", "min": 0, "max": 2}],
    },
    {
        "id": "minecraft:pigzombie",
        "numeric_id": 36,
        "class": "PigZombie",
        "category": "monster",
        "properties": {"health": 12, "run_speed": 0.7, "width": 0.6, "height": 1.8, "attack_damage": 5},
        "texture": "mob/pigzombie.png",
        "fire_immune": True,
        "sounds": {"ambient": "mob.zombiepig.zpig", "hurt": "mob.zombiepig.zpighurt", "death": "mob.zombiepig.zpigdeath"},
        "drops": [{"item": "minecraft:gold_ingot", "min": 0, "max": 1}],
        "equipment": {"main_hand": "minecraft:sword_gold"},
    },
]

base_path = "/home/dxc/github/mcpe-community/data/minecraft/entities"
for e in entities:
    name = e["id"].split(":")[1]
    filename = f"{base_path}/{name}.json"
    with open(filename, "w") as f:
        json.dump(e, f, indent=2)
        f.write("\n")
    print(f"  {filename}")
print(f"\nGenerated {len(entities)} entity definition files")
