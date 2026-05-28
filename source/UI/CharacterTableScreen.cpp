#include "CharacterTableScreen.h"
#include "Game/TwineEngine.h"

// layout constants JUST so we don't have to type them over and over again.

// ADJUST TO SCALE THE OVERALL UI SCALE
float CharacterTableScreen::UIScale = 1.5f;
float CharacterTableScreen::PortraitScale = 1.0f;
float CharacterTableScreen::NameScale = 1.0f;
float CharacterTableScreen::DescriptionScale = 1.0f;
float CharacterTableScreen::LevelScale = 1.0f;
float CharacterTableScreen::HPMPScale = 1.0f;
float CharacterTableScreen::StatBarScale = 2.0f;
float CharacterTableScreen::StatLabelScale = 0.7f;

CharacterTableScreen::CharacterTableScreen(const std::vector<CharacterTable>& entries) : Entries(entries)
{
	// build entry list
	for (int i = 0; i < (int)Entries.size(); i++)
	{
		GameText label(Entries[i].GetDisplayName(), 22, Color::White, false);
		EntryLabels.push_back(label);

	}

	RebuildDetailPanel();
}

void CharacterTableScreen::RebuildDetailPanel()
{
	StatBars.clear();
	if (Entries.empty()) return; // if no entries exist, we return.

	const CharacterTable& e = Entries[CursorIndex];
	CharacterStats Stats = e.GetStats();

	float PortraitSize = 16.0f * (UIScale * 16.0f * PortraitScale);
	float InfoFontSize = (unsigned int)(26.0f * UIScale * NameScale);
	float DescriptionFontSize = (unsigned int)(18.0f * UIScale * DescriptionScale);
	float LevelFontSize = (unsigned int)(20.0f * UIScale * LevelScale);
	float HPMPFontSize = (unsigned int)(20.0f * UIScale * HPMPScale);

	// set the scale size for our stat bar (or the tiles)
	StatBar::SetTileScale(UIScale * 2.0f * StatBarScale);
	StatBar::UIScale = UIScale * StatBarScale;

	float PortraitX = -10.0f;
	float PortraitY = 80.0f;

	float PortraitSpriteX = 40.0f;
	float PortraitSpriteY = 30.0f;

	// this text sits right of portrait
	float InfoTextX = PortraitX + PortraitSize + (60.0f * UIScale);

	float StatsY = PortraitY + PortraitSize + (80.f * UIScale);
	float StatSpacing = (12.0f * StatBar::UIScale) + (20.0f * UIScale);

	// portrait box.
	PortraitBGSprite.Load("content/textures/ui/spr_portrait_background.png");
	PortraitBGSprite.SetScale(UIScale * 10.0f);
	PortraitBGSprite.setOrigin({ 10.0f, 12.0f });
	PortraitBGSprite.setPosition({ PortraitX + PortraitSize / 2.0f, PortraitY + PortraitSize / 2.0f});

	// portrait sprite.
	PortraitSprite.Load(e.GetSpritePath());
	PortraitSprite.SetScale(UIScale * 5.0f);
	PortraitSprite.setOrigin({ 16.0f, 16.0f });
	PortraitSprite.setPosition({PortraitX + PortraitSize / 2.0f + PortraitSpriteX, PortraitY + PortraitSize / 2.0f + PortraitSpriteY});


	// info box text
	CharacterNameText = GameText(e.GetDisplayName(), InfoFontSize, Color::Yellow, false);
	CharacterDescriptionText = GameText(e.GetDescription(), DescriptionFontSize, Color::White, false);
	CharacterLevelText = GameText("Level: " + std::to_string(e.GetLevel()), LevelFontSize, Color::Cyan, false);
	CharacterHPText = GameText("HP:  " + std::to_string(e.GetBaseHP()) + " / " + std::to_string(e.GetBaseHP()), HPMPFontSize, Color(80, 220, 80), false);
	CharacterMPText = GameText("MP:  " + std::to_string(e.GetBaseMP()) + " / " + std::to_string(e.GetBaseMP()), HPMPFontSize, Color(80, 180, 220), false);

	CharacterNameText.setPosition({ InfoTextX, PortraitY });
	CharacterNameText.SetFont(SMT1_FONT);

	CharacterDescriptionText.setPosition({ InfoTextX, PortraitY + (55.0f * UIScale) });
	CharacterDescriptionText.SetFont(SMT2_FONT);
	CharacterDescriptionText.setScale({0.75, 0.75});

	CharacterLevelText.setPosition({ InfoTextX, PortraitY + (140.0f * UIScale) });
	CharacterLevelText.SetFont(SMT1_FONT);
	CharacterLevelText.setScale({ 0.75, 0.75 });

	CharacterHPText.setPosition({ InfoTextX, PortraitY + (180.0f * UIScale) });
	CharacterHPText.SetFont(SMT1_FONT);
	CharacterHPText.setScale({ 0.75, 0.75 });

	CharacterMPText.setPosition({ InfoTextX, PortraitY + (220.0f * UIScale) });
	CharacterMPText.SetFont(SMT1_FONT);
	CharacterMPText.setScale({ 0.75, 0.75 });
	
	float ScaledH = 12.0f * StatBar::UIScale;

	StatBars.reserve(5);

	StatBar::LabelFontScale = StatLabelScale;

	// stat bars
	struct StatDef { std::string label; int value; Color color; };
	std::vector<StatDef> statDefs =
	{
		// looks cleaner to read.
		{ "STR",	Stats.Strength,		Color(220, 80,  80)},
		{ "MAG",	Stats.Magic,		Color(180,  80, 255)},
		{ "VIT",	Stats.Vitality,		Color(80,  200, 80)},
		{ "AGI",	Stats.Agility,		Color(220, 220, 80)},
		{ "LU",		Stats.Luck,			Color(80,  220, 220)}
	};


	for (int i = 0; i < (int)statDefs.size(); i++)
	{
		StatBars.emplace_back(statDefs[i].label, statDefs[i].value, MAX_STAT_VALUE, Vector2f{ 80.0f, StatsY + i * StatSpacing });
		StatBars.back().SetColor(statDefs[i].color);
	}

	// sets up font for the text used before stat bar.
	for (StatBar& bar : StatBars)
		bar.SetTextFont(SMT1_FONT);

	// applies a different font for other text elements.
	//ApplyFont(SMT1_FONT);
}

