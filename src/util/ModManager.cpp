#include "ModManager.h"
#include "DynamicDataLoader.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cstdio>

namespace fs = std::filesystem;

void ModManager::scanMods() {
    m_mods.clear();
    m_idIndex.clear();

    // Core mod: data/minecraft/
    {
        ModInfo core;
        core.id = "minecraft";
        core.name = "Minecraft";
        core.description = "Base game content";
        core.core = true;
        core.path = "data/minecraft";
        m_idIndex[core.id] = (int)m_mods.size();
        m_mods.push_back(core);
    }

    // User mods: mods/*/modinfo.json
    const char* modDirs[] = {"mods", nullptr};
    for (const char** dir = modDirs; *dir; dir++) {
        if (!fs::exists(*dir) || !fs::is_directory(*dir))
            continue;
        for (auto& entry : fs::directory_iterator(*dir)) {
            if (!entry.is_directory())
                continue;
            fs::path modinfoPath = entry.path() / "modinfo.json";
            if (!fs::exists(modinfoPath))
                continue;

            std::ifstream f(modinfoPath);
            if (!f.is_open())
                continue;

            try {
                json data = json::parse(f);
                // Support both object and array
                if (data.is_array()) {
                    for (auto& item : data) {
                        if (item.is_object() && item.value("type", "") == "mod_info") {
                            data = item;
                            break;
                        }
                    }
                }
                if (!data.is_object() || data.value("type", "") != "mod_info")
                    continue;

                ModInfo mod;
                mod.id = data.value("id", "");
                mod.name = data.value("name", mod.id);
                mod.description = data.value("description", "");
                mod.version = data.value("version", "");
                mod.core = false;
                mod.path = entry.path().string();

                if (mod.id.empty()) {
                    fprintf(stderr, "ModManager: mod at %s has no id\n", modinfoPath.c_str());
                    continue;
                }

                if (data.contains("dependencies")) {
                    for (auto& dep : data["dependencies"])
                        mod.dependencies.push_back(dep.get<std::string>());
                }

                if (m_idIndex.count(mod.id)) {
                    fprintf(stderr, "ModManager: duplicate mod id '%s' at %s\n",
                            mod.id.c_str(), modinfoPath.c_str());
                    continue;
                }

                m_idIndex[mod.id] = (int)m_mods.size();
                m_mods.push_back(mod);
                printf("ModManager: found mod '%s' (%s) at %s\n",
                       mod.id.c_str(), mod.name.c_str(), mod.path.c_str());

            } catch (const std::exception& e) {
                fprintf(stderr, "ModManager: error reading %s: %s\n",
                        modinfoPath.c_str(), e.what());
            }
        }
    }
}

void ModManager::resolveDependencies() {
    m_ordered.clear();
    std::vector<bool> visited(m_mods.size(), false);
    std::vector<bool> inStack(m_mods.size(), false);

    std::function<void(int)> visit = [&](int idx) {
        if (visited[idx]) return;
        if (inStack[idx]) {
            fprintf(stderr, "ModManager: dependency cycle detected involving '%s'\n",
                    m_mods[idx].id.c_str());
            return;
        }
        inStack[idx] = true;
        for (auto& dep : m_mods[idx].dependencies) {
            auto it = m_idIndex.find(dep);
            if (it != m_idIndex.end())
                visit(it->second);
        }
        inStack[idx] = false;
        visited[idx] = true;
        m_ordered.push_back(m_mods[idx]);
    };

    for (int i = 0; i < (int)m_mods.size(); i++)
        visit(i);
}

void ModManager::loadMods() {
    resolveDependencies();
    printf("ModManager: loading %d mods in dependency order...\n", (int)m_ordered.size());
    for (auto& mod : m_ordered) {
        printf("ModManager: loading mod '%s' from %s\n", mod.id.c_str(), mod.path.c_str());
        loadMod(mod);
    }
    printf("ModManager: all mods loaded\n");
}

void ModManager::loadMod(const ModInfo& mod) {
    auto& ddl = DynamicDataLoader::singleton();
    int fileCount = 0;

    if (!fs::exists(mod.path) || !fs::is_directory(mod.path)) {
        fprintf(stderr, "ModManager: mod path '%s' not found\n", mod.path.c_str());
        return;
    }

    // Walk all subdirectories, load .json files
    for (auto& subdir : fs::directory_iterator(mod.path)) {
        if (!subdir.is_directory())
            continue;

        // Skip hidden dirs
        std::string dirName = subdir.path().filename().string();
        if (dirName.empty() || dirName[0] == '.')
            continue;

        for (auto& file : fs::directory_iterator(subdir.path())) {
            if (!file.is_regular_file())
                continue;
            auto path = file.path();
            if (path.extension() != ".json")
                continue;

            std::ifstream f(path);
            if (!f.is_open())
                continue;

            try {
                json data = json::parse(f);
                ddl.dispatch(mod.id, data);
                fileCount++;
            } catch (const std::exception& e) {
                fprintf(stderr, "ModManager: error parsing %s: %s\n",
                        path.c_str(), e.what());
            }
        }
    }

    printf("ModManager: loaded %d files from mod '%s'\n", fileCount, mod.id.c_str());
}

void ModManager::report() const {
    printf("ModManager: %d mods loaded:\n", (int)m_ordered.size());
    for (auto& mod : m_ordered) {
        printf("  [%s] %s", mod.id.c_str(), mod.name.c_str());
        if (mod.core) printf(" (core)");
        for (auto& dep : mod.dependencies)
            printf(" -> %s", dep.c_str());
        printf("\n");
    }
}

ModManager& ModManager::singleton() {
    static ModManager instance;
    return instance;
}
