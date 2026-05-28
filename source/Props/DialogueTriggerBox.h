#pragma once
#include <SFML/Graphics.hpp>
#include "Core/Object.h"
#include "Core/Interactable.h"
#include "UI/GameText.h"
#include <vector>
#include <string>

using namespace sf;
using namespace std;

//
class DialogueTriggerBox : public Object, public Interactable
{
public:
	DialogueTriggerBox(Vector2f position, std::vector<std::string> lines) : DialogueLines(lines)
	{
		Position = position;
		SetInteract(true);
	}

public:

	void Update(float DeltaTime) override {}
	void Draw(RenderWindow& window) override {}
	const char* GetName() const override { return "DialogueTrigger"; }
	// the size set to allow interactions.
	FloatRect GetBounds() const override
	{
		return FloatRect({ Position.x - 32.f, Position.y - 32.f }, { 64.f, 64.f });
	}

public:

	void OnInteract() override
	{
		if (bIsActive)
		{
			return;
		}

		bIsActive = true;
		CurrentLine = 0;
	}

	bool IsActive() const 
	{ 
		return bIsActive; 
	}

	bool IsFinished() const
	{
		return CurrentLine >= (int)DialogueLines.size();
	}

	void Dismiss()
	{
		CurrentLine++;
		if (IsFinished())
		{
			bIsActive = false;
		}
	}

	string GetCurrentLine() const
	{
		if (CurrentLine < (int)DialogueLines.size())
		{
			return DialogueLines[CurrentLine];
		}
		return "";
	}

private:
	vector<string> DialogueLines;
	int  CurrentLine = 0;
	bool bIsActive = false;
};