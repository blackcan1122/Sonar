#pragma once
#include "raylib.h"

struct IntVector2
{
	IntVector2() {};
	IntVector2(int X_In, int Y_In)
	{
		X = X_In;
		Y = Y_In;
	};

	int X;
	int Y;
};

struct WindowProperties
{
	IntVector2 WindowDimensions;
	int MaxFPS;

};


class WindowHandle
{
public:
	WindowProperties ActiveWindow;
};

