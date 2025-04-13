#include "Base/ResourceManager.hpp"
#include "Base/GameInstance.h"
#include <omp.h>


void ResourceManager::ParseJson()
{
	
    std::string jsonPath = GameInstance::GetInstance()->g_WorkingDirectory + "/resources/json/example.json";
    std::ifstream file(jsonPath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open JSON file: " << jsonPath << std::endl;
        return;
    }

    json data;
    file >> data;

    // Basic multithreading, for parsing the resource.json
    #pragma omp parallel for
    for (int i = 0; i < data["Resources"].size(); i++)
    {
        auto resourceEntry = data["Resources"][i];
        TextureResource resource;
        resource.name = resourceEntry.value("Name", "");
        resource.textureID = resourceEntry.value("TextureID", 0);
        resource.textureKind = resourceEntry.value("TextureKind", "");
        resource.path = GameInstance::GetInstance()->g_WorkingDirectory + "\\resources\\" + resourceEntry.value("Path", "");

        // Parse additional properties if they exist
        if (resourceEntry.contains("Properties"))
        {
            const auto& props = resourceEntry["Properties"];
            resource.width = props.value("Width", 0);
            resource.height = props.value("Height", 0);
            resource.format = props.value("Format", "");
            resource.wrapMode = props.value("WrapMode", "");
        }

        // Check if the texture kind is NPatch and parse TextureNPatchInfo if present
        if (resource.textureKind == "NPatchTexture" && resourceEntry.contains("TextureNPatchInfo") && !resourceEntry["TextureNPatchInfo"].is_null())
        {
            const auto& npInfo = resourceEntry["TextureNPatchInfo"];
            TextureNPatchInfo info;
            if (npInfo.contains("SourceRect"))
            {
                const auto& src = npInfo["SourceRect"];
                info.sourceX = src.value("x", 0);
                info.sourceY = src.value("y", 0);
                info.sourceWidth = src.value("width", 0);
                info.sourceHeight = src.value("height", 0);
            }
            if (npInfo.contains("Padding"))
            {
                const auto& pad = npInfo["Padding"];
                info.padLeft = pad.value("left", 0);
                info.padTop = pad.value("top", 0);
                info.padRight = pad.value("right", 0);
                info.padBottom = pad.value("bottom", 0);
            }
            if (npInfo.contains("HoverOffset"))
            {
                const auto& HOF = npInfo["HoverOffset"];
                info.HoverOffsetX = HOF.value("width", 0);
                info.HoverOffsetY = HOF.value("height", 0);
            }
            info.layout = npInfo.value("Layout", "");
            resource.nPatchInfo = info;
        }

        #pragma omp critical
        AllResources.emplace(resource.name, std::move(resource));
    }

    std::cout << "finished" << std::endl;
}

Texture2DWrap TextureResource::LoadTexture()
{
    LOG_INFO(l_RESOURCES, TEXT("Requesting Load for : '{}'", name));

    if (LoadedTexture.id == 0)
    {
        LOG_INFO(l_RESOURCES, TEXT("Loading: '{}' into VRAM", name));
        LoadedTexture = RAYLIB_H::LoadTexture(path.c_str());

    }
    LOG_INFO(l_RESOURCES, TEXT("Texture '{}' already Loaded, will be reused", name));
    return Texture2DWrap(&LoadedTexture, this);
}

void TextureResource::RemoveRef()
{
    RefCount--;

    if (RefCount == 0 && WorkerDone.load())
    {
        WorkerFuture = std::async(std::launch::async, [this]()
            {
                LOG_INFO(l_RESOURCES, TEXT("Starting GC Collection for: '{}'", name));
                WorkerDone.store(false);
                auto StartTime = std::chrono::system_clock::now();
                auto CurrentTime = StartTime;
                bool bStillZero = true;
                while (StartTime + std::chrono::seconds(10) > CurrentTime)
                {
                    CurrentTime = std::chrono::system_clock::now();
                    if (RefCount != 0)
                    {
                        bStillZero = false;
                    }
                }

                if (bStillZero)
                {
                    LOG_INFO(l_RESOURCES, TEXT("Enqueing unloading for Texture: '{}' from Vram", name));
                    GameInstance::GetInstance()->MainQueue.Enqueue([this]()
                        {
                            this->UnloadTexture();
                        });

                    WorkerDone.store(true);
                    return;
                }

                LOG_INFO(l_RESOURCES, TEXT("Aborting Cleaning '{}' as still used", name));
                WorkerDone.store(true);
                return;
            });
    }
}

bool TextureResource::UnloadTexture()
{
    if (LoadedTexture.id != 0)
    {
        RAYLIB_H::UnloadTexture(LoadedTexture);
        LoadedTexture = {};
        LOG_INFO(l_RESOURCES, TEXT("'{}' was Cleaned from Vram", name));
        return true;
    }

    return false;
}