void CharacterTableScreen::DrawDividerWithLabel(RenderWindow& window, float y, const std::string& label)
{
	// full right panel width
	float StartX = 80.f;
	float EndX = (float)SCREEN_WIDTH - 80.f;
	float TotalW = EndX - StartX;
	float CenterX = StartX + TotalW / 2.f;
	float LabelW = 60.f;
	float LineWidth = TotalW / 2.f - LabelW;

	// left line.
	RectangleShape LineL({ LineWidth, 2.f * UIScale });
	LineL.setFillColor(Color(80, 80, 80));
	LineL.setPosition({ StartX, y });
	window.draw(LineL);

	// the label set in center.
	GameText DivLabel(label, (unsigned int)(18.f * UIScale), Color(180, 180, 180), false);
	DivLabel.SetFont(SMT1_FONT);

	float LeftLineEnd = StartX + LineWidth;
	float RightLineStart = CenterX + LabelW;
	float GapCenter = LeftLineEnd + (RightLineStart - LeftLineEnd) / 2.f;

	// measure the text width after font is set
	FloatRect LabelBounds = DivLabel.getLocalBounds();
	float LabelHalfWidth = LabelBounds.position.x + LabelBounds.size.x / 2.f;

	// position so text center lands on CenterX
	DivLabel.setPosition({ GapCenter - LabelHalfWidth, y - LabelBounds.size.y });
	DivLabel.DrawText();

	// right line.
	RectangleShape LineR({ LineWidth, 2.f * UIScale });
	LineR.setFillColor(Color(80, 80, 80));
	LineR.setPosition({ CenterX + LabelW, y });
	window.draw(LineR);
}

