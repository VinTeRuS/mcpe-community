#include "TileInclude.h"
#include "../Level.h"
#include "../../entity/player/Player.h"
#include "../../entity/item/ItemEntity.h"
#include "../../item/Item.h"
#include "../../item/TileItem.h"

#include "../../../util/Random.h"
#include "../../../util/JsonLoader.h"
#include "../../entity/Entity.h"
#include "../LevelSource.h"
#include "../material/Material.h"
#include "../../phys/AABB.h"
#include "../../phys/HitResult.h"
#include "../../phys/Vec3.h"
#include "../../../locale/I18n.h"
#include "../../item/ClothTileItem.h"
#include <unordered_map>

#include "../../item/AuxDataTileItem.h"
#include "../../item/LeafTileItem.h"
#include "../../item/StoneSlabTileItem.h"
#include "../../item/SaplingTileItem.h"
#include "../../item/ItemCategory.h"

const int Tile::RENDERLAYER_OPAQUE          = 0;
const int Tile::RENDERLAYER_ALPHATEST       = 1;
const int Tile::RENDERLAYER_BLEND           = 2;

const std::string Tile::TILE_DESCRIPTION_PREFIX("tile.");

const Tile::SoundType Tile::SOUND_NORMAL("stone", 1, 1);
const Tile::SoundType Tile::SOUND_WOOD("wood", 1, 1);
const Tile::SoundType Tile::SOUND_GRAVEL("gravel", 1, 1);
const Tile::SoundType Tile::SOUND_GRASS("grass", 0.5f, 1);
const Tile::SoundType Tile::SOUND_STONE("stone", 1, 1);
const Tile::SoundType Tile::SOUND_METAL("stone", 1, 1.5f);
const Tile::SoundType Tile::SOUND_GLASS("stone", "random.glass", 1, 1);
const Tile::SoundType Tile::SOUND_CLOTH("cloth", 1, 1);

#ifdef PRE_ANDROID23
	const Tile::SoundType Tile::SOUND_SAND("sand", 0.45f, 1);
#else
	const Tile::SoundType Tile::SOUND_SAND("sand", "step.gravel", 1, 1);
#endif

const Tile::SoundType Tile::SOUND_SILENT("", 0, 0);

Tile* Tile::tiles[] = {NULL};

Tile* Tile::sand        = NULL;
Tile* Tile::sandStone   = NULL;
const int SANDSTONE_TEXTURES[] = { 0 + 16 * 12, 5 + 16 * 14, 6 + 16 * 14 };
const int SANDSTONE_TEXTURE_COUNT = 3;

Tile* Tile::stoneBrick  = NULL;
Tile* Tile::redBrick    = NULL;
Tile* Tile::wood        = NULL;
Tile* Tile::sapling     = NULL;
Tile* Tile::glass       = NULL;
Tile* Tile::web	        = NULL;
Tile* Tile::thinGlass   = NULL;
Tile* Tile::calmWater   = NULL;
Tile* Tile::calmLava    = NULL;
Tile* Tile::gravel      = NULL;
Tile* Tile::rock        = NULL;
Tile* Tile::unbreakable = NULL;
Tile* Tile::dirt        = NULL;
Tile* Tile::grass       = NULL;
Tile* Tile::ice         = NULL;
Tile* Tile::clay        = NULL;
Tile* Tile::farmland    = NULL;
Tile* Tile::stoneSlab   = NULL;
Tile* Tile::stoneSlabHalf=NULL;
Tile* Tile::cloth       = NULL;
Tile* Tile::flower      = NULL;
Tile* Tile::rose        = NULL;
Tile* Tile::mushroom1   = NULL;
Tile* Tile::mushroom2   = NULL;
Tile* Tile::topSnow     = NULL;
Tile* Tile::treeTrunk   = NULL;
Tile* Tile::snow        = NULL;
LeafTile* Tile::leaves  = NULL;
Tile* Tile::emeraldOre  = NULL;
Tile* Tile::redStoneOre = NULL;
Tile* Tile::redStoneOre_lit = NULL;
Tile* Tile::goldOre     = NULL;
Tile* Tile::ironOre     = NULL;
Tile* Tile::coalOre     = NULL;
Tile* Tile::lapisOre    = NULL;
Tile* Tile::lapisBlock	= NULL;
Tile* Tile::reeds       = NULL;
Tile* Tile::ladder      = NULL;
Tile* Tile::obsidian    = NULL;
Tile* Tile::tnt         = NULL;
Tile* Tile::bookshelf	= NULL;
Tile* Tile::sign        = NULL;
Tile* Tile::wallSign    = NULL;
Tile* Tile::mossStone	= NULL;
Tile* Tile::torch       = NULL;
Tile* Tile::water       = NULL;
Tile* Tile::lava        = NULL;
FireTile* Tile::fire    = NULL;
Tile* Tile::invisible_bedrock = NULL;
Tile* Tile::goldBlock   = NULL;
Tile* Tile::ironBlock   = NULL;
Tile* Tile::emeraldBlock= NULL;
Tile* Tile::workBench   = NULL;
Tile* Tile::stonecutterBench = NULL;
Tile* Tile::crops		= NULL;
Tile* Tile::furnace     = NULL;
Tile* Tile::furnace_lit = NULL;
Tile* Tile::chest		= NULL;
Tile* Tile::lightGem    = NULL;
Tile* Tile::stairs_wood = NULL;
Tile* Tile::stairs_stone= NULL;
Tile* Tile::stairs_brick= NULL;
Tile* Tile::door_wood   = NULL;
Tile* Tile::door_iron   = NULL;
Tile* Tile::cactus      = NULL;

Tile* Tile::melon       = NULL;
Tile* Tile::melonStem   = NULL;

Tile* Tile::bed         = NULL;
Tile* Tile::tallgrass   = NULL;
Tile* Tile::trapdoor    = NULL;
Tile* Tile::stoneBrickSmooth = NULL;
const int STONE_BRICK_TEXTURES[] = { 6 + 16 * 3, 4 + 16 * 6, 5 + 16 * 6 };
const int STONE_BRICK_TEXTURE_COUNT = 3;

Tile* Tile::fence       = NULL;
Tile* Tile::fenceGate   = NULL;

Tile* Tile::info_updateGame1 = NULL;
Tile* Tile::info_updateGame2 = NULL;
Tile* Tile::info_reserved6   = NULL;
Tile* Tile::grass_carried    = NULL;
LeafTile* Tile::leaves_carried   = NULL;

Tile* Tile::netherReactor = NULL;
Tile* Tile::glowingObsidian = NULL;

Tile* Tile::stairs_stoneBrickSmooth   = NULL;
Tile* Tile::netherBrick   = NULL;
Tile* Tile::netherrack   = NULL;
Tile* Tile::stairs_netherBricks   = NULL;
Tile* Tile::stairs_sandStone   = NULL;
Tile* Tile::quartzBlock   = NULL;
Tile* Tile::stairs_quartz   = NULL;

/*static*/
TileDefinition parseTileDefinition(const json& j) {
    TileDefinition def;
    def.nameId = j.value("name_id", "");
    def.className = j.value("class", "");
    def.material = j.value("material", "");
    def.soundType = j.value("sound_type", "");
    def.renderLayer = j.value("render_layer", "opaque");
    def.shape = j.value("shape", "");
    def.category = j.value("category", "");
    def.creativeGroup = j.value("creative_group", "");
    def.numericId = j.value("numeric_id", -1);
    def.tex = j.value("tex", -1);
    def.lightBlock = j.value("light_block", -1);
    def.lightEmission = j.value("light_emission", -1);
    def.solid = j.value("solid", true);
    def.translucent = j.value("translucent", false);
    def.hardness = j.value("hardness", -1.0f);
    def.resistance = j.value("resistance", -1.0f);
    if (j.contains("ticking")) { def.ticking = j["ticking"]; def.hasTicking = true; }
    return def;
}

