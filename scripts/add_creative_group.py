#!/usr/bin/env python3
"""Add creative_group to tile/item JSONs.
Items: always.
Tiles: skip only fluids, fire, invisible bedrock, crops, farmland, carried variants, info_update.
"""
import json, os, glob

base = os.path.join(os.path.dirname(__file__), "..", "data", "minecraft")

# Tiles to skip (no creative appearance): numeric_id
skip_tiles = {7, 8, 9, 10, 11, 51, 59, 60, 95, 248, 249, 253, 254, 255}

count = 0

for f in glob.glob(os.path.join(base, "tiles", "*.json")):
    with open(f) as fh:
        data = json.load(fh)
    nid = data.get("numeric_id")
    if nid is None or nid in skip_tiles:
        continue
    if "creative_group" not in data or not data["creative_group"]:
        data["creative_group"] = "items"
        with open(f, "w") as fh:
            json.dump(data, fh, indent=2)
            fh.write("\n")
        count += 1

for f in glob.glob(os.path.join(base, "items", "*.json")):
    with open(f) as fh:
        data = json.load(fh)
    if "creative_group" not in data or not data["creative_group"]:
        cat = data.get("category", "items")
        data["creative_group"] = cat.lower() if cat else "items"
        with open(f, "w") as fh:
            json.dump(data, fh, indent=2)
            fh.write("\n")
        count += 1

print(f"Added creative_group to {count} files")
