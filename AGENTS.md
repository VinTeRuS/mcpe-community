# MCPE-Community Data-Driven Migration

## Caveman Mode

Every response use caveman style:
- Drop articles (a/an/the), filler (just/really/basically/actually/simply), pleasantries (sure/certainly/of course/happy to), hedging
- Fragments OK. Short synonyms. Technical terms exact. Code blocks unchanged.
- Pattern: `[thing] [action] [reason]. [next step].`
- Ultra level: abbreviate (DB/auth/config/req/res/fn/impl), strip conjunctions, arrows for causality (X → Y), one word when one word enough
- Exceptions: security warnings, destructive confirmations, multi-step sequences where order matters — write normal clarity there. Resume caveman after.
- Commits: conventional commit format, subject ≤50 chars.
- **ALWAYS update AGENTS.md → Conventions → progress (this file) after every major change. Include commit hash and file count. Do not repeat yourself**

## Sanity Check (auto-run after every change)

After ANY code change, run these in order. Fix all issues before committing.

1. **Build both targets** — `./build.sh MinecraftClient debug && ./build.sh MinecraftServer debug`
2. **Launch test** — `timeout 10 ./debug/MinecraftClient 2>&1; [ $? -eq 124 ]` (exit 124 = clean timeout, no crash)
3. **Server build only** — `./build.sh MinecraftServer debug` (catches `STANDALONE_SERVER` compile errors)
4. **No warnings in new code** — Check compiler output for new warnings. Fix or document them.
5. **Commit** only after all 4 pass. If launch fails, revert + fix.

Exceptions: Phase 2+ may break client launch temporarily (GL overhaul). Mark in commit message: `[skip sanity]` and explain why.

## Project

MCPE v0.6.1 alpha source. Ported to C++17. Builds on Linux via CMake + SDL2.

**Binary targets:** `MinecraftClient` (GUI) and `MinecraftServer` (headless dedicated).

**Build:**
```
./build.sh [MinecraftClient|MinecraftServer] [debug|release]
```
Debug: `-O0 -g`. Release: `-O3 -w -fpermissive`.

**Deps:** SDL2, OpenGL, OpenAL, ZLIB, PNG, GLEW, EGL (optional), Threads.

Current branch: `data-migration` — full conversion to JSON-driven game.

## Repo Stats

- 1073 source files (716 `.h`, 354 `.cpp`, 3 `.mm`)
- ~178K lines total
- 0 tests
- 0 JSON configs (except CMakePresets.json)

## Source Layout

```
src/
  main_linux.cpp          Linux client entry (SDL2)
  main_dedicated.cpp      Server entry (headless)
  main_android.cpp        Android entry
  NinecraftApp.h/.cpp     Top-level app (extends Minecraft)
  App.h                   Abstract app base
  AppPlatform.h           Platform abstraction
  client/
    Minecraft.cpp/.h      Core game class
    Options.cpp/.h        Key=value options.txt
    gui/                  Screen + component hierarchy
    renderer/             Chunk rendering, GL abstraction
    particle/             Particle types
    sound/                Sound engine
    multiplayer/          Network layer
  world/
    entity/               Entity hierarchy + AI goals
    item/                 Item hierarchy + recipes
    level/                Level, biome, chunk, dimension
      tile/               Block types (86 files)
      levelgen/           Terrain generation
    inventory/            Container menus
    phys/                 AABB, Vec3, hit result
  network/
    packet/               51 MCPE protocol packets
  nbt/                    Named Binary Tag serialization
  platform/               CThread, time, audio (AL/SL/Win)
  raknet/                 Embedded RakNet (245 files)
  server/                 Server-level logic
  locale/                 I18n .lang system
```

## Architecture (current)

