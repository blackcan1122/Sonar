#pragma once
#include "Base/Core.h"

DECLARE_CLASS(Entity, Object)

public:
	Vector2 Position = { 0,0 };
	Vector2 Velocity = { 0,0 };
	Vector2 FacingVector = { 0, -1 };
	float Rotation = 0.f;
	float Scale = 1.f;

	bool IsActive = true;
	bool IsVisible = true;
	bool CollisionEnabled = false;

	Texture2D Texture; // don't know about this, since we should need a way to cicyle through a tileset maybe

	std::string Name;

	virtual void Tick(float DeltaTime) override;


	

END_CLASS