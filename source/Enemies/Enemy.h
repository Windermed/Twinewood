#pragma once
#include "Character/Character.h"

using namespace sf;
using namespace std;

// our base enemy class.
class Enemy : public Character
{
public:
	Enemy(Vector2f spawnPosition, float patrolDistance = 150);

	void Update(float DeltaTime) override;
	void Draw(RenderWindow& window) override;
	const char* GetName() const override { return "Enemy"; }

	FloatRect GetBounds() const { return CharacterSprite.getGlobalBounds(); }

	bool IsEngaged() const { return bIsEngaged; }
	void SetEngaged(bool engaged) { bIsEngaged = engaged; }

	void StartHitEffect()
	{
		Character::StartHitEffect();
		// store original tint to restore after flash
		OriginalColor = CharacterSprite.getColor();
	}

	void SetCharacterID(const std::string& id) { CharacterID = id; }
	void SetBattleBackground(const std::string& path) { BattleBackgroundPath = path; }

	string GetEnemyItemDefinition() const { return CharacterID; }
	string GetBattleBackground() const { return BattleBackgroundPath; }

protected:

	// default battle bg. we could change this later.
	string BattleBackgroundPath = "content/textures/battle/test_room_background.png";

	Color GetBaseColor() const override { return Color(255, 100, 100); }
	string CharacterID = "";

	float PatrolDistance = 150.0f;
	float PatrolOriginX = 0.0f;

	// go 1 for right, go -1 for left.
	float PatrolDirection = 1.0f;

	bool bIsEngaged = false;

	Color OriginalColor = Color(255, 100, 100);
};