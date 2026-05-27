#include "TwineEngine.h"
#include "Sounds/SoundManager.h"

void TwineEngine::Initialize()
{

	// set up camera
	Camera = View(FloatRect({ 0.f, 0.f }, { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }));

	// load our character table
	CharacterEntries = CharacterTable::LoadFromFile("content/tables/character_table.json");
	for (auto& entry : CharacterEntries)
	{
		Message("The following character: " << entry.GetDisplayName() << " Has loaded!" << endl)
	}

	// initialize the character table screen using the entries.
	CharTableScreen = new CharacterTableScreen(CharacterEntries);

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

	RegisterObject(GamePlayer);
	for (Enemy* e : Enemies)
	{
		RegisterObject(e);
	}
}

void TwineEngine::Input()
{
	// call base input.
	Engine::Input();
}

void TwineEngine::CheckInteraction()
{
	if (!Probe.IsActive()) return;

	for (Interactable* obj : Interactables)
	{
		if (!obj->CanInteract()) continue;

		// every interactable is also an object so we should cast to get bounds.
		Object* interactObj = dynamic_cast<Object*>(obj);
		if (!interactObj) continue;

		if (Probe.GetBounds().findIntersection(interactObj->GetBounds()))
		{
			obj->OnInteract();
		}
	}

}

void TwineEngine::DrawProbeDebug()
{
	if (!Probe.IsActive()) return;

	FloatRect PB = Probe.GetBounds();
	RectangleShape ProbeRect(PB.size);

	ProbeRect.setPosition(PB.position);
	ProbeRect.setFillColor(Color(255, 255, 0, 60));
	ProbeRect.setOutlineColor(Color::Yellow);
	ProbeRect.setOutlineThickness(2.f);

	Window.draw(ProbeRect);
}

void TwineEngine::Update(float DeltaTime)
{
	Engine::Update(DeltaTime);

	if (!CurrentRoom || !GamePlayer) return;

	// dont update if we are on the character table screen.
	if (CurrentState == TwineGameMode::CharacterTableTest) return;
	

	if (CurrentState == TwineGameMode::Overworld)
	{
		// count down probe timer and clear when done.
		if (Probe.IsActive())
		{
			ProbeTimer -= DeltaTime;
			if (ProbeTimer <= 0.f)
				Probe.Clear();
		}

		// spawn probe this frame if Z was pressed
		if (bInteractPressed)
		{
			Probe.Spawn(GamePlayer->GetPosition(), GamePlayer->GetBounds(), GamePlayer->GetFacingDirection());
			ProbeTimer = 0.060f;
			bInteractPressed = false;
		}

		// save player position before movement.
		Vector2f LastValidPos = GamePlayer->GetPosition();

		GamePlayer->Update(DeltaTime);

		// sample all 8 points around the collision bounds
		FloatRect Bounds = GamePlayer->GetBounds();

		float Left = Bounds.position.x;
		float Right = Bounds.position.x + Bounds.size.x;
		float Top = Bounds.position.y;
		float Bottom = Bounds.position.y + Bounds.size.y;
		float MidX = Left + Bounds.size.x / 2.0f;
		float MidY = Top + Bounds.size.y / 2.0f;

		bool bBlocked = !CurrentRoom->IsWalkable({Left, Top}) || !CurrentRoom->IsWalkable({ Right, Top }) ||
		!CurrentRoom->IsWalkable({Left, Bottom}) ||
		!CurrentRoom->IsWalkable({Right, Bottom}) ||
		!CurrentRoom->IsWalkable({MidX, Top}) ||
		!CurrentRoom->IsWalkable({MidX, Bottom}) ||
		!CurrentRoom->IsWalkable({Left, MidY}) ||
		!CurrentRoom->IsWalkable({Right, MidY});

		if (bBlocked)
		{
			GamePlayer->SetPosition(LastValidPos);
		}

		for (Enemy* e : Enemies)
		{
			e->Update(DeltaTime);
		}

		CheckEnemyCollision();
		CheckInteraction();

	}

	if (CurrentState == TwineGameMode::Battle)
	{
		if (CurrentBattle)
		{
			CurrentBattle->Update(DeltaTime);

			// if battle is over and the player has won.
			if (CurrentBattle->IsBattleOver() && CurrentBattle->IsPlayerWon())
			{
				// we return to the overworld.
				delete CurrentBattle;
				CurrentBattle = nullptr; // reset.
				CurrentState = TwineGaemMode::Overworld;
				GamePlayer->SetInputLocked(false);
			}
		}
	}

	// update debug text when our player moves
	PlayerDebugText.setString("X: " + to_string((int)GamePlayer->GetPosition().x) +
	"\nY: " + to_string((int)GamePlayer->GetPosition().y) + "\nMode: " + (CurrentState == TwineGameMode::Battle ? "Battle" : "Overworld"));

	// set the camera to our player.
	Camera.setCenter(GamePlayer->GetPosition());

}

void TwineEngine::Draw()
{
	Window.clear(Color::Black);

	
	// draw battle.
	if (CurrentState == TwineGameMode::Battle)
	{
		Window.setView(Window.getDefaultView());
		if (CurrentBattle) CurrentBattle->Draw(Window);
	}

	// only draw world if we are in overworld or battle.
	if (CurrentState != TwineGameMode::CharacterTableTest)
	{
		// apply camera for world rendering
		Window.setView(Camera);

		if (CurrentRoom) CurrentRoom->Draw(Window);
		// draw every enemy on screen.
		for (Enemy* e : Enemies) e->Draw(Window);
		if (GamePlayer) GamePlayer->Draw(Window);

		if (bShowCollisionDebug)
		{
			DrawCollisionDebug();

			if (Probe.IsActive())
			{
				FloatRect PB = Probe.GetBounds();
				RectangleShape ProbeRect(PB.size);
				ProbeRect.setPosition(PB.position);
				ProbeRect.setFillColor(Color::Yellow);
				Window.draw(ProbeRect);
			}
		}
	}

	// switch back to default view for HUD
	Window.setView(Window.getDefaultView());

	if (CurrentState == TwineGameMode::CharacterTableTest)
	{
		if (CharTableScreen) CharTableScreen->Draw(Window);
	}



	PlayerDebugText.DrawText();

	Window.display();
}

void TwineEngine::OnKeyPressed(Keyboard::Key key)
{
	Message("From TwineEngine, Key Pressed: " << (int)key);
	if (key == Keyboard::Key::Z)
	{
		Message("Z pressed, spawning probe!");
		bInteractPressed = true;
	}

	// pass all inputs to battle.
	if (CurrentState == TwineGameMode::Battle)
	{
		if (CurrentBattle) CurrentBattle->OnKeyPressed(key);
		return;
	}

	// pass all inputs to the character table test room.
	if (CurrentState == TwineGameMode::CharacterTableTest)
	{
		if (CharTableScreen) CharTableScreen->OnKeyPressed(key);
		if (key == Keyboard::Key::Space)
			CurrentState = TwineGameMode::Overworld;
		return;
	}
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
	enemy->SetEngaged(true);
	CurrentState = TwineGameMode::Battle;
	GamePlayer->SetInputLocked(true);

	
	CharacterTable PlayerEntry = CharacterEntries[0];

	// for now, we will only do one enemy.
	CharacterTable EnemyEntry = CharacterEntries[1];

	// clean up any previous battles if they remain.
	if (CurrentBattle) { delete CurrentBattle; CurrentBattle = nullptr; }

	CurrentBattle = new BattleSequence(PlayerEntry, EnemyEntry);

}