/*static*/
void Tile::applyDefinitions() {
    auto& loader = JsonLoader::singleton();

    // Sound type lookup
    static const std::unordered_map<std::string, const SoundType*> s_soundMap = {
        {"stone", &SOUND_STONE},
        {"wood", &SOUND_WOOD},
        {"gravel", &SOUND_GRAVEL},
        {"grass", &SOUND_GRASS},
        {"metal", &SOUND_METAL},
        {"glass", &SOUND_GLASS},
        {"cloth", &SOUND_CLOTH},
        {"sand", &SOUND_SAND},
        {"silent", &SOUND_SILENT},
        {"normal", &SOUND_NORMAL},
    };

    int loaded = 0;
    loader.loadDir("minecraft", "tiles", [&](const std::string& path, const json& data) {
        TileDefinition def = parseTileDefinition(data);
        if (def.numericId < 0 || def.numericId >= 256)
            return;
        Tile* tile = Tile::tiles[def.numericId];
        if (!tile)
            return;

        // Apply nameId
        if (!def.nameId.empty())
            tile->setNameId(def.nameId);

        // Apply sound type
        if (!def.soundType.empty()) {
            auto it = s_soundMap.find(def.soundType);
            if (it != s_soundMap.end())
                tile->soundType = it->second;
        }

        // Apply hardness
        if (def.hardness >= 0)
            tile->destroySpeed = def.hardness;

        // Apply resistance (stored as * 3 internally, matching setExplodeable)
        if (def.resistance >= 0)
            tile->explosionResistance = def.resistance * 3.0f;

        // Apply light block
        if (def.lightBlock >= 0)
            tile->properties.lightBlock = def.lightBlock;

        // Apply light emission (0-15 range, stored as MAX_BRIGHTNESS * f internally)
        if (def.lightEmission >= 0)
            tile->properties.lightEmission = def.lightEmission;

        // Apply solid/translucent
        tile->properties.solid = def.solid;
        tile->properties.translucent = def.translucent;

        // Apply ticking
        if (def.hasTicking)
            tile->properties.shouldTick = def.ticking;

        // Apply category
        if (!def.category.empty()) {
            int cat = -1;
            if (def.category == "structures") cat = 1;
            else if (def.category == "decorations") cat = 8;
            else if (def.category == "tools") cat = 2;
            else if (def.category == "food_armor") cat = 4;
            else if (def.category == "mechanisms") cat = 16;
            if (cat >= 0) tile->category = cat;
        }

        // Apply creative group
        tile->creativeGroup = def.creativeGroup;

        // Apply tex
        if (def.tex >= 0)
            tile->tex = def.tex;

        loaded++;
    });

    printf("Tile::applyDefinitions: loaded %d tile definitions from JSON\n", loaded);
}

// ─── Tile Factory System ────────────────────────────────────────
static std::unordered_map<std::string, Tile::TileFactory>& s_tileFactories() {
    static std::unordered_map<std::string, Tile::TileFactory> factories;
    return factories;
}

void Tile::registerTileClass(const std::string& name, TileFactory factory) {
    s_tileFactories()[name] = std::move(factory);
}

