#!/usr/bin/env python3
"""Generate recipe JSON definition files from hardcoded C++ recipe data."""

import json, os

recipes_dir = "/home/dxc/github/mcpe-community/data/minecraft/recipes"
os.makedirs(recipes_dir, exist_ok=True)

def shaped(name, pattern, key, result_item, result_count=1, result_data=-1):
    """Shaped recipe template."""
    keys = {}
    for k, v in key.items():
        if isinstance(v, dict):
            keys[k] = v
        else:
            keys[k] = {"item": v}
    return {
        "type": "shaped",
        "result": {"item": result_item, "count": result_count, "data": result_data} if result_data >= 0 else {"item": result_item, "count": result_count},
        "pattern": pattern,
        "key": keys,
    }

def shapeless(name, ingredients, result_item, result_count=1, result_data=-1):
    """Shapeless recipe template."""
    ings = []
    for ing in ingredients:
        if isinstance(ing, dict):
            ings.append(ing)
        else:
            ings.append({"item": ing})
    return {
        "type": "shapeless",
        "result": {"item": result_item, "count": result_count, "data": result_data} if result_data >= 0 else {"item": result_item, "count": result_count},
        "ingredients": ings,
    }

def furnace(input_item, output_item, output_count=1):
    """Furnace recipe template."""
    return {
        "type": "furnace",
        "input": {"item": input_item},
        "output": {"item": output_item, "count": output_count},
    }

# ===== TOOL RECIPES =====
# Patterns: pickAxe=0, shovel=1, hatchet=2, hoe=3
# Materials: wood(0), stone(1), iron(2), diamond/emerald(3), gold(4)
tool_shapes = [
    (["XXX", " # ", " # "], "pickaxe"),
    (["X", "#", "#"], "shovel"),
    (["XX", "X#", " #"], "hatchet"),
    (["XX", " #", " #"], "hoe"),
]
tool_materials = [
    ("minecraft:wood", "pickaxe_wood", "shovel_wood", "hatchet_wood", "hoe_wood"),
    ("minecraft:stonebrick", "pickaxe_stone", "shovel_stone", "hatchet_stone", "hoe_stone"),
    ("minecraft:iron_ingot", "pickaxe_iron", "shovel_iron", "hatchet_iron", "hoe_iron"),
    ("minecraft:emerald", "pickaxe_emerald", "shovel_emerald", "hatchet_emerald", "hoe_emerald"),
    ("minecraft:gold_ingot", "pickaxe_gold", "shovel_gold", "shovel_gold", "hoe_gold"),
]

tools = []
for i, (mat_id, *tool_names) in enumerate(tool_materials):
    for j, (shape, tool_type) in enumerate(tool_shapes):
        name = f"tools/{tool_type}_{['wood','stone','iron','emerald','gold'][i]}"
        tools.append(shaped(name, shape, {"X": mat_id, "#": "minecraft:stick"}, tool_names[min(j, len(tool_names)-1)]))
tools.append(shaped("tools/shears", [" #", "# "], {"#": "minecraft:iron_ingot"}, "shears"))
with open(f"{recipes_dir}/tools.json", "w") as f:
    json.dump({"namespace": "minecraft", "recipes": tools}, f, indent=2)
print(f"  tools.json ({len(tools)} recipes)")

# ===== WEAPON RECIPES =====
weapons = []
weapon_materials = [
    ("minecraft:wood", "sword_wood"),
    ("minecraft:stonebrick", "sword_stone"),
    ("minecraft:iron_ingot", "sword_iron"),
    ("minecraft:emerald", "sword_emerald"),
    ("minecraft:gold_ingot", "sword_gold"),
]
for mat_id, sword_name in weapon_materials:
    weapons.append(shaped(f"weapons/{sword_name}", ["X", "X", "#"], {"X": mat_id, "#": "minecraft:stick"}, sword_name))
weapons.append(shaped("weapons/bow", [" #X", "# X", " #X"], {"X": "minecraft:string", "#": "minecraft:stick"}, "bow"))
weapons.append(shaped("weapons/arrow", ["X", "#", "Y"], {"X": "minecraft:flint", "#": "minecraft:stick", "Y": "minecraft:feather"}, "arrow", 4))
with open(f"{recipes_dir}/weapons.json", "w") as f:
    json.dump({"namespace": "minecraft", "recipes": weapons}, f, indent=2)
print(f"  weapons.json ({len(weapons)} recipes)")

