#pragma once
#include "Base/Export.hpp"
#include "Base/Core.h"
#include <fstream>
#include <atomic>
#include <thread>
#include <future>
#include <optional>
#include "Base/Texture2DWrap.hpp"
#include "Base/EventDispatcher.hpp"
#include "Events/AllPurposeEvent.h"
#include "Events/StringEventData.hpp"
#include "Base/GameThreadQueue.hpp"

class ResourceManager;

/**
 * @class SONARENGINE_API ResourceManager
 * @brief Manages the loading, parsing, and lifecycle of game resources.
 *
 * Purpose:
 * - Provides a centralized manager for all game resources, such as textures.
 * - Parses resource data from JSON and tracks resource metadata.
 *
 * Usage:
 * - Call `ParseJson()` to load resources from a predefined JSON file.
 * - Access resources through GameInstance::GetInstance()->GetResource(std::string Name);
 *
 * Notes:
 * - Ensure JSON files are correctly formatted according to the expected schema.
 * - Resources are managed using `TextureResource` for metadata and loading/unloading logic.
 */

 /**
  * @struct TextureResource
  * @brief Represents individual texture metadata and handles texture loading.
  *
  * Purpose:
  * - Encapsulates metadata about textures (e.g., dimensions, format).
  * - Provides mechanisms for loading and unloading textures safely.
  *
  * Usage:
  * - Use `LoadTexture()` to load the texture into memory.
  * - Call `UnloadTexture()` to release the texture when it is no longer needed.
  *
  * Notes:
  * - Textures are reference-counted; ensure proper usage to avoid resource leaks.
  * - Supports `NPatchInfo` for advanced texture configurations.
  */
struct TextureNPatchInfo 
{
    int sourceX, sourceY, sourceWidth, sourceHeight;
    int padLeft, padTop, padRight, padBottom;
    int HoverOffsetX, HoverOffsetY;
    std::string layout;

    // Conversion Operator to return a NPatchInfo implicitly
    operator Sonar::NPatchInfo()
    {
        return Sonar::NPatchInfo { Sonar::Rectangle {(float)sourceX, (float)sourceY, (float)sourceWidth, (float)sourceHeight }, padLeft, padTop, padRight, padBottom };
    }

    Sonar::NPatchInfo GetOfsettedNPatchInfo()
    {
        return Sonar::NPatchInfo{ Sonar::Rectangle {(float)sourceX + HoverOffsetX, (float)sourceY + HoverOffsetY, (float)sourceWidth, (float)sourceHeight }, padLeft, padTop, padRight, padBottom };

    }
};

struct TextureResource 
{
    friend SharedTexture2D;
    friend GameInstance;
    friend ResourceManager;

    TextureResource()
        :WorkerDone(true)
    {}

    TextureResource(TextureResource&& Other) noexcept
        : name(std::move(Other.name)),
        textureID(Other.textureID),
        textureKind(std::move(Other.textureKind)),
        nPatchInfo(std::move(Other.nPatchInfo)),
        path(std::move(Other.path)),
        width(Other.width),
        height(Other.height),
        format(std::move(Other.format)),
        wrapMode(std::move(Other.wrapMode)),
        LoadedTexture(Other.LoadedTexture),
        RefCount(Other.RefCount.load(std::memory_order_relaxed)),
        WorkerDone(Other.WorkerDone.load()),
        WorkerFuture(std::move(Other.WorkerFuture)),
        Owner(Other.Owner)
    {

        Other.textureID = -1;
        Other.LoadedTexture = {};
        Other.RefCount = 0;
        Other.WorkerDone.store(true);
        Other.Owner = nullptr;
    }

    TextureResource& operator=(TextureResource&& Other) noexcept 
    {
        if (this != &Other) 
        {
            name = std::move(Other.name);
            textureID = Other.textureID;
            textureKind = std::move(Other.textureKind);
            nPatchInfo = std::move(Other.nPatchInfo);
            path = std::move(Other.path);
            width = Other.width;
            height = Other.height;
            format = std::move(Other.format);
            wrapMode = std::move(Other.wrapMode);
            LoadedTexture = Other.LoadedTexture;
            RefCount.store(Other.RefCount.load(std::memory_order_relaxed), std::memory_order_relaxed);
            WorkerDone.store(Other.WorkerDone.load());
            WorkerFuture = std::move(Other.WorkerFuture);
            Owner = Other.Owner;


            Other.textureID = -1;
            Other.LoadedTexture = {};
            Other.RefCount = 0;
            Other.WorkerDone.store(true);
            Other.Owner = nullptr;
        }
        return *this;
    }

    TextureResource(const TextureResource&) = delete;
    TextureResource& operator=(const TextureResource&) = delete;

    std::string name;
    int textureID;
    std::string textureKind;
    std::optional<TextureNPatchInfo> nPatchInfo;
    std::string path;
    int width;
    int height;
    std::string format;
    std::string wrapMode;

    SONARENGINE_API SharedTexture2D LoadTexture();

    // Properties

    SONARENGINE_API void SetHeight(int Height);
    SONARENGINE_API void SetWidth(int Width);
    SONARENGINE_API void GenerateMipMaps();
    SONARENGINE_API void ForceCleanup();


private:

    Sonar::Texture2D LoadedTexture = {};
    std::atomic<int> RefCount = 0;
    std::atomic<bool> WorkerDone;
    std::atomic<bool> ShutdownRequested{false};
    std::future<void> WorkerFuture;
    ResourceManager* Owner = nullptr;

    void AddRef()
    {
        RefCount.fetch_add(1, std::memory_order_relaxed);
    }

    void RemoveRef();
    bool UnloadTexture();
};

class SONARENGINE_API ResourceManager
{
public:
	ResourceManager() = default;
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	void ParseJson();
    void CleanAllResources();
    void SetWorkingDirectory(std::string WorkingDirectory) { m_WorkingDirectory = std::move(WorkingDirectory); }
    void SetResourceRoot(std::string ResourceRoot) { m_ResourceRoot = std::move(ResourceRoot); }
    void SetManifestPath(std::string ManifestPath) { m_ManifestPath = std::move(ManifestPath); }
    void SetMainQueue(GameThreadQueue* MainQueue) { m_MainQueue = MainQueue; }
    const std::string& GetWorkingDirectory() const { return m_WorkingDirectory; }
    const std::string& GetResourceRoot() const { return m_ResourceRoot; }
    std::string GetManifestPath() const;
    std::string ResolveResourcePath(const std::string& RelativePath) const;
    std::string ResolveShaderPath(const std::string& ShaderFileName) const;
    void EnqueueMainThreadTask(std::function<void()> Task);
    std::unordered_map<std::string, TextureResource> AllResources;

private:
    std::string m_WorkingDirectory;
    std::string m_ResourceRoot;
    std::string m_ManifestPath;
    GameThreadQueue* m_MainQueue = nullptr;
};