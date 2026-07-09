#ifndef MOD_MANAGER_H__
#define MOD_MANAGER_H__

#include <string>
#include <vector>
#include <unordered_map>

struct ModInfo {
    std::string id;
    std::string name;
    std::string description;
    std::string version;
    std::vector<std::string> dependencies;
    std::string path;
    bool core = false;
};

class ModManager {
public:
    void scanMods();
    void loadMods();
    void report() const;

    static ModManager& singleton();

private:
    void loadMod(const ModInfo& mod);
    void resolveDependencies();

    std::vector<ModInfo> m_mods;
    std::vector<ModInfo> m_ordered;  // after dependency resolution
    std::unordered_map<std::string, int> m_idIndex;
};

#endif