/*static*/
void Tile::initTileFactories() {
    // Helper templates
    auto regTex = [](const std::string& name, auto fn) {
        registerTileClass(name, fn);
    };

    // Basic generic cube (most common)
    registerTileClass("Tile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 1;
        const Material* mat = def.material.empty() ? Material::stone : Material::byName(def.material);
        auto* t = new Tile(id, tex, mat);
        t->init();
        return t;
    });

    // ── Register all known subclasses ────────────────────────
    // Simple (int id, int tex)
    regTex("StoneTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 1; auto* t = new StoneTile(id, tex); t->init(); return t; });
    regTex("DirtTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 2; auto* t = new DirtTile(id, tex); t->init(); return t; });
    regTex("OreTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 1; auto* t = new OreTile(id, tex); t->init(); return t; });
    regTex("GravelTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 19; auto* t = new GravelTile(id, tex); t->init(); return t; });
    regTex("WebTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 11; auto* t = new WebTile(id, tex); t->init(); return t; });
    regTex("ClayTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 16*4+8; auto* t = new ClayTile(id, tex); t->init(); return t; });
    regTex("ReedTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 16*4+9; auto* t = new ReedTile(id, tex); t->init(); return t; });
    regTex("LadderTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 3+5*16; auto* t = new LadderTile(id, tex); t->init(); return t; });
    regTex("CactusTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 16*4+6; auto* t = new CactusTile(id, tex); t->init(); return t; });
    regTex("TorchTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 5*16; auto* t = new TorchTile(id, tex); t->init(); return t; });
    regTex("TopSnowTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 16*4+2; auto* t = new TopSnowTile(id, tex); t->init(); return t; });
    regTex("IceTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 16*4+3; auto* t = new IceTile(id, tex); t->init(); return t; });
    regTex("SnowTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 16*4+2; auto* t = new SnowTile(id, tex); t->init(); return t; });
    regTex("TntTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 8; auto* t = new TntTile(id, tex); t->init(); return t; });
    regTex("BookshelfTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 35; auto* t = new BookshelfTile(id, tex); t->init(); return t; });
    regTex("MetalTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 1; auto* t = new MetalTile(id, tex); t->init(); return t; });
    regTex("FenceTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 4; auto* t = new FenceTile(id, tex); t->init(); return t; });
    regTex("Bush", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 13; auto* t = new Bush(id, tex); t->init(); return t; });
    regTex("Mushroom", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 13+16; auto* t = new Mushroom(id, tex); t->init(); return t; });
    regTex("TallGrass", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 2*16+7; auto* t = new TallGrass(id, tex); t->init(); return t; });
    regTex("Sapling", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 15; auto* t = new Sapling(id, tex); t->init(); return t; });
    regTex("CropTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 8+5*16; auto* t = new CropTile(id, tex); t->init(); return t; });
    regTex("LightGemTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 9+16*6; auto* t = new LightGemTile(id, tex, Material::glass); t->init(); return t; });
    regTex("CarriedTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 1; auto* t = new CarriedTile(id, 3, 12*16+12); t->init(); return t; });

    // (int id, int tex, const Material*)
    regTex("HeavyTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 18; auto* t = new HeavyTile(id, tex, Material::byName(def.material.empty()?"sand":def.material)); t->init(); return t; });
    regTex("ThinFenceTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 1+3*16; auto* t = new ThinFenceTile(id, tex, 4+9*16, Material::glass, false); t->init(); return t; });

    // (int id)
    regTex("GrassTile", [](int id, const TileDefinition&) -> Tile* { auto* t = new GrassTile(id); t->init(); return t; });
    regTex("BedTile", [](int id, const TileDefinition&) -> Tile* { auto* t = new BedTile(id); t->init(); return t; });
    regTex("ClothTile", [](int id, const TileDefinition&) -> Tile* { auto* t = new ClothTile(id); t->init(); return t; });
    regTex("ChestTile", [](int id, const TileDefinition&) -> Tile* { auto* t = new ChestTile(id); t->init(); return t; });
    regTex("FarmTile", [](int id, const TileDefinition&) -> Tile* { auto* t = new FarmTile(id); t->init(); return t; });
    regTex("MelonTile", [](int id, const TileDefinition&) -> Tile* { auto* t = new MelonTile(id); t->init(); return t; });
    regTex("StonecutterTile", [](int id, const TileDefinition&) -> Tile* { auto* t = new StonecutterTile(id); t->init(); return t; });
    regTex("TreeTile", [](int id, const TileDefinition&) -> Tile* { auto* t = new TreeTile(id); t->init(); return t; });
    regTex("QuartzBlockTile", [](int id, const TileDefinition&) -> Tile* { auto* t = new QuartzBlockTile(id); t->init(); return t; });
    regTex("WorkbenchTile", [](int id, const TileDefinition&) -> Tile* { auto* t = new WorkbenchTile(id); t->init(); return t; });

    // Special constructors
    registerTileClass("GlassTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 49;
        auto* t = new GlassTile(id, tex, def.material.empty() ? Material::glass : Material::byName(def.material), false);
        t->init();
        return t;
    });

    registerTileClass("LeafTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 4 + 3 * 16;
        auto* t = new LeafTile(id, tex);
        t->init();
        return t;
    });

    registerTileClass("StairTile", [](int id, const TileDefinition& def) -> Tile* {
        // Stairs need a base tile reference; stub for now
        auto* t = new StairTile(id, Tile::stoneBrick ? Tile::stoneBrick : Tile::rock);
        t->init();
        return t;
    });

    registerTileClass("StoneSlabTile", [](int id, const TileDefinition& def) -> Tile* {
        bool full = (id == 43);
        auto* t = new StoneSlabTile(id, full);
        t->init();
        return t;
    });

    registerTileClass("FurnaceTile", [](int id, const TileDefinition& def) -> Tile* {
        bool lit = (id == 62);
        auto* t = new FurnaceTile(id, lit);
        t->init();
        return t;
    });

    registerTileClass("SignTile", [](int id, const TileDefinition&) -> Tile* {
        bool standing = (id == 63);
        auto* t = new SignTile(id, TileEntityType::Sign, standing);
        t->init();
        return t;
    });

    registerTileClass("DoorTile", [](int id, const TileDefinition& def) -> Tile* {
        const Material* mat = (id == 71) ? Material::metal : Material::wood;
        auto* t = new DoorTile(id, mat);
        t->init();
        return t;
    });

    registerTileClass("RedStoneOreTile", [](int id, const TileDefinition& def) -> Tile* {
        bool lit = (id == 74);
        int tex = def.tex >= 0 ? def.tex : 16 * 3 + 3;
        auto* t = new RedStoneOreTile(id, tex, lit);
        t->init();
        return t;
    });

    registerTileClass("ObsidianTile", [](int id, const TileDefinition& def) -> Tile* {
        bool glow = (id == 246);
        int tex = def.tex >= 0 ? def.tex : (glow ? 10 + 16 * 13 : 37);
        auto* t = new ObsidianTile(id, tex, glow);
        t->init();
        return t;
    });

    registerTileClass("InvisibleTile", [](int id, const TileDefinition& def) -> Tile* {
        auto* t = new InvisibleTile(id, 0, def.material.empty() ? Material::stone : Material::byName(def.material));
        t->init();
        return t;
    });

    registerTileClass("FireTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 1 * 16 + 15;
        auto* t = new FireTile(id, tex);
        t->init();
        return t;
    });

    registerTileClass("TrapDoorTile", [](int id, const TileDefinition&) -> Tile* {
        auto* t = new TrapDoorTile(id, Material::wood);
        t->init();
        return t;
    });

    registerTileClass("StemTile", [](int id, const TileDefinition&) -> Tile* {
        auto* t = new StemTile(id, Tile::melon ? Tile::melon : Tile::melon);
        t->init();
        return t;
    });

    registerTileClass("FenceGateTile", [](int id, const TileDefinition&) -> Tile* {
        auto* t = new FenceGateTile(id, 4);
        t->init();
        return t;
    });

    registerTileClass("NetherReactor", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 10 + 14 * 16;
        auto* t = new NetherReactor(id, tex, def.material.empty() ? Material::metal : Material::byName(def.material));
        t->init();
        return t;
    });

    // MultiTextureTile and SandStoneTile use int array for textures — skip for now
    registerTileClass("MultiTextureTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 1;
        auto* t = new Tile(id, tex, def.material.empty() ? Material::stone : Material::byName(def.material));
        t->init();
        return t;
    });
    registerTileClass("SandStoneTile", [](int id, const TileDefinition& def) -> Tile* {
        int tex = def.tex >= 0 ? def.tex : 1;
        auto* t = new Tile(id, tex, def.material.empty() ? Material::stone : Material::byName(def.material));
        t->init();
        return t;
    });

    registerTileClass("LiquidTileDynamic", [](int id, const TileDefinition& def) -> Tile* {
        (void)def;
        auto* t = new LiquidTileDynamic(id, (id == 8 || id == 9) ? Material::water : Material::lava);
        t->init();
        return t;
    });

    registerTileClass("LiquidTileStatic", [](int id, const TileDefinition& def) -> Tile* {
        (void)def;
        auto* t = new LiquidTileStatic(id, (id == 8 || id == 9) ? Material::water : Material::lava);
        t->init();
        return t;
    });
}

// ─── Apply TileDefinition properties to an existing tile ─────
static void applyDefToTile(Tile* tile, const TileDefinition& def) {
    if (!def.nameId.empty())
        tile->setNameId(def.nameId);

    static const std::unordered_map<std::string, const Tile::SoundType*> s_soundMap = {
        {"stone", &Tile::SOUND_STONE},
        {"wood", &Tile::SOUND_WOOD},
        {"gravel", &Tile::SOUND_GRAVEL},
        {"grass", &Tile::SOUND_GRASS},
        {"metal", &Tile::SOUND_METAL},
        {"glass", &Tile::SOUND_GLASS},
        {"cloth", &Tile::SOUND_CLOTH},
        {"sand", &Tile::SOUND_SAND},
        {"silent", &Tile::SOUND_SILENT},
        {"normal", &Tile::SOUND_NORMAL},
    };
    if (!def.soundType.empty()) {
        auto it = s_soundMap.find(def.soundType);
        if (it != s_soundMap.end())
            tile->soundType = it->second;
    }

    if (def.hardness >= 0)
        tile->destroySpeed = def.hardness;
    if (def.resistance >= 0)
        tile->explosionResistance = def.resistance * 3.0f;
    if (def.lightBlock >= 0)
        tile->properties.lightBlock = def.lightBlock;
    if (def.lightEmission >= 0)
        tile->properties.lightEmission = def.lightEmission;
    tile->properties.solid = def.solid;
    tile->properties.translucent = def.translucent;
    if (def.hasTicking)
        tile->properties.shouldTick = def.ticking;

    if (!def.category.empty()) {
        int cat = -1;
        if (def.category == "structures") cat = 1;
        else if (def.category == "decorations") cat = 8;
        else if (def.category == "tools") cat = 2;
        else if (def.category == "food_armor") cat = 4;
        else if (def.category == "mechanisms") cat = 16;
        if (cat >= 0) tile->category = cat;
    }
    tile->creativeGroup = def.creativeGroup;
    if (def.tex >= 0)
        tile->tex = def.tex;

    // Auto-generate descriptionId from nameId if missing
    if (!def.nameId.empty()) {
        std::string desc = def.nameId;
        auto colon = desc.find(':');
        if (colon != std::string::npos) desc = desc.substr(colon + 1);
        tile->setDescriptionId(desc);
    }
}

/*static*/
Tile* Tile::createFromDefinition(const TileDefinition& def) {
    if (def.numericId < 0 || def.numericId >= 256)
        return nullptr;
    if (Tile::tiles[def.numericId]) {
        applyDefToTile(Tile::tiles[def.numericId], def);
        return Tile::tiles[def.numericId];
    }

    std::string cname = def.className.empty() ? "Tile" : def.className;
    auto& factories = s_tileFactories();
    auto it = factories.find(cname);
    Tile* tile = nullptr;
    if (it != factories.end()) {
        tile = it->second(def.numericId, def);
    } else {
        printf("Tile: unknown class '%s' for id %d, falling back to basic\n", cname.c_str(), def.numericId);
        it = factories.find("Tile");
        if (it != factories.end())
            tile = it->second(def.numericId, def);
    }
    if (!tile) return nullptr;

    applyDefToTile(tile, def);
    return tile;
}

/*static*/
void Tile::resolveStaticPointers() {
    struct { const char* nameId; Tile*& ptr; } mapping[] = {
        {"minecraft:stone", Tile::rock},
        {"minecraft:grass", Tile::grass},
        {"minecraft:dirt", Tile::dirt},
        {"minecraft:stonebrick", Tile::stoneBrick},
        {"minecraft:wood", Tile::wood},
        {"minecraft:sapling", Tile::sapling},
        {"minecraft:bedrock", Tile::unbreakable},
        {"minecraft:water", Tile::water},
        {"minecraft:calm_water", Tile::calmWater},
        {"minecraft:lava", Tile::lava},
        {"minecraft:calm_lava", Tile::calmLava},
        {"minecraft:sand", Tile::sand},
        {"minecraft:gravel", Tile::gravel},
        {"minecraft:gold_ore", Tile::goldOre},
        {"minecraft:iron_ore", Tile::ironOre},
        {"minecraft:coal_ore", Tile::coalOre},
        {"minecraft:log", Tile::treeTrunk},
        {"minecraft:glass", Tile::glass},
        {"minecraft:lapis_ore", Tile::lapisOre},
        {"minecraft:lapis_block", Tile::lapisBlock},
        {"minecraft:sandstone", Tile::sandStone},
        {"minecraft:bed", Tile::bed},
        {"minecraft:web", Tile::web},
        {"minecraft:tallgrass", Tile::tallgrass},
        {"minecraft:cloth", Tile::cloth},
        {"minecraft:flower", Tile::flower},
        {"minecraft:rose", Tile::rose},
        {"minecraft:mushroom", Tile::mushroom1},
        {"minecraft:mushroom_red", Tile::mushroom2},
        {"minecraft:gold_block", Tile::goldBlock},
        {"minecraft:iron_block", Tile::ironBlock},
        {"minecraft:stone_slab", Tile::stoneSlab},
        {"minecraft:stone_slab_half", Tile::stoneSlabHalf},
        {"minecraft:brick", Tile::redBrick},
        {"minecraft:tnt", Tile::tnt},
        {"minecraft:bookshelf", Tile::bookshelf},
        {"minecraft:moss_stone", Tile::mossStone},
        {"minecraft:obsidian", Tile::obsidian},
        {"minecraft:torch", Tile::torch},
        {"minecraft:stairs_wood", Tile::stairs_wood},
        {"minecraft:chest", Tile::chest},
        {"minecraft:emerald_ore", Tile::emeraldOre},
        {"minecraft:emerald_block", Tile::emeraldBlock},
        {"minecraft:workbench", Tile::workBench},
        {"minecraft:crops", Tile::crops},
        {"minecraft:farmland", Tile::farmland},
        {"minecraft:furnace", Tile::furnace},
        {"minecraft:lit_furnace", Tile::furnace_lit},
        {"minecraft:sign", Tile::sign},
        {"minecraft:door_wood", Tile::door_wood},
        {"minecraft:ladder", Tile::ladder},
        {"minecraft:stairs_stone", Tile::stairs_stone},
        {"minecraft:wall_sign", Tile::wallSign},
        {"minecraft:door_iron", Tile::door_iron},
        {"minecraft:redstone_ore", Tile::redStoneOre},
        {"minecraft:lit_redstone_ore", Tile::redStoneOre_lit},
        {"minecraft:top_snow", Tile::topSnow},
        {"minecraft:ice", Tile::ice},
        {"minecraft:snow", Tile::snow},
        {"minecraft:cactus", Tile::cactus},
        {"minecraft:clay", Tile::clay},
        {"minecraft:reeds", Tile::reeds},
        {"minecraft:fence", Tile::fence},
        {"minecraft:light_gem", Tile::lightGem},
        {"minecraft:invisible_bedrock", Tile::invisible_bedrock},
        {"minecraft:trapdoor", Tile::trapdoor},
        {"minecraft:stone_brick_smooth", Tile::stoneBrickSmooth},
        {"minecraft:thin_glass", Tile::thinGlass},
        {"minecraft:melon", Tile::melon},
        {"minecraft:melon_stem", Tile::melonStem},
        {"minecraft:fence_gate", Tile::fenceGate},
        {"minecraft:stairs_brick", Tile::stairs_brick},
        {"minecraft:stairs_stone_brick_smooth", Tile::stairs_stoneBrickSmooth},
        {"minecraft:nether_brick", Tile::netherBrick},
        {"minecraft:stairs_nether_brick", Tile::stairs_netherBricks},
        {"minecraft:stairs_sandstone", Tile::stairs_sandStone},
        {"minecraft:quartz_block", Tile::quartzBlock},
        {"minecraft:stairs_quartz", Tile::stairs_quartz},
        {"minecraft:stonecutter", Tile::stonecutterBench},
        {"minecraft:glowing_obsidian", Tile::glowingObsidian},
        {"minecraft:nether_reactor", Tile::netherReactor},
        {"minecraft:grass_carried", Tile::grass_carried},
    };
    for (auto& m : mapping) {
        for (int i = 0; i < 256; i++) {
            if (Tile::tiles[i] && Tile::tiles[i]->getNameId() == m.nameId) {
                m.ptr = Tile::tiles[i];
                break;
            }
        }
    }
    // Subclass pointers (LeafTile*, FireTile*)
    for (int i = 0; i < 256; i++) {
        if (!Tile::tiles[i]) continue;
        const std::string& nid = Tile::tiles[i]->getNameId();
        if (nid == "minecraft:leaves") Tile::leaves = (LeafTile*)Tile::tiles[i];
        else if (nid == "minecraft:leaves_carried") Tile::leaves_carried = (LeafTile*)Tile::tiles[i];
        else if (nid == "minecraft:fire") Tile::fire = (FireTile*)Tile::tiles[i];
    }
}

/*static*/
void Tile::handleJsonDefinition(const std::string& modId, const json& data) {
    (void)modId;
    TileDefinition def = parseTileDefinition(data);
    if (def.numericId < 0 || def.numericId >= 256)
        return;
    Tile* tile = Tile::tiles[def.numericId];
    if (tile) {
        applyDefToTile(tile, def);
    } else {
        createFromDefinition(def);
    }
}

/*static*/
void Tile::initTiles() {
	rock        = Tile::create<StoneTile>(1, 1)->setDestroyTime(1.5f)->setExplodeable(10)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("stone");
	grass       = (GrassTile*) Tile::create<GrassTile>(2)->setDestroyTime(0.6f)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Structures)->setDescriptionId("grass");
	dirt        = Tile::create<DirtTile>(3, 2)->setDestroyTime(0.5f)->setSoundType(SOUND_GRAVEL)->setCategory(ItemCategory::Structures)->setDescriptionId("dirt");
	stoneBrick  = Tile::create<Tile>(4, 16, Material::stone)->setDestroyTime(2.0f)->setExplodeable(10)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("stonebrick");
	wood        = Tile::create<Tile>(5, 4, Material::wood)->setDestroyTime(2.0f)->setExplodeable(5)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Structures)->setDescriptionId("wood");
	sapling     = Tile::create<Sapling>(6, 15)->setDestroyTime(0.0f)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Structures)->setDescriptionId("sapling");//->sendTileData();
	unbreakable = Tile::create<Tile>(7, 17, Material::stone)->setDestroyTime(-1)->setExplodeable(6000000)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("bedrock");
	water       = Tile::create<LiquidTileDynamic>(8, Material::water)->setDestroyTime(100.0f)->setLightBlock(3)->setCategory(ItemCategory::Structures)->setDescriptionId("water");
	calmWater   = Tile::create<LiquidTileStatic>(9, Material::water)->setDestroyTime(100.0f)->setLightBlock(3)->setCategory(ItemCategory::Structures)->setDescriptionId("water");
	lava        = Tile::create<LiquidTileDynamic>(10, Material::lava)->setDestroyTime(00.0f)->setLightEmission(1.0f)->setLightBlock(255)->setCategory(ItemCategory::Structures)->setDescriptionId("lava"); // 00.0?
	calmLava    = Tile::create<LiquidTileStatic>(11, Material::lava)->setDestroyTime(100.0f)->setLightEmission(1.0f)->setLightBlock(255)->setCategory(ItemCategory::Structures)->setDescriptionId("lava");
	sand        = Tile::create<HeavyTile>(12, 18)->setDestroyTime(0.5f)->setSoundType(SOUND_SAND)->setCategory(ItemCategory::Structures)->setDescriptionId("sand");
	gravel      = Tile::create<GravelTile>(13, 19)->setDestroyTime(0.6f)->setSoundType(SOUND_GRAVEL)->setCategory(ItemCategory::Structures)->setDescriptionId("gravel");
	goldOre     = Tile::create<OreTile>(14, 32)->setDestroyTime(3.0f)->setExplodeable(5)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Decorations)->setDescriptionId("oreGold");
	ironOre     = Tile::create<OreTile>(15, 33)->setDestroyTime(3.0f)->setExplodeable(5)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Decorations)->setDescriptionId("oreIron");
	coalOre     = Tile::create<OreTile>(16, 34)->setDestroyTime(3.0f)->setExplodeable(5)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("oreCoal");
	treeTrunk   = Tile::create<TreeTile>(17)->setDestroyTime(2.0f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Structures)->setDescriptionId("log");
	leaves      = (LeafTile*) Tile::create<LeafTile>(18, 4 + 3 * 16)->setDestroyTime(0.2f)->setLightBlock(1)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Structures)->setDescriptionId("leaves");

	glass       = Tile::create<GlassTile>(20, 49, Material::glass, false)->setDestroyTime(0.3f)->setSoundType(SOUND_GLASS)->setCategory(ItemCategory::Structures)->setDescriptionId("glass");
	lapisOre    = Tile::create<OreTile>(21, 10 * 16)->setDestroyTime(3.0f)->setExplodeable(5)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("oreLapis");
	lapisBlock	= Tile::create<Tile>(22, 9 * 16, Material::stone)->setDestroyTime(3.0f)->setExplodeable(5)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Decorations)->setDescriptionId("blockLapis");
	sandStone   = Tile::create<SandStoneTile>(24, (const int*)&SANDSTONE_TEXTURES, SANDSTONE_TEXTURE_COUNT)->setSoundType(SOUND_STONE)->setDestroyTime(0.8f)->setCategory(ItemCategory::Structures)->setDescriptionId("sandStone");
	bed         = Tile::create<BedTile>(26)->setDestroyTime(0.2f)->setCategory(ItemCategory::Structures)->setDescriptionId("bed");
	web	        = Tile::create<WebTile>(30, 11)->setLightBlock(1)->setDestroyTime(4.0f)->setCategory(ItemCategory::Decorations)->setDescriptionId("web");
	tallgrass   = Tile::create<TallGrass>(31, 2 * 16 + 7)->setDestroyTime(0.0f)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Decorations)->setDescriptionId("tallgrass");
	cloth       = Tile::create<ClothTile>(35)->setDestroyTime(0.8f)->setSoundType(SOUND_CLOTH)->setCategory(ItemCategory::Structures)->setDescriptionId("cloth");

	flower      = Tile::create<Bush>(37, 13)->setDestroyTime(0.0f)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Decorations)->setDescriptionId("flower");
	rose        = Tile::create<Bush>(38, 12)->setDestroyTime(0.0f)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Decorations)->setDescriptionId("rose");
	mushroom1   = Tile::create<Mushroom>(39, 13 + 16)->setDestroyTime(0.0f)->setSoundType(SOUND_GRASS)->setLightEmission(2 / 16.0f)->setCategory(ItemCategory::Decorations)->setDescriptionId("mushroom");
	mushroom2   = Tile::create<Mushroom>(40, 12 + 16)->setDestroyTime(0.0f)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Decorations)->setDescriptionId("mushroom");
	goldBlock   = Tile::create<MetalTile>(41, 39 - 16)->setDestroyTime(3.0f)->setExplodeable(10)->setSoundType(SOUND_METAL)->setCategory(ItemCategory::Decorations)->setDescriptionId("blockGold");
	ironBlock   = Tile::create<MetalTile>(42, 38 - 16)->setDestroyTime(5.0f)->setExplodeable(10)->setSoundType(SOUND_METAL)->setCategory(ItemCategory::Decorations)->setDescriptionId("blockIron");
	stoneSlab   = Tile::create<StoneSlabTile>(43, true)->setDestroyTime(2.0f)->setExplodeable(10)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("stoneSlab");
	stoneSlabHalf=Tile::create<StoneSlabTile>(44, false)->setDestroyTime(2.0f)->setExplodeable(10)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("stoneSlab");
	redBrick    = Tile::create<Tile>(45, 7, Material::stone)->setDestroyTime(2.0f)->setExplodeable(10)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("brick");
	tnt         = Tile::create<TntTile>(46, 8)->setDestroyTime(0.0f)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Tools)->setDescriptionId("tnt");
	bookshelf	= Tile::create<BookshelfTile>(47, 35)->setDestroyTime(1.5f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Decorations)->setDescriptionId("bookshelf");
	mossStone	= Tile::create<Tile>(48, 36, Material::stone)->setDestroyTime(2.0f)->setExplodeable(10)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("stoneMoss");
	obsidian    = Tile::create<ObsidianTile>(49, 37, false)->setDestroyTime(10.0f)->setExplodeable(2000)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("obsidian");
	torch       = Tile::create<TorchTile>(50, 5 * 16)->setDestroyTime(0.0f)->setLightEmission(15 / 16.0f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Tools)->setDescriptionId("torch");

	stairs_wood = Tile::create<StairTile>(53, wood)->setCategory(ItemCategory::Structures)->setDescriptionId("stairsWood");
	chest		= Tile::create<ChestTile>(54)->setCategory(ItemCategory::Structures)->setDestroyTime(2.5f)->setSoundType(SOUND_WOOD)->setDescriptionId("chest");//->sendTileData();

	emeraldOre  = Tile::create<OreTile>(56, 16 * 3 + 2)->setDestroyTime(3.0f)->setExplodeable(5)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Decorations)->setDescriptionId("oreDiamond");
	emeraldBlock= Tile::create<MetalTile>(57, 40 - 16)->setDestroyTime(5.0f)->setExplodeable(10)->setSoundType(SOUND_METAL)->setCategory(ItemCategory::Decorations)->setDescriptionId("blockDiamond");
	workBench   = Tile::create<WorkbenchTile>(58)->setDestroyTime(2.5f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Structures)->setDescriptionId("workbench");
	crops		= Tile::create<CropTile>(59, 8 + 5 * 16)->setDestroyTime(0.0f)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Structures)->setDescriptionId("crops");//->sendTileData();
	farmland    = Tile::create<FarmTile>(60)->setDestroyTime(0.6f)->setSoundType(SOUND_GRAVEL)->setCategory(ItemCategory::Structures)->setDescriptionId("farmland");
	furnace		= Tile::create<FurnaceTile>(61, false)->setDestroyTime(3.5f)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("furnace");//.sendTileData();
	furnace_lit = Tile::create<FurnaceTile>(62, true)->setDestroyTime(3.5f)->setSoundType(SOUND_STONE)->setLightEmission(14 / 16.0f)->setCategory(ItemCategory::Structures)->setDescriptionId("furnace");//.sendTileData();
    sign        = Tile::create<SignTile>(63, TileEntityType::Sign, true)->setDestroyTime(1.0f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Decorations)->setDescriptionId("sign");//->sendTileData();
	door_wood   = Tile::create<DoorTile>(64, Material::wood)->setDestroyTime(3.0f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Structures)->setDescriptionId("doorWood");
	ladder      = Tile::create<LadderTile>(65, 3 + 5 * 16)->setDestroyTime(0.4f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Structures)->setDescriptionId("ladder");

	stairs_stone= Tile::create<StairTile>(67, stoneBrick)->setCategory(ItemCategory::Structures)->setDescriptionId("stairsStone");
    wallSign    = Tile::create<SignTile>(68, TileEntityType::Sign, false)->setDestroyTime(1.0f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Decorations)->setDescriptionId("sign");//->sendTileData();

	door_iron   = Tile::create<DoorTile>(71, Material::metal)->setDestroyTime(5.0f)->setSoundType(SOUND_METAL)->setCategory(ItemCategory::Structures)->setDescriptionId("doorIron");

	redStoneOre = Tile::create<RedStoneOreTile>(73, 16 * 3 + 3, false)->setDestroyTime(3.0f)->setExplodeable(5)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Mechanisms)->setDescriptionId("oreRedstone");
	redStoneOre_lit = Tile::create<RedStoneOreTile>(74, 16 * 3 + 3, true)->setDestroyTime(3.0f)->setLightEmission(10 / 16.0f)->setExplodeable(5)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Mechanisms)->setDescriptionId("oreRedstone");

	topSnow     = Tile::create<TopSnowTile>(78, 16 * 4 + 2)->setDestroyTime(0.1f)->setSoundType(SOUND_CLOTH)->setCategory(ItemCategory::Structures)->setDescriptionId("snow");
	ice         = Tile::create<IceTile>(79, 16 * 4 + 3)->setDestroyTime(0.5f)->setLightBlock(3)->setSoundType(SOUND_GLASS)->setCategory(ItemCategory::Structures)->setDescriptionId("ice");
	snow		= Tile::create<SnowTile>(80, 16 * 4 + 2)->setDestroyTime(0.2f)->setSoundType(SOUND_CLOTH)->setCategory(ItemCategory::Structures)->setDescriptionId("snow");
    cactus      = Tile::create<CactusTile>(81, 16 * 4 + 6)->setDestroyTime(0.4f)->setSoundType(SOUND_CLOTH)->setCategory(ItemCategory::Structures)->setDescriptionId("cactus");
	clay        = Tile::create<ClayTile>(82, 16 * 4 + 8)->setDestroyTime(0.6f)->setSoundType(SOUND_GRAVEL)->setCategory(ItemCategory::Structures)->setDescriptionId("clay");
	reeds       = Tile::create<ReedTile>(83, 16 * 4 + 9)->setDestroyTime(0.0f)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Structures)->setDescriptionId("reeds");

	fence		= Tile::create<FenceTile>(85, 4)->setDestroyTime(2.0f)->setExplodeable(5)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Structures)->setDescriptionId("fence");

	netherrack = Tile::create<Tile>(87, 7 + 6 * 16, Material::stone)->setDestroyTime(0.4f)->setSoundType(SOUND_STONE)->setDescriptionId("hellrock");
	//hellSand = Tile::create<HellSandTile>(88, 8 + 6 * 16, Material::sand)->setDestroyTime(0.5f)->setSoundType(SOUND_SAND)->setDescriptionId("hellsand");

	lightGem	= Tile::create<LightGemTile>(89, 9 + 16 * 6, Material::glass)->setDestroyTime(0.3f)->setCategory(ItemCategory::Structures)->setSoundType(SOUND_GLASS)->setLightEmission(1.0f)->setDescriptionId("lightgem");
	invisible_bedrock = Tile::create<InvisibleTile>(95, 0, Material::stone)->setDestroyTime(-1)->setExplodeable(6000000);//->setSoundType(SOUND_SILENT);
	trapdoor = Tile::create<TrapDoorTile>(96, Material::wood)->setDestroyTime(3.0f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Structures)->setDescriptionId("trapdoor");//->sendTileData();

	stoneBrickSmooth = Tile::create<MultiTextureTile>(98, (const int*)&STONE_BRICK_TEXTURES, STONE_BRICK_TEXTURE_COUNT, Material::stone)->setDestroyTime(1.5f)->setExplodeable(10)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("stonebricksmooth");

	thinGlass = Tile::create<ThinFenceTile>(102, 1 + 3 * 16, 4 + 9 * 16, Material::glass, false)->setDestroyTime(0.3f)->setSoundType(SOUND_GLASS)->setCategory(ItemCategory::Structures)->setDescriptionId("thinGlass");
	melon = Tile::create<MelonTile>(103)->setDestroyTime(1.0f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::FoodArmor)->setDescriptionId("melon");
	melonStem = Tile::create<StemTile>(105, Tile::melon)->setDestroyTime(0.0f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::FoodArmor)->setDescriptionId("pumpkinStem");//->sendTileData();
	fenceGate = Tile::create<FenceGateTile>(107, 4)->setDestroyTime(2.0f)->setExplodeable(5)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Structures)->setDescriptionId("fenceGate");//->sendTileData();
	stairs_brick = Tile::create<StairTile>(108, Tile::redBrick)->setCategory(ItemCategory::Structures)->setDescriptionId("stairsBrick");//->sendTileData();

	stairs_stoneBrickSmooth = Tile::create<StairTile>(109, Tile::stoneBrickSmooth)->setCategory(ItemCategory::Structures)->setDescriptionId("stairsStoneBrickSmooth");//->sendTileData();
	netherBrick    = Tile::create<Tile>(112, 0 + 14 * 16, Material::stone)->setDestroyTime(2.0f)->setExplodeable(10)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("netherBrick");
	stairs_netherBricks = Tile::create<StairTile>(114, Tile::netherBrick)->setCategory(ItemCategory::Structures)->setDescriptionId("stairsNetherBrick");//->sendTileData();
	stairs_sandStone = Tile::create<StairTile>(128, Tile::sandStone)->setCategory(ItemCategory::Structures)->setDescriptionId("stairsSandStone");//->sendTileData();

	quartzBlock   = Tile::create<QuartzBlockTile>(155)->setSoundType(SOUND_STONE)->setDestroyTime(0.8f)->setCategory(ItemCategory::Structures)->setDescriptionId("quartzBlock");
	stairs_quartz = Tile::create<StairTile>(156, Tile::quartzBlock)->setCategory(ItemCategory::Structures)->setDescriptionId("stairsQuartz");//->sendTileData();

	//
	// Special tiles for Pocket Edition is placed at high IDs
	//
	stonecutterBench= Tile::create<StonecutterTile>(245)->setDestroyTime(2.5f)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("stonecutter");
	glowingObsidian = Tile::create<ObsidianTile>(246, 10 + 16 * 13, true)->setDestroyTime(10.0f)->setLightEmission(14 / 16.0f)->setExplodeable(2000)->setSoundType(SOUND_STONE)->setCategory(ItemCategory::Structures)->setDescriptionId("glowingobsidian");
	netherReactor	= Tile::create<NetherReactor>(247, 10  + 14 * 16, Material::metal)->setDestroyTime(3.0f)->setSoundType(SOUND_METAL)->setCategory(ItemCategory::Structures)->setDescriptionId("netherreactor");
	info_updateGame1= Tile::create<Tile>(248, 252, Material::dirt)->setDestroyTime(1.0f)->setSoundType(SOUND_GRAVEL)->setCategory(ItemCategory::Structures)->setDescriptionId("info_update");
	info_updateGame2= Tile::create<Tile>(249, 253, Material::dirt)->setDestroyTime(1.0f)->setSoundType(SOUND_GRAVEL)->setCategory(ItemCategory::Structures)->setDescriptionId("info_update");
	grass_carried   = Tile::create<CarriedTile>(253, 3, 12*16 + 12)->setDescriptionId("grass");
	leaves_carried  = (LeafTile*) Tile::create<LeafTile>(254, 11 + 14 * 16)->setDestroyTime(0.2f)->setLightBlock(1)->setSoundType(SOUND_GRASS)->setCategory(ItemCategory::Structures)->setDescriptionId("leaves");
	info_reserved6  = Tile::create<Tile>(255, Material::dirt);

    //
    // Stuff that need to be inited in a specific order (i.e. after the other tiles have been created)
    //
    fire     = (FireTile*) Tile::create<FireTile>(51, 1 * 16 + 15)->setDestroyTime(0.0f)->setLightEmission(1.0f)->setSoundType(SOUND_WOOD)->setCategory(ItemCategory::Structures)->setDescriptionId("fire");

	//
    // Special case for certain items since they can have different icons
	// @note: Make sure those different items are handled in ItemInHandRenderer::renderItem
	//
	Item::items[cloth->id] = (new ClothTileItem(cloth->id - 256))->setCategory(ItemCategory::Structures)->setDescriptionId("cloth");
	Item::items[treeTrunk->id] = (new AuxDataTileItem(treeTrunk->id - 256, treeTrunk))->setCategory(ItemCategory::Structures)->setDescriptionId("log");
	Item::items[stoneBrickSmooth->id] = (new AuxDataTileItem(stoneBrickSmooth->id - 256, stoneBrickSmooth))->setCategory(ItemCategory::Structures)->setDescriptionId("stonebricksmooth");
	Item::items[stoneSlabHalf->id] = (new StoneSlabTileItem(stoneSlabHalf->id - 256))->setCategory(ItemCategory::Structures)->setDescriptionId("stoneSlab");
	Item::items[sapling->id] = (new SaplingTileItem(sapling->id - 256))->setCategory(ItemCategory::Structures)->setDescriptionId("sapling");
	Item::items[leaves->id] = (new LeafTileItem(leaves->id - 256))->setCategory(ItemCategory::Decorations)->setDescriptionId("leaves");
	Item::items[sandStone->id] = (new AuxDataTileItem(sandStone->id - 256, sandStone))->setCategory(ItemCategory::Structures)->setDescriptionId("sandStone");

	Item::items[quartzBlock->id] = (new AuxDataTileItem(quartzBlock->id - 256, quartzBlock))->setCategory(ItemCategory::Structures)->setDescriptionId("quartzBlock");

    for (int i = 0; i < 256; i++) {
        if (Tile::tiles[i] != NULL) {
			if (Item::items[i] == NULL) {
				Item::items[i] = new TileItem(i - 256);
				Item::items[i]->category = Tile::tiles[i]->category;
			}
			// Check for missing category
			if (Item::items[i]->category == -1)
				LOGE("Error: Missing category for tile %d: %s\n", tiles[i]->id, tiles[i]->getDescriptionId().c_str());
        }
    }
}

