#pragma once
#include "Base/Core.h"
#include "single_include/nlohmann/json.hpp"
#include <fstream>
#include <atomic>
#include <thread>
#include <future>
#include "Texture2DWrap.hpp"
#include "Base/EventDispatcher.hpp"
#include "Events/AllPurposeEvent.h"
#include "Events/StringEventData.hpp"
#include "Base/GameThreadQueue.hpp"

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
    friend SharedTexture2D;

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
        RefCount(Other.RefCount),
        WorkerDone(Other.WorkerDone.load()),
        WorkerFuture(std::move(Other.WorkerFuture))
    {

        Other.textureID = -1;
        Other.LoadedTexture = {};
        Other.RefCount = 0;
        Other.WorkerDone.store(true);
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
            RefCount = Other.RefCount;
            WorkerDone.store(Other.WorkerDone.load());
            WorkerFuture = std::move(Other.WorkerFuture);


            Other.textureID = -1;
            Other.LoadedTexture = {};
            Other.RefCount = 0;
            Other.WorkerDone.store(true);
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

    SharedTexture2D LoadTexture();

private:

    Texture2D LoadedTexture = {};
    int RefCount = 0;
    std::atomic<bool> WorkerDone;
    std::future<void> WorkerFuture;

    void AddRef()
    {
        RefCount++;
    }

    void RemoveRef();

    bool UnloadTexture();
};

class ResourceManager
{
public:
	void ParseJson();
    std::unordered_map<std::string, TextureResource> AllResources;
};