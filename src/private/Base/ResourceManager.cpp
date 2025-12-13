#include "Base/ResourceManager.hpp"
#include "Base/GameInstance.h"
#include <omp.h>


void ResourceManager::ParseJson()
{
    
    std::string jsonPath = GameInstance::GetInstance()->g_WorkingDirectory + "/resources/json/Textures.json";
    
    LOG_INFO(l_RESOURCES, TEXT("Starting resource parsing from: '{}'", jsonPath));
    
    std::ifstream file(jsonPath);
    if (!file.is_open())
    {
        LOG_ERROR(l_RESOURCES, TEXT("Failed to open JSON file: '{}'", jsonPath));
        std::cerr << "Failed to open JSON file: " << jsonPath << std::endl;
        return;
    }

    json data;
    try
    {
        file >> data;
    }
    catch (const json::exception& e)
    {
        LOG_ERROR(l_RESOURCES, TEXT("JSON parsing error: {}", e.what()));
        return;
    }

    if (!data.contains("Resources") || !data["Resources"].is_array())
    {
        LOG_ERROR(l_RESOURCES, TEXT("Invalid JSON structure: 'Resources' array not found"));
        return;
    }

    int totalResources = data["Resources"].size();
    LOG_INFO(l_RESOURCES, TEXT("Found {} resource(s) to parse", totalResources));

#if DEBUG
    LOG_INFO(l_RESOURCES, TEXT("Starting multi-threaded parsing with OpenMP"));
    auto parseStartTime = std::chrono::high_resolution_clock::now();
#endif

    // Basic multithreading, for parsing the resource.json
    #pragma omp parallel for
    for (int i = 0; i < totalResources; i++)
    {
        auto resourceEntry = data["Resources"][i];
        TextureResource resource;
        resource.name = resourceEntry.value("Name", "");
        resource.textureID = resourceEntry.value("TextureID", 0);
        resource.textureKind = resourceEntry.value("TextureKind", "");
        resource.path = GameInstance::GetInstance()->g_WorkingDirectory + "/resources/" + resourceEntry.value("Path", "");

#if DEBUG
        LOG_INFO(l_RESOURCES, TEXT("[Thread {}] Parsing resource '{}' (ID: {}, Kind: {})", 
            omp_get_thread_num(), resource.name, resource.textureID, resource.textureKind));
#endif

        // Parse additional properties if they exist
        if (resourceEntry.contains("Properties"))
        {
            const auto& props = resourceEntry["Properties"];
            resource.width = props.value("Width", 0);
            resource.height = props.value("Height", 0);
            resource.format = props.value("Format", "");
            resource.wrapMode = props.value("WrapMode", "");

#if DEBUG
            LOG_INFO(l_RESOURCES, TEXT("[Thread {}] Properties for '{}': {}x{}, Format: {}, WrapMode: {}", 
                omp_get_thread_num(), resource.name, resource.width, resource.height, 
                resource.format, resource.wrapMode));
#endif
        }

        // Check if the texture kind is NPatch and parse TextureNPatchInfo if present
        if (resource.textureKind == "NPatchTexture" && resourceEntry.contains("TextureNPatchInfo") && !resourceEntry["TextureNPatchInfo"].is_null())
        {
#if DEBUG
            LOG_INFO(l_RESOURCES, TEXT("[Thread {}] Parsing NPatch info for '{}'", 
                omp_get_thread_num(), resource.name));
#endif

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

#if DEBUG
            LOG_INFO(l_RESOURCES, TEXT("[Thread {}] NPatch for '{}': SourceRect({},{},{}x{}), Padding(L:{},T:{},R:{},B:{}), Layout: {}", 
                omp_get_thread_num(), resource.name, info.sourceX, info.sourceY, 
                info.sourceWidth, info.sourceHeight, info.padLeft, info.padTop, 
                info.padRight, info.padBottom, info.layout));
#endif
        }
        
        #pragma omp critical
        {
#if DEBUG
            LOG_INFO(l_RESOURCES, TEXT("[Thread {}] Emplacing resource '{}' into AllResources map", 
                omp_get_thread_num(), resource.name));
#endif
            AllResources.emplace(resource.name, std::move(resource));
        }
    }

#if DEBUG
    auto parseEndTime = std::chrono::high_resolution_clock::now();
    auto parseDuration = std::chrono::duration_cast<std::chrono::milliseconds>(parseEndTime - parseStartTime);
    LOG_INFO(l_RESOURCES, TEXT("Resource parsing completed in {} ms", parseDuration.count()));
#endif

    LOG_INFO(l_RESOURCES, TEXT("Successfully parsed and loaded {} resource(s)", AllResources.size()));
}

void ResourceManager::CleanAllResources()
{
    for (auto& [_name, _texture] : this->AllResources)
    {
        _texture.ForceCleanup();
    }
}

SharedTexture2D TextureResource::LoadTexture()
{
    LOG_INFO(l_RESOURCES, TEXT("Requesting Load for : '{}'", name));

    if (LoadedTexture.id == 0)
    {
        LOG_INFO(l_RESOURCES, TEXT("Loading: '{}' into VRAM", name));
        LoadedTexture = RAYLIB_H::LoadTexture(path.c_str());

        LoadedTexture.height = height;
        LoadedTexture.width = width;

    }
    else
    {
        LOG_INFO(l_RESOURCES, TEXT("Texture '{}' already Loaded, will be reused", name));
    }
  
    return SharedTexture2D(&LoadedTexture, this);
}

void TextureResource::SetHeight(int Height)
{
    if (LoadedTexture.id != 0)
    {
        LoadedTexture.height = Height;
    }
    height = Height;
}

void TextureResource::SetWidth(int Width)
{
    if (LoadedTexture.id != 0)
    {
        LoadedTexture.width = width;
    }
    width = Width;
}

void TextureResource::GenerateMipMaps()
{
    if (LoadedTexture.id != 0)
    {
        GenTextureMipmaps(&LoadedTexture);
    }
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
                bool StillZero = true;
                size_t ResetCounter = 0;
                while (StartTime + std::chrono::seconds(120) > CurrentTime && !ShutdownRequested.load())
                {
                    // Sleep in smaller chunks to check ShutdownRequested more frequently
                    for (int i = 0; i < 50 && !ShutdownRequested.load(); ++i) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                    
                    CurrentTime = std::chrono::system_clock::now();
                    if (RefCount != 0)
                    {
                        if (ResetCounter < 5)
                        {
                            LOG_INFO(l_RESOURCES, TEXT("Ref Count Raised, Resetting Timer for GC for the {}", ResetCounter));
                            ResetCounter++;
                            StartTime = std::chrono::system_clock::now();
                        }
                        else
                        {
                            LOG_INFO(l_RESOURCES, TEXT("Reset Counter exceed Max Reset Times, GC will be postponed"));
                            StillZero = false;
                            break;
                        }
                    }
                }

                if (StillZero)
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

void TextureResource::ForceCleanup()
{
    ShutdownRequested.store(true);

    // If a GC worker is running, wait for it to finish
    if (WorkerFuture.valid()) {
        WorkerFuture.wait();
    }

    // Unload directly instead of enqueueing
    // Since we're being destroyed anyway
    UnloadTexture();
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
