#include <cstring>
#include "UI/TextBox.h"

TextInputBox& TextInputBox::Construct(Sonar::Rectangle Box, Sonar::Color BackgroundColor)
{
	this->Box = Box;
	this->InitialPosition = Box;
	this->BoxBackgroundColor = BackgroundColor;
	this->bUseBorder = false;
	return *this;
}

TextInputBox& TextInputBox::Construct(float x, float y, float width, float height, Sonar::Color BackgroundColor)
{
	Sonar::Rectangle TempObj = { x, y, width, height };
	return Construct(TempObj, BackgroundColor);
}

TextInputBox& TextInputBox::UseBorder(bool UseBorder)
{
	return *this;
}

TextInputBox& TextInputBox::SetInitialText(std::string InitialText)
{
	this->Text = InitialText;
	return *this;
}

TextInputBox& TextInputBox::SetInitialText(char InitalText[1024])
{
	strncpy(this->StringToHold, InitalText, sizeof(this->StringToHold) - 1);
	this->StringToHold[sizeof(this->StringToHold) - 1] = '\0';
	LetterCount = strlen(InitalText);
	return *this;
}

TextInputBox& TextInputBox::UseWordWrap()
{
	bContainsText = true;
	bWordWrap = true;
	return *this;
}

TextInputBox& TextInputBox::UseContains()
{
	bContainsText = true;
	return *this;
}

TextInputBox& TextInputBox::UseCenter()
{
	bShouldCenter = true;
	return *this;
}

TextInputBox& TextInputBox::ResizeBoxToText()
{
	bGrowBoxToText = true;
	return *this;
}

TextInputBox& TextInputBox::CanBeEdited(bool IsEditable)
{
	this->IsEditable = IsEditable;
	return *this;
}

TextInputBox& TextInputBox::ChangeMaxChars(int MaxCharacters)
{
	MaxChars = MaxCharacters;
	return *this;
}

TextInputBox& TextInputBox::EreaseText()
{
	LetterCount = 0;
	StringToHold[LetterCount] = '\0';
	return *this;
}

std::string TextInputBox::GetText()
{
	return Text;
}

TextInputBox& TextInputBox::UpdateTextOffset(Sonar::Vector2 NewTextPosition)
{
	this->TextOffset = NewTextPosition;
	return *this;
}

TextInputBox& TextInputBox::UpdateBoxPositionOffset(Sonar::Vector2 BoxOffset)
{
	Box.x = Box.x + BoxOffset.x;
	Box.y = Box.y + BoxOffset.y;
	return *this;
}

TextInputBox& TextInputBox::SetBoxPosition(Sonar::Vector2 BoxOffset)
{
	Box.x = BoxOffset.x;
	Box.y = BoxOffset.y;
	return *this;
}

TextInputBox& TextInputBox::UpdateTextPosition()
{
	return *this;
}

TextInputBox& TextInputBox::UpdateFontSize(int NewFontSize)
{
	FontSize = NewFontSize;
	return *this;
}

TextInputBox& TextInputBox::UpdateFontColor(Sonar::Color NewFontColor)
{
	FontColor = NewFontColor;
	return *this;
}

TextInputBox& TextInputBox::UpdatePosition(Sonar::Vector2 NewPosition)
{
	this->Box.x = NewPosition.x;
	this->Box.y = NewPosition.y;
	return *this;
}

TextInputBox& TextInputBox::UpdateSize(Sonar::Vector2 NewSize)
{
	this->Box.width = NewSize.x;
	this->Box.height = NewSize.y;
	return *this;
}

TextInputBox& TextInputBox::UpdateColor(Sonar::Color NewColor)
{
	this->BoxBackgroundColor = NewColor;
	return *this;
}

TextInputBox& TextInputBox::UpdateTextShown(std::string Text)
{
	this->Text = Text;
	return *this;
}

TextInputBox& TextInputBox::UpdateBorder(int Thickness, Sonar::Color BorderColor)
{
	return *this;
}

TextInputBox& TextInputBox::ResetSizeToInitial()
{
	Box = InitialPosition;
	return *this;
}