void CharacterTableScreen::DrawSelectionScreen(RenderWindow& window)
{
	GameText Title("CHARACTER PREVIEW", 32, Color::Yellow, false);
	Title.setPosition({ SCREEN_WIDTH / 2.0f - 285.0f, 60.0f });
	Title.SetFont(SMT1_FONT);
	Title.DrawText();

	GameText Instructions("UP/DOWN: Navigate   Z: Select   SPACE: Exit", 20, Color(150, 150, 150), false);
	Instructions.setPosition({ SCREEN_WIDTH / 2.0f - 450.0f, 900.0f });
	Instructions.SetFont(SMT1_FONT);
	Instructions.DrawText();

	// for each entry.
	for (int i = 0; i < (int)EntryLabels.size(); i++)
	{
		float EntryX = SCREEN_WIDTH / 2.0f - 200.0f;
		float EntryY = 200.0f + i * 55.0f;

		if (i == CursorIndex)
		{
			RectangleShape Highlight({ 400.0f, 42.0f });
			Highlight.setFillColor(Color(40, 40, 60));
			Highlight.setPosition({ EntryX - 20.0f, EntryY - 2.0f });
			window.draw(Highlight);

			EntryLabels[i].setFillColor(Color::Yellow);

			GameText Cursor(">", 22, Color::Yellow, false);
			Cursor.setPosition({ EntryX - 36.0f, EntryY });
			Cursor.DrawText();
		}
		else
		{
			EntryLabels[i].setFillColor(Color::White);
		}

		EntryLabels[i].setPosition({ EntryX, EntryY });
		EntryLabels[i].SetFont(SMT2_FONT);
		EntryLabels[i].DrawText();

	}
}

void CharacterTableScreen::DrawDetailScreen(RenderWindow& window)
{
	// layout anchors. now uses full screen width.
	float PortraitSize = 16.0f * (UIScale * 16.0f);
	float PortraitX = 80.0f;
	float PortraitY = 80.0f;
	float InfoTextX = PortraitX + PortraitSize + 40.0f;
	float StatsY = PortraitY + PortraitSize + (80.0f * UIScale);

	// back instructions.
	if (bShowInstructions)
	{
		GameText BackText(CustomBackText, 16, Color(150, 150, 150), false);
		BackText.setPosition({ 40.f, (float)SCREEN_HEIGHT - 40.f });
		BackText.SetFont(SMT1_FONT);
		BackText.DrawText();
	}

	// draws portrait box and sprite.
	window.draw(PortraitBGSprite);
	window.draw(PortraitSprite);

	// character info
	CharacterNameText.DrawText();
	CharacterDescriptionText.DrawText();
	CharacterLevelText.DrawText();
	CharacterHPText.DrawText();
	CharacterMPText.DrawText();

	// stat divider and bars
	DrawDividerWithLabel(window, StatsY - 20.0f, "STATS");

	for (StatBar& bar : StatBars)
		bar.Draw(window);
}

// controls on the table screen.
void CharacterTableScreen::OnKeyPressed(Keyboard::Key key)
{
	// if on the selection screen.
	if (ScreenState == TableScreenState::Selection)
	{
		if (key == Keyboard::Key::Up)
		{
			if (CursorIndex > 0)
			{
				CursorIndex--;
				RebuildDetailPanel();
			}
		}
		else if (key == Keyboard::Key::Down)
		{
			if (CursorIndex < (int)Entries.size() - 1)
			{
				CursorIndex++;
				RebuildDetailPanel();
			}
		}
		else if (key == Keyboard::Key::Z)
		{
			RebuildDetailPanel();
			ScreenState = TableScreenState::Detail;
		}
	}
	else if (ScreenState == TableScreenState::Detail)
	{
		// pressing X goes back to the selection screen.
		// todo: if used in battles, we should ensure this depends on a flag.
		if (key == Keyboard::Key::X)
		{
			ScreenState = TableScreenState::Selection;
		}

		// allows the player to use left and right to navigate thru entries within the stat screen.
		if (bAllowNavigation)
		{
			if (key == Keyboard::Key::Left)
			{
				if (CursorIndex > 0) 
				{ 
					CursorIndex--; RebuildDetailPanel();
				}
			}
			else if (key == Keyboard::Key::Right)
			{
				if (CursorIndex < (int)Entries.size() - 1) 
				{ 
					CursorIndex++; RebuildDetailPanel();
				}
			}
		}
	}
	
}

void CharacterTableScreen::Draw(RenderWindow& window)
{
	// black background
	RectangleShape BG({ (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT });
	BG.setFillColor(Color(10, 10, 10));
	window.draw(BG);

	if (ScreenState == TableScreenState::Selection)
		DrawSelectionScreen(window);
	else
		DrawDetailScreen(window);
}