/*static*/
void Tile::teardownTiles() {
	for (int i = 0; i < 256; ++i)
		if (Tile::tiles[i]) {
			delete Tile::tiles[i];
			Tile::tiles[i] = NULL;
		}
}

int Tile::transformToValidBlockId( int blockId ) {
	return transformToValidBlockId(blockId, 0, 0, 0);
}

int Tile::transformToValidBlockId( int blockId, int x, int y, int z ) {
	if (blockId != 0 && Tile::tiles[blockId] == NULL)
		return (((x + y + z)&1) == 1)? Tile::info_updateGame1->id : Tile::info_updateGame2->id;
	return blockId;
}


Tile::Tile(int id, const Material* material)
:	id(id),
	material(material),
	tex(1),
	category(-1),
	gravity(1.0f),
	friction(0.6f),
	soundType(&Tile::SOUND_NORMAL),
	tmpBB(0,0,0,1,1,1),
	xx0(0),yy0(0),zz0(0),
	xx1(1),yy1(1),zz1(1)
{
	if (Tile::tiles[id]) {
		printf("Slot %d is already occupied by %p when adding %p\n", id, &Tile::tiles[id], this);
	}
	Tile::tiles[id] = this;
	setShape(xx0, yy0, zz0, xx1, yy1, zz1);
}