bool TextInputBox::bIsFocused(Sonar::Vector2 MousePosition)
{
	return Sonar::CheckCollisionPointRec(MousePosition, this->Box);
}

void TextInputBox::Tick(float DeltaTime)
{
	if (bGrowBoxToText)
	{
		this->Box.y = Box.y - (FontSize * NumberOfLines);
		this->Box.height = Box.height + (FontSize * NumberOfLines);
		NumberOfLines = 0;
	}

	Sonar::DrawRectangle(this->Box.x, this->Box.y, this->Box.width, this->Box.height, this->BoxBackgroundColor);

	if (bIsFocused(Sonar::GetMousePosition()) && this->IsEditable)
	{
		int PressedKey = Sonar::GetCharPressed();

		if (Sonar::IsKeyPressed(Sonar::KEY_BACKSPACE))
		{
			if (LetterCount > 0)
			{
				LetterCount--;
				if (StringToHold[LetterCount] == '\n')
				{
					NumberOfLines--;
				}

				StringToHold[LetterCount] = '\0';
			}

			DelayTimer = 0.0f;
			RepeatTimer = 0.0f;
		}
		else if (Sonar::IsKeyDown(Sonar::KEY_BACKSPACE))
		{
			float DeltaTimeFrame = Sonar::GetFrameTime();
			DelayTimer += DeltaTimeFrame;

			if (DelayTimer >= Delay)
			{
				RepeatTimer += DeltaTimeFrame;
				if (RepeatTimer >= RepeatInterval)
				{
					if (LetterCount > 0)
					{
						LetterCount--;
						if (StringToHold[LetterCount] == '\n')
						{
							NumberOfLines--;
						}
						StringToHold[LetterCount] = '\0';
					}

					RepeatTimer = 0.0f;
				}
			}
		}
		else
		{
			DelayTimer = 0.0f;
			RepeatTimer = 0.0f;
		}

		if (strlen(StringToHold) < MaxChars && PressedKey > 0)
		{
			if (Sonar::MeasureText(StringToHold, FontSize) > Box.width && bContainsText)
			{
				if (bWordWrap)
				{
					std::string TextBuffer;
					TextBuffer.append(&StringToHold[LastSpaceIndex]);
					StringToHold[LastSpaceIndex - 1] = '\n';
					LetterCount = LastSpaceIndex;

					for (size_t i = 0; i < TextBuffer.size(); i++)
					{
						StringToHold[LetterCount] = TextBuffer[i];
						LetterCount++;
					}

					StringToHold[LetterCount] = (char)PressedKey;
					LetterCount++;
					StringToHold[LetterCount] = '\0';
					NumberOfLines++;
					std::cout << "After WordWrap: " << LetterCount << std::endl;
				}
				else
				{
					StringToHold[LetterCount] = '\n';
					StringToHold[LetterCount + 1] = (char)PressedKey;
					StringToHold[LetterCount + 2] = '\0';
					NumberOfLines++;
					LetterCount++;
				}
			}
			else
			{
				StringToHold[LetterCount] = (char)PressedKey;
				StringToHold[LetterCount + 1] = '\0';
				LetterCount++;
			}

			if (PressedKey == 32)
			{
				BackupSpaceIndex = LastSpaceIndex;
				LastSpaceIndex = LetterCount;
			}
		}
	}

	if (bShouldCenter)
	{
		int LineCount = 1;
		size_t pos = 0;

		std::string TempString(StringToHold);

		while ((pos = TempString.find('\n', pos)) != std::string::npos)
		{
			LineCount++;
			pos++;
		}

		int textWidth = Sonar::MeasureText(TempString.c_str(), FontSize);
		int LineHeight = FontSize * LineCount;

		float XPosition = (Box.width - textWidth) / 2.0f;
		float YPosition = (Box.height - LineHeight) / 2.0f;

		TextOffset.x = XPosition;
		TextOffset.y = YPosition;
	}

	if (IsEditable)
	{
		Sonar::DrawText(StringToHold, Box.x + TextOffset.x, Box.y + TextOffset.y, FontSize, FontColor);
	}
	else
	{
		Sonar::DrawText(Text.c_str(), Box.x + TextOffset.x, Box.y + TextOffset.y, FontSize, FontColor);
	}
}