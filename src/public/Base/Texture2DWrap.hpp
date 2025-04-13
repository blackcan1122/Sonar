#pragma once
#include "Base/Core.h"

class ResourceManager;
struct TextureResource;


class Texture2DWrap
{
public:
	
	Texture2DWrap() = default;
	Texture2DWrap(Texture2D* TextureHandle, TextureResource* Outter);
	Texture2DWrap(const Texture2DWrap& other);
	Texture2DWrap(Texture2DWrap&& Other) noexcept;

	~Texture2DWrap();

	// Copy Assignment
	Texture2DWrap& operator=(const Texture2DWrap& Other);
	Texture2DWrap& operator=(Texture2DWrap& Other);

	// Move Assignment
	Texture2DWrap& operator=(Texture2DWrap&& Other);

	// Conversion Operator to Texture2D
	operator Texture2D();


private:

	bool m_IsInitialized = false;
	TextureResource* m_Outter;
	Texture2D* m_Texture;

};