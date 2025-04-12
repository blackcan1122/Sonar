#include "Base/Texture2DWrap.hpp"
#include "Base/ResourceManager.hpp"
#include <utility>
#include <iomanip>

Texture2DWrap::Texture2DWrap(Texture2D* TextureHandle, TextureResource* Outter)
	:m_Texture(TextureHandle), m_Outter(Outter)
{
	m_IsInitialized = true;
	m_Outter->AddRef();
	std::cout << m_Outter->name << ": " << m_Outter->RefCount << std::endl;
}

Texture2DWrap::Texture2DWrap(const Texture2DWrap& Other)
	:m_Outter(Other.m_Outter), m_Texture(Other.m_Texture)
{
	if (this != &Other)
	{
		m_IsInitialized = true;
		m_Outter->AddRef();
		std::cout << m_Outter->name << ": " << m_Outter->RefCount << std::endl;
	}
}

Texture2DWrap::Texture2DWrap(Texture2DWrap&& Other) noexcept
	:m_Outter(Other.m_Outter), m_Texture(Other.m_Texture)
{
	m_IsInitialized = true;
	Other.m_Outter = nullptr;
	Other.m_Texture = nullptr;
}

Texture2DWrap::~Texture2DWrap()
{
	if (m_IsInitialized)
	{
		m_Outter->RemoveRef();
		std::cout << m_Outter->name << ": " << m_Outter->RefCount << std::endl;
	}
}

Texture2DWrap& Texture2DWrap::operator=(const Texture2DWrap& Other)
{
	if (this == &Other)
	{
		return *this;
	}
	m_IsInitialized = true;
	m_Outter = Other.m_Outter;
	m_Texture = Other.m_Texture;
	m_Outter->AddRef();
	return *this;
}

Texture2DWrap& Texture2DWrap::operator=(Texture2DWrap& Other)
{
	if (this == &Other)
	{
		return *this;
	}
	m_IsInitialized = true;
	m_Outter = Other.m_Outter;
	m_Texture = Other.m_Texture;
	m_Outter->AddRef();
	return *this;
}

Texture2DWrap::operator Texture2D()
{
	return *m_Texture;
}
