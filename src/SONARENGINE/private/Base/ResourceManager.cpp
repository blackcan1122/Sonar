#include "Base/ResourceManager.hpp"
#include <omp.h>


std::string ResourceManager::GetManifestPath() const
{
    if (!m_ManifestPath.empty())
    {
        return m_ManifestPath;
    }

    return m_ResourceRoot + "/json/Textures.json";
}

std::string ResourceManager::ResolveResourcePath(const std::string& RelativePath) const
{
    if (RelativePath.empty())
    {
        return {};
    }

    const std::filesystem::path ResourcePath(RelativePath);
    if (ResourcePath.is_absolute())
    {
        return ResourcePath.lexically_normal().generic_string();
    }

    return (std::filesystem::path(m_ResourceRoot) / ResourcePath).lexically_normal().generic_string();
}

std::string ResourceManager::ResolveShaderPath(const std::string& ShaderFileName) const
{
    if (ShaderFileName.empty())
    {
        return {};
    }

    const std::filesystem::path ShaderPath(ShaderFileName);
    if (ShaderPath.is_absolute() && std::filesystem::exists(ShaderPath))
    {
        return ShaderPath.generic_string();
    }

    const std::filesystem::path WorkingDirectory = m_WorkingDirectory.empty()
        ? std::filesystem::current_path()
        : std::filesystem::path(m_WorkingDirectory);

    const std::array<std::filesystem::path, 4> Candidates = {
        WorkingDirectory / "src" / "SONAR" / "shaders" / ShaderFileName,
        WorkingDirectory / "bin" / "src" / "shaders" / ShaderFileName,
        std::filesystem::current_path() / "src" / "SONAR" / "shaders" / ShaderFileName,
        std::filesystem::current_path() / "src" / "shaders" / ShaderFileName,
    };

    for (const auto& Candidate : Candidates)
    {
        if (std::filesystem::exists(Candidate))
        {
            return Candidate.generic_string();
        }
    }

    return {};
}

void ResourceManager::EnqueueMainThreadTask(std::function<void()> Task)
{
    if (m_MainQueue != nullptr)
    {
        m_MainQueue->Enqueue(std::move(Task));
        return;
    }

    Task();
}


