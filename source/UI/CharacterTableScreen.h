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

	// left panel
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


};