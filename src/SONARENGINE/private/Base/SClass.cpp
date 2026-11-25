#include "Base/SClass.hpp"
#include <mutex>
std::unordered_map<std::string, SClass*>& SClass::GetClassRegistry() {
    static std::unordered_map<std::string, SClass*> registry;
    return registry;
}
SClass* SClass::GetOrCreate(const SClass* parent, const char* name) {
    static std::recursive_mutex mutex;
    std::lock_guard lock(mutex);
    if (auto found = FindClass(name)) return found;
    static std::vector<std::unique_ptr<SClass>> owned;
    owned.push_back(std::make_unique<SClass>(parent, name));
    return owned.back().get();
}
