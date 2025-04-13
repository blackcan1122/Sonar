#pragma once
#include "Base/Core.h"

class ResourceManager;
struct TextureResource;


class SharedTexture2D
{
public:
	
	SharedTexture2D() = default;
	SharedTexture2D(Texture2D* TextureHandle, TextureResource* Outter);
	SharedTexture2D(const SharedTexture2D& other);
	SharedTexture2D(SharedTexture2D&& Other) noexcept;

	~SharedTexture2D();

	// Copy Assignment
	SharedTexture2D& operator=(const SharedTexture2D& Other);
	SharedTexture2D& operator=(SharedTexture2D& Other);

	// Move Assignment
	SharedTexture2D& operator=(SharedTexture2D&& Other);

	// Conversion Operator to Texture2D
	operator Texture2D();
	operator Texture2D* ();


private:

	bool m_IsInitialized = false;
	TextureResource* m_Outter;
	Texture2D* m_Texture;

};