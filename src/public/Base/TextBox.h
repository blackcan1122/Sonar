#pragma once
#include "raylib.h"
#include <string>
#include "BaseUI.h"


class TextInputBox : public BaseUI
{
public:

	// Methods

	/*
	* This class got way to specialized for the Chat
	* TODO: Should maybe be split up in different inheritaded classes and somehow be more aligned with the button class
	* Also not really smart to use const char[] here while using a std::string in the button....
	*/

	TextInputBox() = default;

	TextInputBox& Construct(Rectangle Box, Color BackgroundColor);
	TextInputBox& Construct(float x, float y, float width, float height, Color BackgroundColor);
	TextInputBox& UseBorder(bool UseBorder);
	TextInputBox& SetInitialText(std::string InitialText);
	TextInputBox& SetInitialText(char InitalText[51]);
	TextInputBox& UseWordWrap();
	TextInputBox& UseContains();
	TextInputBox& UseCenter();
	TextInputBox& ResizeBoxToText();
	TextInputBox& CanBeEdited(bool IsEditable);
	TextInputBox& ChangeMaxChars(int MaxCharacters);
	TextInputBox& EreaseText();
	std::string GetText();



	TextInputBox& UpdateTextOffset(Vector2 NewTextPosition);
	TextInputBox& UpdateBoxPositionOffset(Vector2 BoxOffset);
	TextInputBox& SetBoxPosition(Vector2 BoxOffset);
	TextInputBox& UpdateTextPosition();
	TextInputBox& UpdateFontSize(int NewFontSize);
	TextInputBox& UpdateFontColor(Color NewFontColor);
	TextInputBox& UpdatePosition(Vector2 NewPosition);
	TextInputBox& UpdateSize(Vector2 NewSize);
	TextInputBox& UpdateColor(Color NewColor);
	TextInputBox& UpdateTextShown(std::string Text);
	TextInputBox& UpdateBorder(int Thickness, Color BorderColor);
	TextInputBox& ResetSizeToInitial();



	virtual void Update() override;

	bool bIsFocused(Vector2 MousePosition);

	char StringToHold[1024 + 1] = "\0";

	Rectangle Box;

private:

	// Member
	
	Color BoxBackgroundColor;
	std::string Text;
	Vector2 TextPosition;
	Vector2 TextOffset = { 0,0 };
	int FontSize = 8;
	Color FontColor = WHITE;
	bool IsEditable = true;
	bool bShouldCenter = false;

	int LastSpaceIndex;
	int BackupSpaceIndex;

	float DelayTimer;
	float RepeatTimer;
	float Delay = 0.2f;
	float RepeatInterval = 0.05f;

	int MaxChars = 5;
	
	
	int LetterCount = 0;

	Rectangle InitialPosition;

	bool bWordWrap = false;
	bool bContainsText = false;
	bool bGrowBoxToText = false;
	int NumberOfLines = 0;

	bool bUseBorder;
	bool bHasBeenConstructed;
	bool bUsesText;

};

