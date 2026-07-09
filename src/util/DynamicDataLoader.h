#ifndef DYNAMIC_DATA_LOADER_H__
#define DYNAMIC_DATA_LOADER_H__

#include <string>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class DynamicDataLoader {
public:
    using Handler = std::function<void(const std::string& modId, const json& data)>;

    void addHandler(const std::string& type, Handler handler);
    void dispatch(const std::string& modId, const json& data);

    void initialize();

    static DynamicDataLoader& singleton();

private:
    DynamicDataLoader() {}
    std::unordered_map<std::string, Handler> m_handlers;
};

#endif
