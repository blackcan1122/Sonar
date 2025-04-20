#pragma once
// CoreInline.h
#pragma once

#include "Core.h"
#include "GameInstance.h" // Full definition of GameInstance

// Implement TryLoad() here
template <typename T>
inline std::shared_ptr<T> SoftObjectPath<T>::TryLoad() 
{
    return GameInstance::GetInstance()->LoadAssetFromSoftObjectPath(*this);
}
