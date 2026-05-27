#pragma once
#include "Engine/Engine.h"
#include "Core/Interactable.h"
#include "Core/InteractProbe.h"
#include "Tables/CharacterTable.h"
#include "UI/CharacterTableScreen.h"
#include "Rooms/BaseRoom.h"
#include "Player/Player.h"
#include "Enemies/Enemy.h"
#include "Battle/BattleSequence.h"
#include <vector>

enum class TwineGameMode
{
	CharacterTableTest, // temp.
	Overworld,
	Battle
};
// base game engine class - this should be renamed and used for your own project.
class TwineEngine : public Engine
{
public:
	TwineEngine() {}

	// initialize the engine instance.
	static void Init()
	{
		if (!EngineInstance)
		{
			EngineInstance = new TwineEngine();

			// initialize after the instance is set.
			static_cast<TwineEngine*>(EngineInstance)->Initialize();
		}
	}

protected:

	/* replacement for the default constructor. just call this on Init() */
	void Initialize();

	void Input() override;
	void Update(float DeltaTime) override;
	void Draw() override;

	void OnKeyPressed(Keyboard::Key key) override;

	const char* GetName() const override { return "TwineEngine"; }

private:
	void CheckEnemyCollision();
	void ResolveEnemyCollision(Enemy* enemy);
	void EnterBattle(Enemy* enemy);

	void CheckInteraction();
	void DrawProbeDebug();

private:
	// demo sprites
	GameSprite DemoSprite;
	float RotationAngle = 0.0f;

	// demo text.
	GameText TitleText;
	GameText SubtitleText;
	GameText TeaserText;

	BaseRoom* CurrentRoom = nullptr;
	Player* GamePlayer = nullptr; // in the future, lets fix this in engine so that this isnt necessary.
	View Camera;

	GameText PlayerDebugText;

	// enemies.
	vector<Enemy*> Enemies;
	TwineGameMode CurrentState = TwineGameMode::CharacterTableTest;

	// battle.
	BattleSequence* CurrentBattle = nullptr;

	// character entries.
	std::vector<CharacterTable> CharacterEntries;
	CharacterTableScreen* CharTableScreen = nullptr;

	// interaction
	InteractProbe Probe;
	vector<Interactable*> Interactables;

	// key binds.
	// in the future, we should make these keys configurable.
	bool bInteractPressed = false;

	// probe
	float ProbeTimer = 0.f;

};