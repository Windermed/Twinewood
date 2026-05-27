#include "TwineEngine.h"
#include "Sounds/SoundManager.h"

void TwineEngine::Initialize()
{

	// set up camera
	Camera = View(FloatRect({ 0.f, 0.f }, { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }));

	// load the test room
	CurrentRoom = new BaseRoom("content/textures/rooms/first_test_room.png", 6);

	// set up player
	GamePlayer = new Player();

	// spawn player at the room's spawn point.
	CurrentRoom->SetSpawnPoint({ -200, 2110 });
	GamePlayer->SetPosition(CurrentRoom->GetSpawnPoint());

	// the camera will follow player
	Vector2f RoomSize = CurrentRoom->GetRoomSize();
	Vector2f RoomOffset = CurrentRoom->GetRoomOffset();

	// set movement bounds based on room.
	GamePlayer->SetMovementBounds(RoomOffset.x, RoomOffset.y, RoomOffset.x + RoomSize.x, RoomOffset.y + RoomSize.y);
	Camera.setCenter(GamePlayer->GetPosition());

	PlayerDebugText = GameText("", 24, Color::Yellow, false);
	PlayerDebugText.setPosition({ 10, 10 });

	Vector2f SpawnPoint = CurrentRoom->GetSpawnPoint();
	// to test, let's spawn a few enemies to our room and see what happens.
	Enemies.push_back(new Enemy({ SpawnPoint.x + 200.f, SpawnPoint.y }, 120.f));
	Enemies.push_back(new Enemy({ SpawnPoint.x + 500.f, SpawnPoint.y }, 200.f));
}

void TwineEngine::Input()
{
	// call base input.
	Engine::Input();
}

void TwineEngine::Update(float DeltaTime)
{
	Engine::Update(DeltaTime);

	if (!CurrentRoom || !GamePlayer) return;

	if (CurrentState == TwineGameMode::Overworld)
	{
		GamePlayer->Update(DeltaTime);

		for (Enemy* e : Enemies)
		{
			e->Update(DeltaTime);
		}

		CheckEnemyCollision();
	}

	// update debug text when our player moves
	PlayerDebugText.setString("X: " + to_string((int)GamePlayer->GetPosition().x) +
	"\nY: " + to_string((int)GamePlayer->GetPosition().y)
	+ "\nMode: " + (CurrentState == TwineGameMode::Battle ? "Battle" : "Overworld"));
	// line got too long so i had to move them down.
	

	// set the camera to our player.
	Camera.setCenter(GamePlayer->GetPosition());
}

void TwineEngine::Draw()
{
	Window.clear(Color::Black);

	// apply camera for world rendering
	Window.setView(Camera);

	if (CurrentRoom) CurrentRoom->Draw(Window);

	// draw every enemy on screen.
	for (Enemy* e : Enemies)
	{
		e->Draw(Window);
	}
		
	if (GamePlayer) GamePlayer->Draw(Window);

	// switch back to default view for HUD
	Window.setView(Window.getDefaultView());

	PlayerDebugText.DrawText();

	Window.display();
}

void TwineEngine::CheckEnemyCollision()
{
	FloatRect PlayerBounds = GamePlayer->GetSprite().getGlobalBounds();

	for (Enemy* e : Enemies)
	{
		if (e->IsEngaged()) continue;
		if (!e->HasCollision()) continue; // we skip any enemies that disable collision.

		if (PlayerBounds.findIntersection(e->GetBounds()))
		{
			ResolveEnemyCollision(e);

			if (CurrentState != TwineGameMode::Battle)
			{
				EnterBattle(e);
			}

			return;
		}
	}
}

void TwineEngine::ResolveEnemyCollision(Enemy* enemy)
{
	FloatRect PB = GamePlayer->GetSprite().getGlobalBounds();
	FloatRect EB = enemy->GetBounds();

	// find an overlap on each axis
	float OverlapX = (PB.position.x + PB.size.x / 2.f) - (EB.position.x + EB.size.x / 2.f);
	float OverlapY = (PB.position.y + PB.size.y / 2.f) - (EB.position.y + EB.size.y / 2.f);

	float HalfWidths = (PB.size.x + EB.size.x) / 2.f;
	float HalfHeights = (PB.size.y + EB.size.y) / 2.f;

	float interactX = HalfWidths - std::abs(OverlapX);
	float interactY = HalfHeights - std::abs(OverlapY);

	Vector2f PlayerPos = GamePlayer->GetPosition();

	// push it out along the axis with the smallest interaction
	if (interactX < interactY)
	{
		PlayerPos.x += (OverlapX > 0.f ? interactX : -interactX);
	}
	else
	{
		PlayerPos.y += (OverlapY > 0.f ? interactY : -interactY);
	}

	GamePlayer->SetPosition(PlayerPos);
		
}

void TwineEngine::EnterBattle(Enemy* enemy)
{
	//enemy->SetEngaged(true);
	//CurrentState = TwineGameMode::Battle;
	//GamePlayer->SetInputLocked(true);

	// TODO: BATTLES.

}