- `App → Minecraft → NinecraftApp` hierarchy
- Fixed-function GL 1.x / GLES 1.1 via `gles.cpp` wrapper
- Chunks: 16x128x16, `unsigned char[32768]` per chunk (byte block IDs → max 255)
- Tiles: `Tile::tiles[256]` fixed array + global parallel arrays for properties
- Items: `Item::items[512]` with IDs shifted +256
- Entities: switch-based `EntityFactory` + `MobFactory`
- World: fixed 256x256 blocks (`CHUNK_CACHE_WIDTH=16`)
- Biomes: temperature+downfall 2D map, 11 biomes
- Recipes: hardcoded in `Recipes` singleton
- UI: hardcoded pixel positions, no layout engine
- No scripting, no plugin system, no data-driven content

## Design Flaws (fix before data-drive)

### Critical

| # | Flaw | File(s) | Fix |
|---|---|---|---|
| 1 | `unsigned char[32768]` chunk storage caps 255 block types | src/world/level/chunk/ | uint32 per block (ID 16b+data 8b+light 8b) |
| 2 | Global parallel arrays `solid[256]`, `lightBlock[256]` etc | Tile.h | `BlockProperties` struct + map lookup |
| 3 | Two-phase construction `new Tile()+ .init()` | Tile.cpp | Single `BlockDefinition::create(props)` |
| 4 | Switch factories for everything (tiles, items, entities) | Tile.cpp, Item.cpp, EntityFactory.h | `Registry<T>` pattern |
| 5 | World seed bug: chunk RNG ignores world seed | RandomLevelSource.cpp | seed = worldSeed ^ (x*P1 + z*P2) |
| 6 | `#if 1` forces touch click model everywhere | Screen.cpp | runtime bool `useTouchReleaseModel` |
| 7 | Linux forced touch screens | ScreenChooser.cpp | runtime `useTouchScreens` option |
| 8 | BiomeSource thread-unsafe (shared mutable array) | BiomeSource.cpp | per-call output array |
| 9 | `Item::maxDamage` defaults 32 → non-tools damageable | Item.cpp | default 0 |
| 10 | `ItemInstance::isNull()` false-positive on id=0 | ItemInstance.h | check id==0 only |
| 11 | No string IDs — only numeric | everywhere | `nameId` field on all registrables |
| 12 | Caves disabled (`caveFeature.apply()` commented) | RandomLevelSource.cpp | re-enable + JSON config |
| 13 | Fire system gutted (10+ early returns) | FireTile.cpp | fix + JSON config |
| 14 | Occlusion culling dead (`checkQueryResults` empty) | LevelRenderer.cpp | rewrite in GL 3.3 phase |
| 15 | Platform `#ifdef` spaghetti | 50+ files | `Platform` singleton, runtime caps |
| 16 | Dual AI systems (old `updateAi()` + new `newServerAiStep()`) | Mob.cpp | deprecate old path |
| 17 | SynchedEntityData 5-bit ID limit (max 31) | SynchedEntityData.h | wider ID |
| 18 | No entity chunk persistence | Level.cpp | per-chunk entity list |
| 19 | Ad-hoc metadata encoding per tile (no blockstate contract) | all tile/.h | unified blockstate system |
| 20 | `StairTile` delegates ~20 virtuals back to base — stale if base changes | StairTile.h | compose, don't delegate |

### Moderate

| # | Flaw | Fix |
|---|---|---|
| 21 | Float physics degrades at distance | double (opt-in) or 64-8-24 fixed |
| 22 | Public fields everywhere (Entity::x,y,z...) | encapsulate |
| 23 | OreFeature only replaces `Tile::rock` | JSON-define replaceable blocks |
| 24 | Passive material system (behavior in Tile, not material) | material behaviors JSON |
| 25 | Explosion resistance on Tile not Material | explodeable in JSON |
| 26 | Font UV hardcoded 128x128 | read from texture size |
| 27 | GUI scale fixed 1/3 | configurable |
| 28 | Water/lava paired via `id+1` assumption | JSON fluid definition |
| 29 | Creative tabs hardcoded | JSON creative inventory |
| 30 | `isSolidRender()` virtual + global array duplication | BlockProperties only |

## Master Plan — Phases

### Phase 0 — Foundation (fix flaws, prep registries)