Tile::Tile( int id, int tex, const Material* material )
:	id(id),
	tex(tex),
	material(material),
	category(-1),
	gravity(1.0f),
	friction(0.6f),
	soundType(&Tile::SOUND_NORMAL),
	tmpBB(0,0,0,1,1,1),
	xx0(0),yy0(0),zz0(0),
	xx1(1),yy1(1),zz1(1)
{
	if (Tile::tiles[id]) {
		printf("Slot %d is already occupied by %p when adding %p\n", id, &Tile::tiles[id], this);
	}
	Tile::tiles[id] = this;
	setShape(xx0, yy0, zz0, xx1, yy1, zz1);
}

//Tile* sendTileData() {
//    this->properties.sendTileData = true;
//    return this;
//}

/*protected*/
Tile* Tile::setLightEmission(float f) {
    this->properties.lightEmission = (int) (Level::MAX_BRIGHTNESS * f);
    return this;
}

/*public static*/
bool Tile::isFaceVisible(Level* level, int x, int y, int z, int f) {
	switch (f) {
		case Facing::DOWN : y--; break;
		case Facing::UP   : y++; break;
		case Facing::NORTH: z--; break;
		case Facing::SOUTH: z++; break;
		case Facing::WEST : x--; break;
		case Facing::EAST : x++; break;
	}
	return !level->isSolidRenderTile(x, y, z);
}

