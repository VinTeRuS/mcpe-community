#ifndef JSONLOADER_H__
#define JSONLOADER_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JsonLoader {
public:
    using JsonCallback = std::function<void(const std::string& path, const json& data)>;

    JsonLoader(const std::string& basePath = "data");

    json loadFile(const std::string& ns, const std::string& path);

    void loadDir(const std::string& ns, const std::string& dir,
                 const JsonCallback& cb = nullptr);

    void loadAll(const std::string& ns, const JsonCallback& cb = nullptr);

    template<typename T>
    void loadDirInto(const std::string& ns, const std::string& dir,
                     std::unordered_map<std::string, T>& map,
                     std::function<T(const json&)> parser);

    void addOverlayPath(const std::string& ns, const std::string& path);

    static JsonLoader& singleton();

private:
    std::string m_basePath;
    std::unordered_map<std::string, std::vector<std::string>> m_overlayPaths;
};

#endif