# ===== ARMOR RECIPES =====
armor_shapes = [
    (["XXX", "X X"], "helmet"),
    (["X X", "XXX", "XXX"], "chestplate"),
    (["XXX", "X X", "X X"], "leggings"),
    (["X X", "X X"], "boots"),
]
armor_materials = [
    ("minecraft:leather", "helmet_cloth", "chestplate_cloth", "leggings_cloth", "boots_cloth"),
    ("minecraft:iron_ingot", "helmet_iron", "chestplate_iron", "leggings_iron", "boots_iron"),
    ("minecraft:emerald", "helmet_diamond", "chestplate_diamond", "leggings_diamond", "boots_diamond"),
    ("minecraft:gold_ingot", "helmet_gold", "chestplate_gold", "leggings_gold", "boots_gold"),
]
armor = []
for mat_id, helm, chest, legs, boots in armor_materials:
    for shape, piece in armor_shapes:
        name = {"helmet": helm, "chestplate": chest, "leggings": legs, "boots": boots}[piece]
        armor.append(shaped(f"armor/{name}", shape, {"X": mat_id}, name))
with open(f"{recipes_dir}/armor.json", "w") as f:
    json.dump({"namespace": "minecraft", "recipes": armor}, f, indent=2)
print(f"  armor.json ({len(armor)} recipes)")

# ===== ORE RECIPES =====
ore = []
ore_pairs = [
    ("minecraft:gold_block", "minecraft:gold_ingot", 9),
    ("minecraft:iron_block", "minecraft:iron_ingot", 9),
    ("minecraft:emerald_block", "minecraft:emerald", 9),
    ("minecraft:lapis_block", "minecraft:dye_powder", 9),
]
for block, item, count in ore_pairs:
    ore.append(shaped(f"ore/block_to_{item.split(':')[1]}", ["#"], {"#": block}, item, count))
    ore.append(shaped(f"ore/{block.split(':')[1]}_from_{item.split(':')[1]}", ["###", "###", "###"], {"#": item}, block, 1))
with open(f"{recipes_dir}/ores.json", "w") as f:
    json.dump({"namespace": "minecraft", "recipes": ore}, f, indent=2)
print(f"  ores.json ({len(ore)} recipes)")

# ===== FOOD RECIPES =====
food = []
food.append(shapeless("food/mushroom_stew",
    ["minecraft:mushroom", "minecraft:mushroom", "minecraft:bowl"], "mushroom_stew"))
food.append(shaped("food/melon_block", ["MMM", "MMM", "MMM"], {"M": "minecraft:melon"}, "melon_block"))
food.append(shaped("food/seeds_melon", ["M"], {"M": "minecraft:melon"}, "seeds_melon"))
with open(f"{recipes_dir}/food.json", "w") as f:
    json.dump({"namespace": "minecraft", "recipes": food}, f, indent=2)
print(f"  food.json ({len(food)} recipes)")

# ===== STRUCTURE RECIPES =====
struct = []
struct.append(shaped("structure/chest", ["###", "# #", "###"], {"#": "minecraft:wood"}, "chest"))
struct.append(shaped("structure/furnace", ["###", "# #", "###"], {"#": "minecraft:stonebrick"}, "furnace"))
struct.append(shaped("structure/workbench", ["##", "##"], {"#": "minecraft:wood"}, "workbench"))
struct.append(shaped("structure/stonecutter", ["##", "##"], {"#": "minecraft:stonebrick"}, "stonecutter"))
struct.append(shaped("structure/sandstone", ["##", "##"], {"#": "minecraft:sand"}, "sandstone"))
struct.append(shaped("structure/sandstone_smooth", ["##", "##"], {"#": "minecraft:sandstone"}, "sandstone", 4, 4))
struct.append(shaped("structure/sandstone_heiroglyphs", ["#", "#"], {"#": "minecraft:stone_slab"}, "sandstone", 1, 1))
struct.append(shaped("structure/stonebrick_smooth", ["##", "##"], {"#": "minecraft:rock"}, "stonebricksmooth", 4))
struct.append(shaped("structure/thin_glass", ["###", "###"], {"#": "minecraft:glass"}, "thin_glass", 16))
struct.append(shaped("structure/nether_brick", ["NN", "NN"], {"N": "minecraft:netherbrick"}, "nether_brick"))
struct.append(shaped("structure/quartz_block", ["NN", "NN"], {"N": "minecraft:nether_quartz"}, "quartz_block"))
with open(f"{recipes_dir}/structures.json", "w") as f:
    json.dump({"namespace": "minecraft", "recipes": struct}, f, indent=2)
