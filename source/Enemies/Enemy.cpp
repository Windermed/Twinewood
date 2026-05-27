#include "Enemy.h"

Enemy::Enemy(Vector2f spawnPosition, float patrolDistance) : PatrolDistance(patrolDistance)
{
	Position = spawnPosition;
	PatrolOriginX = spawnPosition.x;

	CharacterSprite.Load("content/textures/player/character_base_16x16_temp.png");
	CharacterSprite.setPosition(Position);
	CharacterSprite.SetScale(6);
	CharacterSprite.setTextureRect(IntRect({ 0, 0 }, { 16, 16 }));
	CharacterSprite.setOrigin({ 8.0f, 8.0f });

	// tints it red so that they can be visually different from player.
	CharacterSprite.setColor(Color(255, 100, 100));
}

void Enemy::Update(float DeltaTime)
{
	if (bIsEngaged)
		return;

	// patrol

	Position.x += Speed * PatrolDirection * DeltaTime;

	if (Position.x > PatrolOriginX + PatrolDistance)
	{
		Position.x = PatrolOriginX + PatrolDistance;
		PatrolDirection = -1.0f;

	}
	else if (Position.x < PatrolOriginX - PatrolDistance)
	{
		Position.x = PatrolOriginX - PatrolDistance;
		PatrolDirection = 1.0f;
	}

	CharacterSprite.setPosition(Position);
}

void Enemy::Draw(RenderWindow& window)
{
	if (bIsVisible)
		window.draw(CharacterSprite);
}