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

    operator NPatchInfo()
    {
        
        return NPatchInfo { Rectangle {(float)sourceX, (float)sourceY, (float)sourceWidth, (float)sourceHeight }, padLeft, padTop, padRight, padBottom };
        
    }

    NPatchInfo GetUpdatedNPatchInfo()
    {
        return NPatchInfo{ Rectangle {(float)sourceX + HoverOffsetX, (float)sourceY + HoverOffsetY, (float)sourceWidth, (float)sourceHeight }, padLeft, padTop, padRight, padBottom };

    }
};

struct TextureResource {
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
};

class ResourceManager
{
public:
	void ParseJson();
    std::unordered_map<std::string, TextureResource> AllResources;
};