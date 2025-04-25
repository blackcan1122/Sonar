#pragma once


class ResourceManager;
struct TextureResource;

/**
 * @class SharedTexture2D
 * @brief A wrapper for managing shared ownership of `Texture2D` objects.
 *
 * Purpose:
 * - Simplifies the management of `Texture2D` objects with reference counting.
 * - Ensures texture resources are properly shared and freed when no longer in use.
 *
 * Usage:
 * - Create `SharedTexture2D` objects to manage `Texture2D` instances.
 * - Use the conversion operators to interact with the underlying `Texture2D` object.
 *
 * Notes:
 * - Textures are automatically freed when the last `SharedTexture2D` instance is destroyed.
 * - Avoid directly managing the lifetime of the underlying `Texture2D` object.
 */
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