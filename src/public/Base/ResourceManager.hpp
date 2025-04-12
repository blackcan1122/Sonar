#pragma once
#include "Base/Core.h"
#include "json.hpp"
#include <fstream>

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
    std::string name;
    Image ImageTexture;
    int textureID;
    std::string textureKind;
    std::optional<TextureNPatchInfo> nPatchInfo;
    std::string path;
    int width;
    int height;
    std::string format;
    std::string wrapMode;

private:
    Texture2D LoadedTexture;
    int RefCount = 0;

    Texture2D LoadTexture()
    {
        // TODO: Own Texture Handle with:
        // Custom Constructor
        // Custom Move Constructor / operator
        // Custom Copy operator
        // Custom Desturctor
        // Should be passed a pointer to its outter
        LoadedTexture = RAYLIB_H::LoadTexture(path.c_str());
        RefCount++;
        return LoadedTexture;
    }

    bool UnloadTexture()
    {
        RAYLIB_H::UnloadTexture(LoadedTexture);
    }
};

class ResourceManager
{
public:
	void ParseJson();
    std::unordered_map<std::string, TextureResource> AllResources;
};