```
[0.1] chunk storage: uint32 per block → remove 256 cap
[0.2] BlockProperties struct + map → kill global parallel arrays
[0.3] single-pass BlockDefinition → kill two-phase ctor
[0.4] Registry<T> for all (block/item/entity/recipe/biome/feature)
[0.5] fix seed bug
[0.6] fix touch/click/Linux forcing → runtime bools
[0.7] fix BiomeSource thread safety
[0.8] fix Item maxDamage default 0
[0.9] fix ItemInstance::isNull()
[0.10] add string IDs to all registries
[0.11] Platform singleton, remove ifdefs
[0.12] re-enable cave + fire (gate behind JSON config later)
```

### Phase 1 — JSON Data-Driven

```
[1.1] JsonLoader: parse data/<namespace>/*.json
[1.2] block definitions JSON (material, hardness, resistance, sound, shape, drops, textures, light, behavior, fluid props, variants)
[1.3] item definitions JSON (tier, food, armor, tool, max stack)
[1.4] entity definitions JSON (health, speed, hitbox, spawn rules, drops, AI goal list)
[1.5] recipe definitions JSON (shaped, shapeless, furnace, stonecutter)
[1.6] biome definitions JSON (temp, downfall, top/fill, trees, mobs, colors)
[1.7] dimension definitions JSON (height, sea level, world size, noise params, biomes)
[1.8] creative inventory JSON (tabs, groups, items)
[1.9] UI definitions JSON (widget tree, position, size, text, action)
[1.10] settings/options JSON schema
[1.11] mod overlay system (data/<mod>/ loads after data/minecraft/)
```

JSON only — no scripting.

### Phase 2 — GL 3.3+ Rendering Overhaul (Sodium-class)

```
[2.1] GL 3.3 core profile context
[2.2] shader-based chunk rendering (replace fixed-function GL)
[2.3] worker thread chunk rebuild (geometry off main thread)
[2.4] indexed tris (pre-compute index buffer, glDrawElements)
[2.5] VBO streaming (glMapBufferRange ring buffer)
[2.6] state sorting (shader→texture→blend per draw call batch)
[2.7] transparent geometry sorting (back-to-front per frame)
[2.8] occlusion culling (software rasterizer + GPU query fallback)
[2.9] per-frame frustum cull (SIMD plane-AABB)
[2.10] dirty chunk priority queue (near=immediate, far=spread)
```

### Phase 3 — World & Dimension System

```
[3.1] dimension definitions from JSON (Phase 1.7)
[3.2] multi-dimension support (Nether, End via DimensionFactory)
[3.3] configurable world size from JSON (256 default)
[3.4] dynamic chunk load/unload (LRU cache, save/load disk)
[3.5] RegionFile format for variable/extensible worlds
[3.6] player-bounded chunk grid (old unload, new load on move)
[3.7] noise generator params from JSON
```

Keep 256x256 default. Configurable. Infinite via streaming.

### Phase 4 — Full Deferred Shader Pipeline

```
[4.1] G-buffer (albedo, normal, specular, material ID, depth)
[4.2] PBR materials (metallic-roughness from JSON)
[4.3] forward overlay (transparent, entities, particles, GUI)
[4.4] dynamic lighting (light buffer, dirty propagation)
[4.5] shadow mapping (CSM)
[4.6] post-processing (tone mapping, bloom, SSAO, FXAA/TAA)
[4.7] entity GPU skinning
[4.8] texture arrays (GL_TEXTURE_2D_ARRAY for blocks)
```

### Phase 5 — Modding UX & Polish

```
[5.1] JSON schema validation on load
[5.2] hot-reload /reload command
[5.3] mod enable/disable GUI
[5.4] documentation + example mod pack
[5.5] world save migration
```

## Phase Dependencies

```
Phase 0 ───────────────────────────────────────────┐
   │                                                │
   ▼                                                │
Phase 1 (all JSON) ─────────────────────────────────┤
   │                                                │
   ├──────┬──────────────────────┐                  │
   ▼      ▼                      ▼                  │
Phase 2  Phase 3           Phase 4 (needs P2)        │
(GL3.3) (infinite world)   (deferred)                │
   │      │                      │                  │
   └──────┴──────────────────────┘                  │
              ▼                                     │
          Phase 5 ←─────────────────────────────────┘
```