/* private */
Tile* Tile::init() {
	this->properties.solid = isSolidRender();
	this->properties.lightBlock = isSolidRender() ? 255 : 0;
	this->properties.translucent = !material->blocksLight();
	return this;
}

/*public virtual*/
float Tile::getDestroyProgress(Player* player) {
    if (destroySpeed < 0) return 0;
    if (!player->canDestroy(this)) return 1 / destroySpeed / 100.0f;
    return (player->getDestroySpeed(this) / destroySpeed) / 30.0f;
}

/*public virtual*/
HitResult Tile::clip(Level* level, int xt, int yt, int zt, const Vec3& A, const Vec3& B) {
    updateShape(level, xt, yt, zt);

	//Stopwatch sw;
	//sw.start();

	Vec3 sub((float)xt, (float)yt, (float)zt);
	Vec3 a = A - sub;//a.add((float)-xt, (float)-yt, (float)-zt);
    Vec3 b = B - sub;//b.add((float)-xt, (float)-yt, (float)-zt);

    Vec3 xh0, xh1, yh0, yh1, zh0, zh1;

	bool bxh0 = a.clipX(b, xx0, xh0);
    bool bxh1 = a.clipX(b, xx1, xh1);

    bool byh0 = a.clipY(b, yy0, yh0);
    bool byh1 = a.clipY(b, yy1, yh1);

    bool bzh0 = a.clipZ(b, zz0, zh0);
    bool bzh1 = a.clipZ(b, zz1, zh1);

    //if (!containsX(xh0)) xh0 = NULL;
    if (!bxh0 || !containsX(xh0)) bxh0 = false;
    if (!bxh1 || !containsX(xh1)) bxh1 = false;
    if (!byh0 || !containsY(yh0)) byh0 = false;
    if (!byh1 || !containsY(yh1)) byh1 = false;
    if (!bzh0 || !containsZ(zh0)) bzh0 = false;
    if (!bzh1 || !containsZ(zh1)) bzh1 = false;
    Vec3* closest = NULL;

    //if (xh0 != NULL && (closest == NULL || a.distanceToSqr(xh0) < a.distanceToSqr(closest))) closest = xh0;
    if (bxh0 && (closest == NULL || a.distanceToSqr(xh0) < a.distanceToSqr(*closest))) closest = &xh0;
    if (bxh1 && (closest == NULL || a.distanceToSqr(xh1) < a.distanceToSqr(*closest))) closest = &xh1;
    if (byh0 && (closest == NULL || a.distanceToSqr(yh0) < a.distanceToSqr(*closest))) closest = &yh0;
    if (byh1 && (closest == NULL || a.distanceToSqr(yh1) < a.distanceToSqr(*closest))) closest = &yh1;
    if (bzh0 && (closest == NULL || a.distanceToSqr(zh0) < a.distanceToSqr(*closest))) closest = &zh0;
    if (bzh1 && (closest == NULL || a.distanceToSqr(zh1) < a.distanceToSqr(*closest))) closest = &zh1;

    if (closest == NULL)
		return HitResult();

    int face = -1;

    if (closest == &xh0) face = 4;
    if (closest == &xh1) face = 5;
    if (closest == &yh0) face = 0;
    if (closest == &yh1) face = 1;
    if (closest == &zh0) face = 2;
    if (closest == &zh1) face = 3;

	//sw.stop();
	//sw.printEvery(5, ">>> ");

    return HitResult(xt, yt, zt, face, closest->add((float)xt, (float)yt, (float)zt));
}

