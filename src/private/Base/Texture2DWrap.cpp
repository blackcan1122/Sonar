#include "Base/Core.h"
#include "Base/Texture2DWrap.hpp"
#include "Base/ResourceManager.hpp"
#include <utility>
#include <iomanip>

SharedTexture2D::SharedTexture2D(Texture2D* TextureHandle, TextureResource* Outter)
	:m_Texture(TextureHandle), m_Outter(Outter)
{
	if (Outter)
	{
		m_IsInitialized = true;
		m_Outter->AddRef();
	}

}

SharedTexture2D::SharedTexture2D(const SharedTexture2D& Other)
	:m_Outter(Other.m_Outter), m_Texture(Other.m_Texture)
{
	if (Other.m_IsInitialized == false)
	{
		return;
	}
	if (this != &Other)
	{
		m_IsInitialized = true;
		m_Outter->AddRef();
	}
}

SharedTexture2D::SharedTexture2D(SharedTexture2D&& Other) noexcept
	:m_Outter(Other.m_Outter), m_Texture(Other.m_Texture)
{
	if (Other.m_IsInitialized == false)
	{
		return;
	}
	m_IsInitialized = true;
	Other.m_IsInitialized = false;
	Other.m_Outter = nullptr;
	Other.m_Texture = nullptr;
}

SharedTexture2D::~SharedTexture2D()
{
	if (m_IsInitialized)
	{
		m_Outter->RemoveRef();
	}
}

SharedTexture2D& SharedTexture2D::operator=(const SharedTexture2D& Other)
{
	if (this == &Other)
	{
		return *this;
	}

	if (Other.m_IsInitialized)
	{
		m_IsInitialized = true;
		m_Outter = Other.m_Outter;
		m_Texture = Other.m_Texture;
		m_Outter->AddRef();
	}

	return *this;
}

SharedTexture2D& SharedTexture2D::operator=(SharedTexture2D& Other)
{
	if (this == &Other)
	{
		return *this;
	}

	if (Other.m_IsInitialized)
	{
		m_IsInitialized = true;
		m_Outter = Other.m_Outter;
		m_Texture = Other.m_Texture;
		m_Outter->AddRef();
	}
	return *this;
}

SharedTexture2D& SharedTexture2D::operator=(SharedTexture2D&& Other)
{
	if (this != &Other) 
	{
		// Release current resource if initialized
		if (m_IsInitialized && m_Outter) 
		{
			m_Outter->RemoveRef();
		}

		// Transfer ownership
		m_IsInitialized = true;
		m_Outter = Other.m_Outter;
		m_Texture = Other.m_Texture;

		// Reset the source object
		Other.m_Outter = nullptr;
		Other.m_Texture = nullptr;
		Other.m_IsInitialized = false;
	}
	return *this;
}

SharedTexture2D::operator Texture2D()
{
	m_Texture->height = m_Outter->height;
	m_Texture->width = m_Outter->width;
	return *m_Texture;
}

SharedTexture2D::operator Texture2D* ()
{
	m_Texture->height = m_Outter->height;
	m_Texture->width = m_Outter->width;
	return m_Texture;
}

bool SharedTexture2D::isValid() const
{
    return m_Outter != nullptr;
}