## Key Files Reference

| File | Purpose |
|---|---|
| `src/main_linux.cpp` | Linux client entry point |
| `src/main_dedicated.cpp` | Server entry point |
| `src/NinecraftApp.h/.cpp` | Top-level app |
| `src/client/Minecraft.cpp/.h` | Core game loop |
| `src/client/renderer/LevelRenderer.cpp/.h` | Chunk rendering orchestrator |
| `src/client/renderer/Chunk.cpp/.h` | Per-chunk state + rebuild |
| `src/client/renderer/TileRenderer.cpp/.h` | Block tessellation dispatch (16 shapes) |
| `src/client/renderer/Tesselator.cpp/.h` | Geometry builder |
| `src/client/renderer/gles.cpp/.h` | GL abstraction layer |
| `src/client/renderer/Textures.cpp/.h` | Texture loading + caching |
| `src/client/gui/Screen.cpp/.h` | Screen base class |
| `src/client/gui/Gui.cpp/.h` | HUD overlay |
| `src/client/gui/screens/ScreenChooser.cpp/.h` | Screen factory |
| `src/world/entity/EntityFactory.h/.cpp` | Entity creation switch |
| `src/world/entity/MobFactory.h` | Mob creation switch |
| `src/world/entity/EntityTypes.h` | Entity ID constants |
| `src/world/entity/SynchedEntityData.h/.cpp` | Network sync for entities |
| `src/world/entity/ai/goal/Goal.h` | AI goal base |
| `src/world/item/Item.h/.cpp` | Item base + initItems() |
| `src/world/item/ItemInstance.h/.cpp` | Item stack |
| `src/world/item/crafting/Recipes.h/.cpp` | All hardcoded recipes |
| `src/world/item/crafting/ShapedRecipe.h` | Shaped recipe |
| `src/world/item/crafting/ShapelessRecipe.h` | Shapeless recipe |
| `src/world/level/tile/Tile.h/.cpp` | Tile base + initTiles() (90 types) |
| `src/world/level/tile/entity/TileEntity.h/.cpp` | Tile entity factory |
| `src/world/level/levelgen/RandomLevelSource.cpp` | Terrain generation |
| `src/world/level/levelgen/feature/` | Features (trees, ores, lakes, etc) |
| `src/world/level/biome/Biome.h/.cpp` | Biome definitions |
| `src/world/level/biome/BiomeSource.h/.cpp` | Biome map generation |
| `src/world/level/material/Material.h/.cpp` | Material types |
| `src/world/level/dimension/Dimension.h` | Dimension base |
| `src/world/level/Explosion.cpp` | Explosion mechanics |
| `src/CMakeLists.txt` | Build definition |
| `src/network/packet/` | All 51 MCPE packet types |

## Conventions

- **C++17** standard
- No RTTI, no exceptions (embedded RakNet may use exceptions)
- Fixed-function GL → GL 3.3 core (Phase 2)
- JSON schema: snake_case keys. `namespace:id` string IDs. Arrays of definition objects.
- Registry pattern: `Registry<T>` with `register(name, factory_fn)`, `get(name)`, `all()`
- Blockstate: property map `{"facing": "north", "open": true}` → unified metadata
- Keep numeric IDs for internal array perf, string IDs for JSON → map at load time
- UI: retained-mode widget tree with constraint layout engine (Phase 1.9)
- World size: 256 default, configurable, dynamic streaming
- Mod overlay: `data/<mod>/` files override `data/minecraft/`

## Progress

### Batch 1 (committed: `0d8a227`)
- [0.5] Seed bug: `RandomLevelSource.cpp:495` XOR world seed
- [0.6] `Screen.cpp` `#if 1` → runtime `useTouchscreen()`; `ScreenChooser.cpp` Linux touch forcing removed
- [0.7] `BiomeSource` thread safety: removed shared mutable arrays, per-call allocation
- [0.8] `Item::maxDamage` default 32 → 0 (`Item.h:217`)
- [0.9] `ItemInstance::isNull()` check id==0 only (`ItemInstance.cpp:51`)
- Fix pre-existing link error: `g_logFile` in `main_dedicated.cpp`