print(f"  structures.json ({len(struct)} recipes)")

# ===== FURNACE RECIPES =====
furnace_recipes = [
    ("minecraft:iron_ore", "minecraft:iron_ingot"),
    ("minecraft:gold_ore", "minecraft:gold_ingot"),
    ("minecraft:ore_emerald", "minecraft:emerald"),
    ("minecraft:sand", "minecraft:glass"),
    ("minecraft:porkchop_raw", "minecraft:porkchop_cooked"),
    ("minecraft:beef_raw", "minecraft:beef_cooked"),
    ("minecraft:chicken_raw", "minecraft:chicken_cooked"),
    ("minecraft:stonebrick", "minecraft:rock"),
    ("minecraft:clay", "minecraft:brick"),
    ("minecraft:cactus", "minecraft:dye_powder"),
    ("minecraft:mushroom", "minecraft:dye_powder"),
    ("minecraft:tree_trunk", "minecraft:coal"),
    ("minecraft:netherrack", "minecraft:netherbrick"),
]
furnaces = [furnace(inp, out) for inp, out in furnace_recipes]
with open(f"{recipes_dir}/furnace.json", "w") as f:
    json.dump({"namespace": "minecraft", "recipes": furnaces}, f, indent=2)
print(f"  furnace.json ({len(furnaces)} recipes)")

# ===== DYE RECIPES =====
dye_shapeless = []
# Cloth dyeing (11 colors, skipping 0,3,7,8,15)
for i in range(16):
    if i in (0, 3, 7, 8, 15):
        continue
    dye_shapeless.append(shapeless(f"dye/cloth_{i}", ["minecraft:dye_powder", "minecraft:cloth"], "cloth", 1, i))
# Dye crafting
dye_shapeless.append(shapeless("dye/yellow", ["minecraft:flower"], "dye_powder", 2, 11))
dye_shapeless.append(shapeless("dye/white", ["minecraft:bone"], "dye_powder", 3, 15))
dye_shapeless.append(shapeless("dye/pink", ["minecraft:dye_powder", "minecraft:dye_powder"], "dye_powder", 2, 6))
dye_shapeless.append(shapeless("dye/orange", ["minecraft:dye_powder", "minecraft:dye_powder"], "dye_powder", 2, 14))
dye_shapeless.append(shapeless("dye/lime", ["minecraft:dye_powder", "minecraft:dye_powder"], "dye_powder", 2, 5))
dye_shapeless.append(shapeless("dye/light_blue", ["minecraft:dye_powder", "minecraft:dye_powder"], "dye_powder", 2, 12))
dye_shapeless.append(shapeless("dye/cyan", ["minecraft:dye_powder", "minecraft:dye_powder"], "dye_powder", 2, 9))
dye_shapeless.append(shapeless("dye/purple", ["minecraft:dye_powder", "minecraft:dye_powder"], "dye_powder", 2, 10))
dye_shapeless.append(shapeless("dye/magenta", ["minecraft:dye_powder", "minecraft:dye_powder", "minecraft:dye_powder", "minecraft:dye_powder"], "dye_powder", 4, 2))
with open(f"{recipes_dir}/dye.json", "w") as f:
    json.dump({"namespace": "minecraft", "recipes": dye_shapeless}, f, indent=2)
print(f"  dye.json ({len(dye_shapeless)} recipes)")

# ===== MISC RECIPES (from Recipes.cpp inline) =====
misc = []

