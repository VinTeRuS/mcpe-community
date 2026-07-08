#include "JsonLoader.h"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cstring>

namespace fs = std::filesystem;

JsonLoader::JsonLoader(const std::string& basePath)
    : m_basePath(basePath) {
}

json JsonLoader::loadFile(const std::string& ns, const std::string& path) {
    std::string fullPath = m_basePath + "/" + ns + "/" + path;

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        fprintf(stderr, "JsonLoader: cannot open %s\n", fullPath.c_str());
        return json();
    }

    try {
        return json::parse(file);
    } catch (const json::parse_error& e) {
        fprintf(stderr, "JsonLoader: parse error in %s: %s\n",
                fullPath.c_str(), e.what());
        return json();
    }
}

void JsonLoader::loadDir(const std::string& ns, const std::string& dir,
                          const JsonCallback& cb) {
    std::string dirPath = m_basePath + "/" + ns + "/" + dir;

    if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
        return;
    }

    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (!entry.is_regular_file()) continue;
        auto pathObj = entry.path();
        if (pathObj.extension() != ".json") continue;

        std::string relPath = dir + "/" + pathObj.filename().string();
        json data = loadFile(ns, relPath);
        if (!data.is_null() && cb) {
            cb(relPath, data);
        }
    }

    auto it = m_overlayPaths.find(ns);
    if (it != m_overlayPaths.end()) {
        for (const auto& overlay : it->second) {
            std::string overlayDir = m_basePath + "/" + overlay + "/" + dir;
            if (!fs::exists(overlayDir) || !fs::is_directory(overlayDir))
                continue;
            for (const auto& entry : fs::directory_iterator(overlayDir)) {
                if (!entry.is_regular_file()) continue;
                auto pathObj = entry.path();
                if (pathObj.extension() != ".json") continue;

                std::string filename = pathObj.filename().string();
                std::string relPath = dir + "/" + filename;
                json data = loadFile(overlay, relPath);
                if (!data.is_null() && cb) {
                    cb(relPath, data);
                }
            }
        }
    }
}

void JsonLoader::loadAll(const std::string& ns, const JsonCallback& cb) {
    std::string nsPath = m_basePath + "/" + ns;
    if (!fs::exists(nsPath) || !fs::is_directory(nsPath)) {
        fprintf(stderr, "JsonLoader: namespace %s not found at %s\n",
                ns.c_str(), nsPath.c_str());
        return;
    }

    for (const auto& entry : fs::directory_iterator(nsPath)) {
        if (!entry.is_directory()) continue;
        std::string dirname = entry.path().filename().string();
        loadDir(ns, dirname, cb);
    }

    auto it = m_overlayPaths.find(ns);
    if (it != m_overlayPaths.end()) {
        for (const auto& overlay : it->second) {
            std::string overlayPath = m_basePath + "/" + overlay;
            if (!fs::exists(overlayPath) || !fs::is_directory(overlayPath))
                continue;
            for (const auto& entry : fs::directory_iterator(overlayPath)) {
                if (!entry.is_directory()) continue;
                std::string dirname = entry.path().filename().string();
                loadDir(overlay, dirname, cb);
            }
        }
    }
}

void JsonLoader::addOverlayPath(const std::string& ns, const std::string& path) {
    m_overlayPaths[ns].push_back(path);
}

JsonLoader& JsonLoader::singleton() {
    static JsonLoader instance;
    return instance;
}
