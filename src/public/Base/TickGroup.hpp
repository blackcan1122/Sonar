#pragma once
#include "Base/Core.h"

// Right now we will only use DefaultTick and Rendering and MAX
enum class ETickGroup : uint8_t
{
	DefaultTick			= 0,
	PostTick			= 1 << 0,
	Physics				= 1 << 1,
	PostPhysics			= 1 << 2,
	PostUpdate			= 1 << 3,
	Rendering			= 1 << 4,
	MAX					= 1 << 5
};

class TickGroup
{
public:
	TickGroup() = default;
	TickGroup(ETickGroup InitialGroup) : m_TickGroup(InitialGroup) {};
	ETickGroup GetTickGroup() const { return m_TickGroup; };
	void SetTickGroup(ETickGroup NewGroup) { m_TickGroup = NewGroup; };

private:
	ETickGroup m_TickGroup = ETickGroup::DefaultTick;

};