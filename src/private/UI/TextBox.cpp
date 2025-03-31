#include "UI/TextBox.h"

TextInputBox& TextInputBox::Construct(Rectangle Box, Color BackgroundColor)
{
	this->Box = Box;
	this->InitialPosition = Box;
	this->BoxBackgroundColor = BackgroundColor;
	this->bUseBorder = false;
	//this->TextPosition.x = this->Box.x;
	
	//this->TextPosition.y = this->Box.y + this->Box.height / 2 - (FontSize/2);
	return *this;
}

TextInputBox& TextInputBox::Construct(float x, float y, float width, float height, Color BackgroundColor)
{
	Rectangle TempObj = { x,y, width, height };
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
	this->StringToHold[sizeof(this->StringToHold) - 1] = '\0';  // Ensure null-termination
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

TextInputBox& TextInputBox::UpdateTextOffset(Vector2 NewTextPosition)
{
	this->TextOffset = NewTextPosition;
	return *this;
}

TextInputBox& TextInputBox::UpdateBoxPositionOffset(Vector2 BoxOffset)
{
	Box.x = Box.x + BoxOffset.x;
	Box.y = Box.y + BoxOffset.y;
	return *this;
}

TextInputBox& TextInputBox::SetBoxPosition(Vector2 BoxOffset)
{
	Box.x = BoxOffset.x;
	Box.y = BoxOffset.y;
	return *this;
}

TextInputBox& TextInputBox::UpdateTextPosition()
{
	//this->TextPosition.x = this->Box.x;

	//this->TextPosition.y = this->Box.y;
	return *this;
}

TextInputBox& TextInputBox::UpdateFontSize(int NewFontSize)
{
	FontSize = NewFontSize;
	return *this;
}

TextInputBox& TextInputBox::UpdateFontColor(Color NewFontColor)
{
	FontColor = NewFontColor;
	return *this;
}

TextInputBox& TextInputBox::UpdatePosition(Vector2 NewPosition)
{
	this->Box.x = NewPosition.x;
	this->Box.y = NewPosition.y;
	return *this;
}

TextInputBox& TextInputBox::UpdateSize(Vector2 NewSize)
{
	this->Box.width = NewSize.x;
	this->Box.height = NewSize.y;
	return *this;
}

TextInputBox& TextInputBox::UpdateColor(Color NewColor)
{
	this->BoxBackgroundColor = NewColor;
	return *this;
}

TextInputBox& TextInputBox::UpdateTextShown(std::string Text)
{
	this->Text = Text;
	return *this;
}

TextInputBox& TextInputBox::UpdateBorder(int Thickness, Color BorderColor)
{
	return *this;
}

TextInputBox& TextInputBox::ResetSizeToInitial()
{
	Box = InitialPosition;
	return *this;
}

bool TextInputBox::bIsFocused(Vector2 MousePosition)
{
	return CheckCollisionPointRec(MousePosition, this->Box);
}



void TextInputBox::Update()
{
	if (bGrowBoxToText)
	{
		this->Box.y = Box.y - (FontSize * NumberOfLines);
		this->Box.height = Box.height + (FontSize * NumberOfLines);
		NumberOfLines = 0;
	}

	DrawRectangle(this->Box.x, this->Box.y, this->Box.width, this->Box.height, this->BoxBackgroundColor);


	

	if (bIsFocused(GetMousePosition()) && this->IsEditable)
	{
		int PressedKey = GetCharPressed();

		// ########################
		// Deleting Logic
		// ########################
		if (IsKeyPressed(KEY_BACKSPACE))
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
		else if (IsKeyDown(KEY_BACKSPACE))
		{
			float DeltaTime = GetFrameTime();
			DelayTimer += DeltaTime;

			if (DelayTimer >= Delay)
			{
				// We Delete Characters depending on the Interval defined
				RepeatTimer += DeltaTime;
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

					// Reset the repeat timer for the next deletion.
					RepeatTimer = 0.0f;
				}
			}
		}
		else
		{
			DelayTimer = 0.0f;
			RepeatTimer = 0.0f;
		}

		// ########################
		// Deleting Logic Finished
		// ########################

		// ########################
		// Appending Characters Logic
		// ########################

		if (strlen(StringToHold) < MaxChars && PressedKey > 0)
		{
			if (MeasureText(StringToHold, FontSize) > Box.width && bContainsText)
			{
				if (bWordWrap)
				{
					/*
					* Basicially we just insert and resize an array here
					*/
						std::string TextBuffer;
						// Assigning all but the first space from the last time we did a space to a buffer
						TextBuffer.append(&StringToHold[LastSpaceIndex]);
						// then we do a Line Break where the last space was made
						StringToHold[LastSpaceIndex-1] = '\n';
						// Adapt the Lettercount accordingly (Was the letter count where we made the last letter - the delta of the lastSpaceIndex
						LetterCount = LastSpaceIndex;

						// then we add the stuff from the Textbuffer back
						for (size_t i = 0; i < TextBuffer.size(); i++)
						{
							StringToHold[LetterCount] = TextBuffer[i];
							LetterCount++;
						}

						// then we assign the key we actually pressed too, as we never added it up the actual string
						StringToHold[LetterCount] = (char)PressedKey;
						LetterCount++;
						StringToHold[LetterCount] = '\0';
						NumberOfLines++;
						std::cout << "After WordWrap: " << LetterCount << std::endl;
				}

				// Just Letter Wrap no WordWrap
				else
				{
					StringToHold[LetterCount] = '\n';
					StringToHold[LetterCount + 1] = (char)PressedKey;
					StringToHold[LetterCount + 2] = '\0';
					NumberOfLines++;
					LetterCount++;
				}
			}

			// Normal appending
			else
			{

				StringToHold[LetterCount] = (char)PressedKey;
				StringToHold[LetterCount + 1] = '\0';
				LetterCount++;
			}

			// Saving the Index of the Space for the next case
			if (PressedKey == 32)
			{
				BackupSpaceIndex = LastSpaceIndex;
				LastSpaceIndex = LetterCount;
			}
			
		}
	}

	if (bShouldCenter)
	{
		int LineCount = 1; // Starting with 1 since we always have atleast 1 line
		size_t pos = 0;

		std::string TempString(StringToHold);

		while ((pos = TempString.find('\n', pos)) != std::string::npos)
		{
			LineCount++;
			pos++;
		}

		// Measure the width of the text
		int textWidth = MeasureText(TempString.c_str(), FontSize);
		int LineHeight = FontSize * LineCount;

		// Assuming ButtonDim.x is the button's width, center the text in local space:
		float XPosition = (Box.width - textWidth) / 2.0f;
		float YPosition = (Box.height - LineHeight) / 2.0f;

		// Update the local X offset; Y remains unchanged
		TextOffset.x = XPosition;
		TextOffset.y = YPosition;

	}

	if(IsEditable)
	{
		DrawText(StringToHold, Box.x + TextOffset.x, Box.y + TextOffset.y, FontSize, FontColor);
	}
	
	else
	{
		DrawText(Text.c_str(), Box.x + TextOffset.x, Box.y + TextOffset.y, FontSize, FontColor);
	}
	
}