### Batch 2 (committed: `bff555d`)
- [0.4] `src/util/Registry.h` — header-only `Registry<T>` template

### Batch 3 (committed: `e2b4b84`)
- [0.10] `nameId` string on Tile/Item

### Batch 4 (committed: `3fe1915`)
- [0.12] Re-enable cave generation (`RandomLevelSource.cpp`) + fire system (`FireTile.cpp`)

### Batch 5 (committed: `5b38eab`)
- [0.1] `unsigned char[32768]` → `uint32` chunk storage (16b ID + 8b data + 8b light)
- `LevelChunk.h/cpp`, `Tile.h`, `Level.h/cpp` updated for new storage format
- 7 files, ~30 lines changed

### Batch 6 (committed: `f054f21`)
- [0.2] BlockProperties struct kill global parallel arrays
- `Tile.h` adds `BlockProperties` struct + `getProperties(id)`; all read/write sites updated
- 18 files, ±117 lines

### Batch 7 (committed: `e3dca71`)
- [0.3] Single-pass `Tile::create<T>(args...)` factory
- Two-phase `(new X())->init()` → `Tile::create<X>(...)`
- Registration + shape init moves into base Tile constructors
- `TileEntity::Sign` ODR definition fix
- 3 files, +121/-107

### Batch 8 (committed: `5fdd17a`)
- [0.11] `src/Platform.h` — header-only runtime Platform singleton
- Initialized in `main_linux.cpp` + `main_dedicated.cpp`
- DEMO_MODE ifdefs converted in 3 screen files
- 5 files, +24/-6

### Batch 9 (committed: `c5b1027`)
- Fix: null-guard in `Tile::getProperties()` for unregistered tile IDs (0)
- Fix: missing `buttonClicked()` call in non-touch mouse release path
- 2 files, +6/-1

### Batch 10 (committed: `84678f3`)
- [1.1] JsonLoader: parse `data/<namespace>/*.json` with overlay support
- [1.2] 89 tile JSON definition files in `data/minecraft/tiles/`
- [1.2] `TileDefinition` struct + `Tile::applyDefinitions()` via JsonLoader
- [1.2] `Material::byName()` string→material lookup
- `.gitignore`: `data/` removed (JSON defs are tracked source files)
- 98 files changed, +1360/-4

### Batch 11 (committed: `060b5fa`)
- [1.3] 95 item JSON definition files in `data/minecraft/items/`
- [1.3] `ItemDefinition` struct + `Item::applyDefinitions()` via JsonLoader
- 98 files changed, +1205/-17

### Batch 12 (committed: `b4035f9`)
- [1.4] 9 entity JSON definition files in `data/minecraft/entities/`
- [1.4] `EntityDefinition` struct + `EntityDefinition::applyDefinitions()` via JsonLoader
- Generator script `scripts/gen_entity_defs.py`
- 14 files changed, +498/-1

### Batch 13 (committed: `120b64c`)
- [1.5] 136 recipe JSON definitions in `data/minecraft/recipes/` (9 group files)
- [1.5] `Recipes::applyDefinitions()` via JsonLoader
- Generator script `scripts/gen_recipe_defs.py`
- 6 C++ sources changed, 10 recipe files, 1 script

### Pending
- [1.6] biome definitions JSON

### Phase 0 Legwork
- [0.1] uint32 chunk storage ✔
- [0.2] BlockProperties struct ✔
- [0.3] Single-pass Tile::create<T> ✔
- [0.4] Registry<T> template ✔
- [0.5] Seed bug fix ✔
- [0.6] Touch/click/Linux forcing → runtime bools ✔
- [0.7] BiomeSource thread safety ✔
- [0.8] Item::maxDamage default 0 ✔
- [0.9] ItemInstance::isNull() fix ✔
- [0.10] nameId string on Tile/Item ✔
- [0.11] Platform singleton, remove ifdefs ✔
- [0.12] Re-enable cave + fire ✔
