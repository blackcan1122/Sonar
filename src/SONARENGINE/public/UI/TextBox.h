#pragma once
#include "Rendering/Graphics.hpp"
#include <string>
#include "Base/BaseUI.h"

class SONARENGINE_API TextInputBox : public BaseUI
{
	AUTOBODY(TextInputBox, BaseUI)
public:
	TextInputBox() = default;

	TextInputBox& Construct(Sonar::Rectangle Box, Sonar::Color BackgroundColor);
	TextInputBox& Construct(float x, float y, float width, float height, Sonar::Color BackgroundColor);
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

	TextInputBox& UpdateTextOffset(Sonar::Vector2 NewTextPosition);
	TextInputBox& UpdateBoxPositionOffset(Sonar::Vector2 BoxOffset);
	TextInputBox& SetBoxPosition(Sonar::Vector2 BoxOffset);
	TextInputBox& UpdateTextPosition();
	TextInputBox& UpdateFontSize(int NewFontSize);
	TextInputBox& UpdateFontColor(Sonar::Color NewFontColor);
	TextInputBox& UpdatePosition(Sonar::Vector2 NewPosition);
	TextInputBox& UpdateSize(Sonar::Vector2 NewSize);
	TextInputBox& UpdateColor(Sonar::Color NewColor);
	TextInputBox& UpdateTextShown(std::string Text);
	TextInputBox& UpdateBorder(int Thickness, Sonar::Color BorderColor);
	TextInputBox& ResetSizeToInitial();

	virtual void Tick(float DeltaTime) override;

	bool bIsFocused(Sonar::Vector2 MousePosition);

	char StringToHold[1024 + 1] = "\0";

	Sonar::Rectangle Box;

private:
	Sonar::Color BoxBackgroundColor;
	std::string Text;
	Sonar::Vector2 TextPosition;
	Sonar::Vector2 TextOffset = { 0,0 };
	int FontSize = 8;
	Sonar::Color FontColor = Sonar::WHITE;
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

	Sonar::Rectangle InitialPosition;

	bool bWordWrap = false;
	bool bContainsText = false;
	bool bGrowBoxToText = false;
	int NumberOfLines = 0;

	bool bUseBorder;
	bool bHasBeenConstructed;
	bool bUsesText;
};