void ResourceManager::ParseJson()
{
    
    std::string jsonPath = GetManifestPath();
    
    LOG_INFO(l_RESOURCES, SONAR_FMT("Starting resource parsing from: '{}'", jsonPath));
    
    std::ifstream file(jsonPath);
    if (!file.is_open())
    {
        LOG_ERROR(l_RESOURCES, SONAR_FMT("Failed to open JSON file: '{}'", jsonPath));
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
        LOG_ERROR(l_RESOURCES, SONAR_FMT("JSON parsing error: {}", e.what()));
        return;
    }

    if (!data.contains("Resources") || !data["Resources"].is_array())
    {
        LOG_ERROR(l_RESOURCES, SONAR_FMT("Invalid JSON structure: 'Resources' array not found"));
        return;
    }

    int totalResources = static_cast<int>(data["Resources"].size());
    LOG_INFO(l_RESOURCES, SONAR_FMT("Found {} resource(s) to parse", totalResources));

#if DEBUG
    LOG_INFO(l_RESOURCES, SONAR_FMT("Starting multi-threaded parsing with OpenMP"));
    auto parseStartTime = std::chrono::high_resolution_clock::now();
#endif

    // Basic multithreading, for parsing the resource.json
    #pragma omp parallel for
    for (int i = 0; i < totalResources; i++)
    {
        auto resourceEntry = data["Resources"][i];
        TextureResource resource;
        resource.Owner = this;
        resource.name = resourceEntry.value("Name", "");
        resource.textureID = resourceEntry.value("TextureID", 0);
        resource.textureKind = resourceEntry.value("TextureKind", "");
        resource.path = m_ResourceRoot + "/" + resourceEntry.value("Path", "");

#if DEBUG
        LOG_INFO(l_RESOURCES, SONAR_FMT("[Thread {}] Parsing resource '{}' (ID: {}, Kind: {})", 
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
            LOG_INFO(l_RESOURCES, SONAR_FMT("[Thread {}] Properties for '{}': {}x{}, Format: {}, WrapMode: {}", 
                omp_get_thread_num(), resource.name, resource.width, resource.height, 
                resource.format, resource.wrapMode));
#endif
        }

        // Check if the texture kind is NPatch and parse TextureNPatchInfo if present
        if (resource.textureKind == "NPatchTexture" && resourceEntry.contains("TextureNPatchInfo") && !resourceEntry["TextureNPatchInfo"].is_null())
        {
#if DEBUG
            LOG_INFO(l_RESOURCES, SONAR_FMT("[Thread {}] Parsing NPatch info for '{}'", 
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
            LOG_INFO(l_RESOURCES, SONAR_FMT("[Thread {}] NPatch for '{}': SourceRect({},{},{}x{}), Padding(L:{},T:{},R:{},B:{}), Layout: {}", 
                omp_get_thread_num(), resource.name, info.sourceX, info.sourceY, 
                info.sourceWidth, info.sourceHeight, info.padLeft, info.padTop, 
                info.padRight, info.padBottom, info.layout));
#endif
        }
        
        #pragma omp critical
        {
#if DEBUG
            LOG_INFO(l_RESOURCES, SONAR_FMT("[Thread {}] Emplacing resource '{}' into AllResources map", 
                omp_get_thread_num(), resource.name));
#endif
            AllResources.emplace(resource.name, std::move(resource));
        }
    }

#if DEBUG
    auto parseEndTime = std::chrono::high_resolution_clock::now();
    auto parseDuration = std::chrono::duration_cast<std::chrono::milliseconds>(parseEndTime - parseStartTime);
    LOG_INFO(l_RESOURCES, SONAR_FMT("Resource parsing completed in {} ms", parseDuration.count()));
#endif

    LOG_INFO(l_RESOURCES, SONAR_FMT("Successfully parsed and loaded {} resource(s)", AllResources.size()));
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
    LOG_INFO(l_RESOURCES, SONAR_FMT("Requesting Load for : '{}'", name));

    if (LoadedTexture.id == 0)
    {
        if (!std::filesystem::exists(path))
        {
            LOG_WARN(l_RESOURCES, SONAR_FMT("Texture '{}' could not be loaded because the file is missing: '{}'", name, path));
            return SharedTexture2D();
        }

        LOG_INFO(l_RESOURCES, SONAR_FMT("Loading: '{}' into VRAM", name));
        LoadedTexture = RAYLIB_H::LoadTexture(path.c_str());

        if (LoadedTexture.id == 0)
        {
            LOG_WARN(l_RESOURCES, SONAR_FMT("Texture '{}' failed to load from '{}'", name, path));
            LoadedTexture = {};
            return SharedTexture2D();
        }

        LoadedTexture.height = height;
        LoadedTexture.width = width;

    }
    else
    {
        LOG_INFO(l_RESOURCES, SONAR_FMT("Texture '{}' already Loaded, will be reused", name));
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
        LoadedTexture.width = Width;
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
    RefCount.fetch_sub(1, std::memory_order_relaxed);

    if (RefCount.load(std::memory_order_relaxed) == 0 && WorkerDone.load())
    {
        WorkerFuture = std::async(std::launch::async, [this]()
        {
                LOG_INFO(l_RESOURCES, SONAR_FMT("Starting GC Collection for: '{}'", name));
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
                    if (RefCount.load(std::memory_order_relaxed) != 0)
                    {
                        if (ResetCounter < 5)
                        {
                            LOG_INFO(l_RESOURCES, SONAR_FMT("Ref Count Raised for Texture '{}', Resetting Timer for GC for the {}", name, ResetCounter));
                            ResetCounter++;
                            StartTime = std::chrono::system_clock::now();
                        }
                        else
                        {
                            LOG_INFO(l_RESOURCES, SONAR_FMT("Reset Counter exceed Max Reset Times for Texture '{}', GC will be postponed", name));
                            StillZero = false;
                            break;
                        }
                    }
                }

                if (StillZero)
                {
                    LOG_INFO(l_RESOURCES, SONAR_FMT("Enqueing unloading for Texture: '{}' from Vram", name));
                    Owner->EnqueueMainThreadTask([this]()
                        {
                            this->UnloadTexture();
                        });

                    WorkerDone.store(true);
                    return;
                }

                LOG_INFO(l_RESOURCES, SONAR_FMT("Aborting Cleaning '{}' as still used", name));
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
        LOG_INFO(l_RESOURCES, SONAR_FMT("'{}' was Cleaned from Vram", name));
        return true;
    }

    return false;
}
