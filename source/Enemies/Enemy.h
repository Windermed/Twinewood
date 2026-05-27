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

private:
	float PatrolDistance = 150.0f;
	float PatrolOriginX = 0.0f;

	// go 1 for right, go -1 for left.
	float PatrolDirection = 1.0f;

	bool bIsEngaged = false;
};