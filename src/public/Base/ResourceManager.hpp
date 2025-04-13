#pragma once
#include "Base/Core.h"
#include "json.hpp"
#include <fstream>
#include <atomic>
#include <thread>
#include <future>
#include "Texture2DWrap.hpp"
#include "Base/EventDispatcher.hpp"
#include "Events/AllPurposeEvent.h"
#include "Events/StringEventData.hpp"

using json = nlohmann::json;

struct TextureNPatchInfo 
{
    int sourceX, sourceY, sourceWidth, sourceHeight;
    int padLeft, padTop, padRight, padBottom;
    int HoverOffsetX, HoverOffsetY;
    std::string layout;

    // Conversion Operator to return a NPatchInfo implicitly
    operator NPatchInfo()
    {
        return NPatchInfo { Rectangle {(float)sourceX, (float)sourceY, (float)sourceWidth, (float)sourceHeight }, padLeft, padTop, padRight, padBottom };
    }

    NPatchInfo GetOfsettedNPatchInfo()
    {
        return NPatchInfo{ Rectangle {(float)sourceX + HoverOffsetX, (float)sourceY + HoverOffsetY, (float)sourceWidth, (float)sourceHeight }, padLeft, padTop, padRight, padBottom };

    }
};

struct TextureResource 
{
    friend Texture2DWrap;

    TextureResource()
        :WorkerDone(true)
    {
        UnloadDispatcher.AddListener("Unload Texture", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event)
            {
                UnloadTexture();
            });
    }

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
        LoadedTexture(Other.LoadedTexture),  // Transfer texture handle
        RefCount(Other.RefCount),
        WorkerDone(Other.WorkerDone.load()),  // Atomic load
        WorkerFuture(std::move(Other.WorkerFuture))
    {
        // Invalidate source object
        Other.textureID = -1;
        Other.LoadedTexture = {};  // Reset texture to empty
        Other.RefCount = 0;
        Other.WorkerDone.store(true);

        Other.UnloadDispatcher.RemoveListener("Unload Texture", AllPurposeEvent::StaticClass());
        UnloadDispatcher.AddListener("Unload Texture", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event)
            {
                UnloadTexture();
            });

        // CAUTION: Any existing Texture2DWrap pointing to 'Other' 
        // will now have dangling pointers!
    }

    TextureResource& operator=(TextureResource&& Other) noexcept {
        if (this != &Other) 
        {
            // Cleanup existing resources only if not self-assignment

            // Transfer members
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
            RefCount = Other.RefCount;
            WorkerDone.store(Other.WorkerDone.load());
            WorkerFuture = std::move(Other.WorkerFuture);

            // Invalidate source
            Other.textureID = -1;
            Other.LoadedTexture = {};
            Other.RefCount = 0;
            Other.WorkerDone.store(true);
        }
        Other.UnloadDispatcher.RemoveListener("Unload Texture", AllPurposeEvent::StaticClass());
        UnloadDispatcher.AddListener("Unload Texture", AllPurposeEvent::StaticClass(), [this](std::shared_ptr<IEvent> Event)
            {
                UnloadTexture();
            });
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

    Texture2DWrap LoadTexture()
    {
        if (LoadedTexture.id == 0)
        {
            LoadedTexture = RAYLIB_H::LoadTexture(path.c_str());

        }
        return Texture2DWrap (&LoadedTexture, this);
        
    }

private:

    Texture2D LoadedTexture = {};
    int RefCount = 0;
    std::atomic<bool> WorkerDone;
    std::future<void> WorkerFuture;

    EventDispatcher UnloadDispatcher;

    void AddRef()
    {
        RefCount++;
    }

    void RemoveRef()
    {
        RefCount--;

        if (RefCount == 0 && WorkerDone.load())
        {
            WorkerFuture = std::async(std::launch::async, [this]()
                {
                    WorkerDone.store(false);
                    auto StartTime = std::chrono::system_clock::now();
                    auto CurrentTime = StartTime;
                    bool bStillZero = true;
                    while (StartTime + std::chrono::seconds(2) > CurrentTime)
                    {
                        CurrentTime = std::chrono::system_clock::now();
                        if (RefCount != 0)
                        {
                            bStillZero = false;
                        }
                    }

                    if (bStillZero)
                    {
                        std::shared_ptr<AllPurposeEvent> UnloadEvent = std::make_shared<AllPurposeEvent>();
                        std::shared_ptr<StringEventData> mStringEventData = std::make_shared<StringEventData>();

                        mStringEventData->String = "Unload";
                        UnloadEvent->Payload = std::dynamic_pointer_cast<IEventData>(mStringEventData);

                        UnloadDispatcher.Dispatch(UnloadEvent);
                        WorkerDone.store(true);
                        return;
                    }
                  
                    WorkerDone.store(true);
                    return;
                });
        }
    }

    bool UnloadTexture()
    {
        if (LoadedTexture.id != 0)
        {
            RAYLIB_H::UnloadTexture(LoadedTexture); // Somehow openGL throws an exception here
            std::cout << "Unloaded Textures from Vram yaay" << std::endl;
            LoadedTexture = {};
            return true;
        }
        return false;
    }
};

class ResourceManager
{
public:
	void ParseJson();
    std::unordered_map<std::string, TextureResource> AllResources;
};