# Paper & book
misc.append(shaped("misc/paper", ["###"], {"#": "minecraft:reeds"}, "paper", 3))
misc.append(shaped("misc/book", ["#", "#", "#"], {"#": "minecraft:paper"}, "book"))
# Fence & gate
misc.append(shaped("misc/fence", ["###", "###"], {"#": "minecraft:stick"}, "fence", 2))
misc.append(shaped("misc/fence_gate", ["#W#", "#W#"], {"#": "minecraft:stick", "W": "minecraft:wood"}, "fence_gate"))
# Bookshelf
misc.append(shaped("misc/bookshelf", ["###", "XXX", "###"], {"#": "minecraft:wood", "X": "minecraft:book"}, "bookshelf"))
# Storage blocks
misc.append(shaped("misc/snow_block", ["##", "##"], {"#": "minecraft:snowball"}, "snow"))
misc.append(shaped("misc/clay_block", ["##", "##"], {"#": "minecraft:clay"}, "clay"))
misc.append(shaped("misc/red_brick", ["##", "##"], {"#": "minecraft:brick"}, "red_brick"))
misc.append(shaped("misc/light_gem", ["##", "##"], {"#": "minecraft:yellow_dust"}, "light_gem"))
misc.append(shaped("misc/cloth", ["##", "##"], {"#": "minecraft:string"}, "cloth"))
# TNT
misc.append(shaped("misc/tnt", ["X#X", "#X#", "X#X"], {"X": "minecraft:sulphur", "#": "minecraft:sand"}, "tnt"))
# Slabs (all 6x)
slabs = [("cobblestone", "minecraft:stonebrick"), ("stone", "minecraft:rock"), ("sand", "minecraft:sandstone"),
         ("wood", "minecraft:wood"), ("brick", "minecraft:red_brick"), ("smoothbrick", "minecraft:stonebricksmooth")]
for name, ing in slabs:
    misc.append(shaped(f"misc/slab_{name}", ["###"], {"#": ing}, "stone_slab", 6, {"cobblestone": 0, "stone": 1, "sand": 2, "wood": 3, "brick": 4, "smoothbrick": 5}[name]))
# Ladder
misc.append(shaped("misc/ladder", ["# #", "###", "# #"], {"#": "minecraft:stick"}, "ladder", 2))
# Door & trapdoor
misc.append(shaped("misc/wood_door", ["##", "##", "##"], {"#": "minecraft:wood"}, "door_wood"))
misc.append(shaped("misc/trapdoor", ["###", "###"], {"#": "minecraft:wood"}, "trapdoor", 2))
# Sign
misc.append(shaped("misc/sign", ["###", "###", " X "], {"#": "minecraft:wood", "X": "minecraft:stick"}, "sign"))
# Sugar & wood
misc.append(shaped("misc/sugar", ["#"], {"#": "minecraft:reeds"}, "sugar"))
misc.append(shaped("misc/wood_planks", ["#"], {"#": "minecraft:tree_trunk"}, "wood", 4))
misc.append(shaped("misc/stick", ["#", "#"], {"#": "minecraft:wood"}, "stick", 4))
# Torch (coal + charcoal)
misc.append(shaped("misc/torch", ["X", "#"], {"X": "minecraft:coal", "#": "minecraft:stick"}, "torch", 4))
misc.append(shaped("misc/torch_charcoal", ["X", "#"], {"X": "minecraft:coal", "#": "minecraft:stick"}, "torch", 4))
# Bowl
misc.append(shaped("misc/bowl", ["# #", " # "], {"#": "minecraft:wood"}, "bowl", 4))
# Flint & steel
misc.append(shaped("misc/flint_steel", ["A ", " B"], {"A": "minecraft:iron_ingot", "B": "minecraft:flint"}, "flint_and_steel"))
# Bread
misc.append(shaped("misc/bread", ["###"], {"#": "minecraft:wheat"}, "bread"))
# Stairs (all 4x, L-shape)
stairs_list = [
    ("stairs_wood", "minecraft:wood"),
    ("stairs_stone", "minecraft:stonebrick"),
    ("stairs_brick", "minecraft:red_brick"),
    ("stairs_stonebricksmooth", "minecraft:stonebricksmooth"),
    ("stairs_netherbrick", "minecraft:nether_brick"),
]
for stair_name, ing in stairs_list:
    misc.append(shaped(f"misc/{stair_name}", ["#  ", "## ", "###"], {"#": ing}, stair_name, 4))
# Painting
misc.append(shaped("misc/painting", ["###", "#X#", "###"], {"#": "minecraft:stick", "X": "minecraft:cloth"}, "painting"))
# Bed
misc.append(shaped("misc/bed", ["###", "XXX"], {"#": "minecraft:cloth", "X": "minecraft:wood"}, "bed"))
# Nether reactor
misc.append(shaped("misc/nether_reactor", ["X#X", "X#X", "X#X"], {"#": "minecraft:emerald", "X": "minecraft:iron_ingot"}, "nether_reactor"))

with open(f"{recipes_dir}/misc.json", "w") as f:
    json.dump({"namespace": "minecraft", "recipes": misc}, f, indent=2)
print(f"  misc.json ({len(misc)} recipes)")

total = len(tools + weapons + armor + ore + food + struct + furnaces + dye_shapeless + misc)
print(f"\nTotal: {total} recipes")