/*virtual*/
void Tile::spawnResources(Level* level, int x, int y, int z, int data, float odds) {
    if (level->isClientSide) return;

    int count = getResourceCount(&level->random);
    for (int i = 0; i < count; i++) {
        if (level->random.nextFloat() > odds) continue;
        int type = getResource(data, &level->random);
        if (type <= 0) continue;
        const float s = 0.7f;
        float xo = level->random.nextFloat() * s + (1 - s) * 0.5f;
        float yo = level->random.nextFloat() * s + (1 - s) * 0.5f;
        float zo = level->random.nextFloat() * s + (1 - s) * 0.5f;
		ItemEntity* item = new ItemEntity(level, x + xo, y + yo, z + zo, ItemInstance(type, 1, getSpawnResourcesAuxValue(data)));
        item->throwTime = 10;
        level->addEntity(item);
    }
}

void Tile::spawnResources( Level* level, int x, int y, int z, int data )
{
	spawnResources(level, x, y, z, data, 1);
}

void Tile::popResource(Level* level, int x, int y, int z, const ItemInstance& itemInstance) {
	if (level->isClientSide || level->getLevelData()->getGameType() == GameType::Creative) return;

	float s = 0.7f;
	float xo = level->random.nextFloat() * s + (1 - s) * 0.5f;
	float yo = level->random.nextFloat() * s + (1 - s) * 0.5f;
	float zo = level->random.nextFloat() * s + (1 - s) * 0.5f;

	ItemEntity* item = new ItemEntity(level, x + xo, y + yo, z + zo, itemInstance);
	item->throwTime = 10;
	level->addEntity(item);
}


