"""Add 'type' field to all JSON definition files for DynamicDataLoader dispatch."""
import json
import os
import sys

ROOT = os.path.join(os.path.dirname(__file__), "..", "data", "minecraft")

# Map subdirectory -> type value
TYPE_MAP = {
    "tiles": "tile",
    "items": "item",
    "entities": "entity",
    "recipes": "recipe",
    "biomes": "biome",
    "dimensions": "dimension",
}

def add_type_to_file(path, type_val):
    with open(path, "r") as f:
        data = json.load(f)

    if not isinstance(data, dict):
        print(f"  skip (non-object): {path}")
        return False

    if data.get("type") == type_val:
        return False  # already has correct type

    data["type"] = type_val
    with open(path, "w") as f:
        json.dump(data, f, indent=2)
        f.write("\n")
    return True

def main():
    count = 0
    for subdir, type_val in TYPE_MAP.items():
        dir_path = os.path.join(ROOT, subdir)
        if not os.path.isdir(dir_path):
            continue
        for fname in sorted(os.listdir(dir_path)):
            if not fname.endswith(".json"):
                continue
            path = os.path.join(dir_path, fname)
            if add_type_to_file(path, type_val):
                count += 1
                print(f"  {path}")
    print(f"\nAdded type field to {count} files")

if __name__ == "__main__":
    main()
