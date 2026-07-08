#ifndef UTIL__Registry_H__
#define UTIL__Registry_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

template <typename T>
class Registry
{
public:
    using Factory = std::function<T*(int id)>;

    int registerById(int id, const std::string& name, Factory factory)
    {
        if (id < 0) return -1;
        if (id >= (int)mObjectsById.size())
            mObjectsById.resize(id + 1, nullptr);
        if (mObjectsById[id] != nullptr)
            return -1;
        T* obj = factory(id);
        mObjectsById[id] = obj;
        mIdToName[id] = name;
        mNameToId[name] = id;
        return id;
    }

    int registerNamed(const std::string& name, Factory factory)
    {
        auto it = mNameToId.find(name);
        if (it != mNameToId.end())
            return it->second;
        int id = mNextId++;
        while (id < (int)mObjectsById.size() && mObjectsById[id] != nullptr)
            id++;
        mNextId = id + 1;
        return registerById(id, name, factory);
    }

    T* getById(int id) const
    {
        if (id < 0 || id >= (int)mObjectsById.size())
            return nullptr;
        return mObjectsById[id];
    }

    T* getByName(const std::string& name) const
    {
        auto it = mNameToId.find(name);
        if (it == mNameToId.end())
            return nullptr;
        return getById(it->second);
    }

    const std::string& getNameById(int id) const
    {
        static const std::string empty;
        auto it = mIdToName.find(id);
        return it != mIdToName.end() ? it->second : empty;
    }

    int getIdByName(const std::string& name) const
    {
        auto it = mNameToId.find(name);
        return it != mNameToId.end() ? it->second : -1;
    }

    int getId(const T* obj) const
    {
        for (int i = 0; i < (int)mObjectsById.size(); i++)
            if (mObjectsById[i] == obj)
                return i;
        return -1;
    }

    bool contains(int id) const
    {
        return id >= 0 && id < (int)mObjectsById.size() && mObjectsById[id] != nullptr;
    }

    void forEach(std::function<void(T*)> callback) const
    {
        for (T* obj : mObjectsById)
            if (obj)
                callback(obj);
    }

    int size() const { return (int)mIdToName.size(); }

    void clear()
    {
        for (T* obj : mObjectsById)
            delete obj;
        mObjectsById.clear();
        mIdToName.clear();
        mNameToId.clear();
        mNextId = 0;
    }

private:
    std::vector<T*> mObjectsById;
    std::unordered_map<int, std::string> mIdToName;
    std::unordered_map<std::string, int> mNameToId;
    int mNextId = 1; // start at 1 (0 = null/air)
};

#endif
