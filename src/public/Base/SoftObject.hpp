#pragma once
#include "Base/GameInstance.h"
#include "Base/AssetRegistry.hpp"


/**
* SoftObjectPath provides a way to reference objects by name rather than direct pointer,
* allowing for lazy loading, improved serialization, and better memory management.
*/
template <typename T>
struct SoftObjectPath
{
    SoftObjectPath() = default;


    // Implicit upcast constructor (from derived to base)
    template <typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    SoftObjectPath(const SoftObjectPath<U>& other) 
        : m_Path(other.ToString()) {}

    // Explicit cast method for related types
    template <typename U>
    SoftObjectPath<U> Cast() const
    {
        static_assert(std::is_base_of_v<IObject, U>, "U must derive from IObject");
        static_assert(std::is_base_of_v<U, T> || std::is_base_of_v<T, U>,
            "Types must be related by inheritance");
        return SoftObjectPath<U>(m_Path);
    }

    SoftObjectPath(std::string SoftPath)
        :m_Path(SoftPath)
    {
    }

    std::shared_ptr<T> TryLoad()
    {
        return std::dynamic_pointer_cast<T>(GameInstance::GetAssetRegistry()->LoadAssetFromSoftObjectPath(*this));
    }

    std::string ToString() const
    {
        return m_Path;
    }

protected:
    std::string m_Path;
};
