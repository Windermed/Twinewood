#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "UI/GameText.h"
#include "Textures/GameSprite.h"
#include "UI/StatBar.h"
#include "Tables/CharacterTable.h"

using namespace sf;

enum class TableScreenState
{
	Selection,
	Detail
};

// this is mainly to be used for testing. it displays the characters we have in our character table.
class CharacterTableScreen
{
public:
	CharacterTableScreen(const vector<CharacterTable>& entries);

	void Draw(RenderWindow& window);
	void OnKeyPressed(Keyboard::Key key);

public:
	void SetScreenState(TableScreenState state) { ScreenState = state; }
	void SetAllowNavigation(bool allow) { bAllowNavigation = allow; }
	void SetBackText(const std::string& text) { CustomBackText = text; }
	void SetShowInstructions(bool show) { bShowInstructions = show; }
private:
	void RebuildDetailPanel();
	void DrawDividerWithLabel(RenderWindow& window, float y, const string& label);

	void DrawSelectionScreen(RenderWindow& window);
	void DrawDetailScreen(RenderWindow& window);

	void ApplyFont(const std::string fontPath)
	{
		CharacterNameText.SetFont(fontPath);
		CharacterDescriptionText.SetFont(fontPath);
		CharacterLevelText.SetFont(fontPath);
		CharacterHPText.SetFont(fontPath);
		CharacterMPText.SetFont(fontPath);
	}

public:
	// the scale for the portrait. aka where sprites will appear. global.
	static float PortraitScale;

	TableScreenState ScreenState = TableScreenState::Selection;

	// change this to configure the size of ALL elements.
	static float UIScale;
	static float NameScale;
	static float DescriptionScale;
	static float LevelScale;
	static float HPMPScale;
	static float StatBarScale;
	static float StatLabelScale;

private:
	const vector<CharacterTable>& Entries;
	int CursorIndex = 0;

	RectangleShape PortraitBox;

	// character table instruction text.
	GameText TableInstructionsText;

	GameText TitleText;
	GameText InstructionsText;
	vector<GameText> EntryLabels;

	// right panel
	GameText CharacterNameText;
	GameText CharacterDescriptionText;
	GameText CharacterLevelText;
	GameText CharacterHPText;
	GameText CharacterMPText;
	GameText StatsTitleText;
	RectangleShape InfoBox;
	RectangleShape PanelDivider;

	// the sprite that appears in the stat screen.
	GameSprite PortraitSprite;

	// background sprite for the portrait.
	GameSprite PortraitBGSprite;

	// the stat bars. meant to go for the right panel
	vector<StatBar> StatBars;

	// whether or not we allow navigation thru the stat screen. (mainly just so that you cant do this in battle).
	bool bAllowNavigation = true;

	// show instructions for controls.
	bool bShowInstructions = true;

	string CustomBackText = "Left/Right: Scroll Through Entries                                 X: Back";


};