void Tile::destroy( Level* level, int x, int y, int z, int data )
{
}

bool Tile::isCubeShaped()
{
	return true;
}

int Tile::getRenderShape()
{
	return SHAPE_BLOCK;
}

float Tile::getBrightness( LevelSource* level, int x, int y, int z )
{
	return level->getBrightness(x, y, z);
}

bool Tile::shouldRenderFace( LevelSource* level, int x, int y, int z, int face )
{
	if (face == 0 && y == -1) return false;
	// For fixed size worlds //@todo: external constants rather than magic numbers
	if (face == 2 && z == -1)  return false;
	if (face == 3 && z == 256) return false;
	if (face == 4 && x == -1)  return false;
	if (face == 5 && x == 256) return false;
	// Common
	if (face == 0 && yy0 > 0) return true;
	if (face == 1 && yy1 < 1) return true;
	if (face == 2 && zz0 > 0) return true;
	if (face == 3 && zz1 < 1) return true;
	if (face == 4 && xx0 > 0) return true;
	if (face == 5 && xx1 < 1) return true;
	Tile* t = Tile::tiles[level->getTile(x, y, z)];
	if (!t) return true;
	if (face == 1 && t->id == topSnow->id) return false;
	return !t->isSolidRender();
	//return (!level->isSolidRenderTile(x, y, z));
}

int Tile::getTexture( LevelSource* level, int x, int y, int z, int face )
{
	return getTexture(face, level->getData(x, y, z));
}

int Tile::getTexture( int face, int data )
{
	return getTexture(face);
}

int Tile::getTexture( int face )
{
	return tex;
}

void Tile::addAABBs( Level* level, int x, int y, int z, const AABB* box, std::vector<AABB>& boxes )
{
	AABB* aabb = getAABB(level, x, y, z);
	if (aabb != NULL && box->intersects(*aabb)) {
		boxes.push_back(*aabb);
	}
}

AABB* Tile::getAABB( Level* level, int x, int y, int z )
{
	tmpBB.x0 = x + xx0;
	tmpBB.y0 = y + yy0;
	tmpBB.z0 = z + zz0;
	tmpBB.x1 = x + xx1;
	tmpBB.y1 = y + yy1;
	tmpBB.z1 = z + zz1;
	return &tmpBB;
}

bool Tile::isSolidRender()
{
	return true;
}

bool Tile::mayPick( int data, bool liquid )
{
	return mayPick();
}

bool Tile::mayPick()
{
	return true;
}

int Tile::getTickDelay()
{
	return 10;
}

int Tile::getResourceCount( Random* random )
{
	return 1;
}

int Tile::getResource( int data, Random* random )
{
	return id;
}

float Tile::getExplosionResistance( Entity* source )
{
	return explosionResistance / 5.0f;
}

int Tile::getRenderLayer()
{
	return Tile::RENDERLAYER_OPAQUE;
}

bool Tile::use( Level* level, int x, int y, int z, Player* player )
{
	return false;
}

bool Tile::spawnBurnResources( Level* level, float x, float y, float z )
{
	return false;
}

int Tile::getColor( LevelSource* level, int x, int y, int z )
{
	return 0xffffff;
}

bool Tile::getSignal( LevelSource* level, int x, int y, int z )
{
	return false;
}

bool Tile::getSignal( LevelSource* level, int x, int y, int z, int dir )
{
	return false;
}

bool Tile::isSignalSource()
{
	return false;
}

bool Tile::getDirectSignal( Level* level, int x, int y, int z, int dir )
{
	return false;
}

void Tile::playerDestroy( Level* level, Player* player, int x, int y, int z, int data )
{
	//player.awardStat(Stats.blockMined[id], 1);
	spawnResources(level, x, y, z, data);
}

bool Tile::canSurvive( Level* level, int x, int y, int z )
{
	return true;
}

Tile* Tile::setDescriptionId( const std::string& id )
{
	descriptionId = TILE_DESCRIPTION_PREFIX + id;
	return this;
}

std::string Tile::getName() const
{
	return I18n::get(getDescriptionId() + ".name");
}

std::string Tile::getDescriptionId() const
{
	return descriptionId;
}

Tile* Tile::setSoundType( const SoundType& soundType )
{
	this->soundType = &soundType;
	return this;
}

Tile* Tile::setLightBlock( int i )
{
	this->properties.lightBlock = i;
	return this;
}

Tile* Tile::setExplodeable( float explosionResistance )
{
	this->explosionResistance = explosionResistance * 3;
	return this;
}

Tile* Tile::setDestroyTime( float destroySpeed )
{
	this->destroySpeed = destroySpeed;
	if (explosionResistance < destroySpeed * 5) explosionResistance = destroySpeed * 5;
	return this;
}

void Tile::setTicking( bool tick )
{
	this->properties.shouldTick = tick;
}

int Tile::getSpawnResourcesAuxValue( int data )
{
	return 0;
}

bool Tile::containsX( const Vec3& v )
{
	return v.y >= yy0 && v.y <= yy1 && v.z >= zz0 && v.z <= zz1;
}

bool Tile::containsY( const Vec3& v )
{
	return v.x >= xx0 && v.x <= xx1 && v.z >= zz0 && v.z <= zz1;
}

bool Tile::containsZ( const Vec3& v )
{
	return v.x >= xx0 && v.x <= xx1 && v.y >= yy0 && v.y <= yy1;
}

/*public*/
AABB Tile::getTileAABB(Level* level, int x, int y, int z) {
	return AABB(x + xx0, y + yy0, z + zz0, x + xx1, y + yy1, z + zz1);
}

/*public*/
void Tile::setShape(float x0, float y0, float z0, float x1, float y1, float z1) {
	this->xx0 = x0;
	this->yy0 = y0;
	this->zz0 = z0;
	this->xx1 = x1;
	this->yy1 = y1;
	this->zz1 = z1;
}

/*public*/
bool Tile::mayPlace(Level* level, int x, int y, int z, unsigned char face) {
	return mayPlace(level, x, y, z);
}

bool Tile::mayPlace( Level* level, int x, int y, int z ) {
	int t = level->getTile(x, y, z);
	return t == 0 || Tile::tiles[t]->material->isReplaceable();
}

Tile* Tile::setCategory(int category) {
	this->category = category;
	return this;
}
