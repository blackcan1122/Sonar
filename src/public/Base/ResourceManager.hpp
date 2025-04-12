#pragma once
#include "Base/Core.h"
#include "json.hpp"
#include <fstream>
#include "Texture2DWrap.hpp"

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

    void AddRef()
    {
        RefCount++;
    }

    void RemoveRef()
    {
        RefCount--;
    }

    bool UnloadTexture()
    {
        RAYLIB_H::UnloadTexture(LoadedTexture);
        LoadedTexture = {};
        return true;
    }
};

class ResourceManager
{
public:
	void ParseJson();
    std::unordered_map<std::string, TextureResource> AllResources;
};