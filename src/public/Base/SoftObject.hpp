#pragma once
#include <functional>
#include <memory>
#include <string>
#include <type_traits>

// Forward Declarations - KEIN #include "GameInstance.h"!
class IObject;
class AssetRegistry;

// Globaler Zugriff auf AssetRegistry (wird in GameInstance.cpp definiert)
std::shared_ptr<AssetRegistry> GetGlobalAssetRegistry();

template <typename T>
struct SoftObjectPath
{
    SoftObjectPath() = default;

    template <typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    SoftObjectPath(const SoftObjectPath<U>& other)
        : m_Path(other.ToString()) {
    }

    template <typename U>
    SoftObjectPath<U> Cast() const
    {
        static_assert(std::is_base_of_v<IObject, U>, "U must derive from IObject");
        static_assert(std::is_base_of_v<U, T> || std::is_base_of_v<T, U>,
            "Types must be related by inheritance");
        return SoftObjectPath<U>(m_Path);
    }

    SoftObjectPath(std::string SoftPath)
        : m_Path(SoftPath)
    {
    }

    // Implementierung hier, aber mit freier Funktion statt GameInstance
    bool IsValid() const;
    std::shared_ptr<T> TryLoad() const;

    std::string ToString() const { return m_Path; }

    bool operator==(const SoftObjectPath<T>& other) const { return m_Path == other.m_Path; }
    bool operator!=(const SoftObjectPath<T>& other) const { return !(*this == other); }
    operator bool() const { return IsValid(); }

protected:
    std::string m_Path;
};

namespace std
{
    template <typename T>
    struct hash<SoftObjectPath<T>>
    {
        size_t operator()(const SoftObjectPath<T>& path) const
        {
            return std::hash<std::string>{}(path.ToString());
        }
    };
}

// Template-Implementierungen nach der Klassendefinition
#include "Base/AssetRegistry.hpp"

template <typename T>
bool SoftObjectPath<T>::IsValid() const
{
    auto Registry = GetGlobalAssetRegistry();
    return Registry ? Registry->LoadAssetFromSoftObjectPath(*this) != nullptr : false;
}

template <typename T>
std::shared_ptr<T> SoftObjectPath<T>::TryLoad() const
{
    auto Registry = GetGlobalAssetRegistry();
    if (!Registry) return nullptr;
    return std::dynamic_pointer_cast<T>(Registry->LoadAssetFromSoftObjectPath(*this));
}