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
	CurrentRoom->SetSpawnPoint({ -300, 2050 });
	GamePlayer->SetPosition(CurrentRoom->GetSpawnPoint());

	// the camera will follow player
	Vector2f RoomSize = CurrentRoom->GetRoomSize();
	Vector2f RoomOffset = CurrentRoom->GetRoomOffset();

	// set movement bounds based on room.
	GamePlayer->SetMovementBounds(RoomOffset.x, RoomOffset.y, RoomOffset.x + RoomSize.x, RoomOffset.y + RoomSize.y);
	Camera.setCenter(GamePlayer->GetPosition());

	// setup dialogue box sprite.
	OverworldDialogueBox.Load("content/textures/ui/battle/spr_dialogue_box.png");
	OverworldDialogueBox.setScale({ (SCREEN_WIDTH - 20.f) / 170.f, 4.f });
	OverworldDialogueBox.setPosition({ 600.f, (float)SCREEN_HEIGHT - 220.f });

	OverworldDialogueText = GameText("", 22, Color::White, false);
	OverworldDialogueText.setPosition({ 70.f, (float)SCREEN_HEIGHT - 290.f });
	OverworldDialogueText.SetFont(SMT2_FONT);

	
	Vector2f SpawnPoint = CurrentRoom->GetSpawnPoint();
	// intended coords are 1904.0f, and -139.0f.
	//DialogueTriggerBox* StairTriggerBox = new DialogueTriggerBox({ 1904.0f, -139.0f},

	// FOR TESTING
	//DialogueTriggerBox* StairTriggerBox = new DialogueTriggerBox({ SpawnPoint.x + 100.0f, SpawnPoint.y + 139.0f },

	DialogueTriggerBox* StairTriggerBox = new DialogueTriggerBox({ 1904.0f, -139.0f },
	{
		"This looks like it leads somewhere..",
		"for some reason though, you get this\nweird feeling.",
		"a feeling that you shouldn't go in\njust yet.",
		"...",
		"you wonder if this area is unfinished\nand not ready to be seen.",
		"...",
		"maybe i should wait until this is ready."
	});


	// 3052, -1452
	//DialogueTriggerBox* EndTriggerBox = new DialogueTriggerBox({ 3052, -1452 },
	DialogueTriggerBox* EndTriggerBox = new DialogueTriggerBox({ 3052, -1452 },
	{
		"You tried slowly to open the door\nto see that's inside...",
		"?",
		"It's... locked?",
		"within the boss, he dropped a note.",
		"This note reads:",
		"EVEN IF YOU DEFEAT ME, YOU CAN'T GO\nINSIDE JUST YET.",
		"COME BACK ANOTHER TIME.\nANOTHER DAY EVEN.",
		"IF YOU DO. THERE MIGHT BE\n SOMETHING ELSE WAITING.",
		"..."
		"in other words\nit's the end of the demo!"
	});

	DialogueTriggers.push_back(StairTriggerBox);
	DialogueTriggers.push_back(EndTriggerBox);
	Interactables.push_back(StairTriggerBox);
	Interactables.push_back(EndTriggerBox);

	// debug text that prints out coords..
	PlayerDebugText = GameText("", 24, Color::Yellow, false);
	PlayerDebugText.setPosition({ 10, 10 });
	PlayerDebugText.setCharacterSize(20);
	PlayerDebugText.SetFont(SMT1_FONT);

	Enemies.push_back(new Enemy({ -400.f, 142.f }, 150.f));
	Enemies[0]->SetCharacterID("Character_Test02");

	Enemies.push_back(new Enemy({ -600.f, 142.f }, 150.f));
	Enemies[1]->SetCharacterID("Character_Test02");

	// boss enemy.
	Enemies.push_back(new Enemy({ 3052, -1452 }, 0.0f));
	Enemies[2]->SetCharacterID("Character_Boss");
	Enemies[2]->SetBattleBackground("content/textures/battle/spr_test_room_background_boss.png");

	RegisterObject(GamePlayer);
	RegisterObject(StairTriggerBox);
	RegisterObject(EndTriggerBox);
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

	//Message("Probe active, time to check for " << Interactables.size() << " interactables");

	for (Interactable* obj : Interactables)
	{
		if (!obj->CanInteract()) continue;

		// every interactable is also an object so we should cast to get bounds.
		Object* interactObj = dynamic_cast<Object*>(obj);
		if (!interactObj) continue;

		Message("Probe bounds are: " << Probe.GetBounds().position.x << ", " << Probe.GetBounds().position.y << " size: " << Probe.GetBounds().size.x << ", " << Probe.GetBounds().size.y);

		Message("Trigger bounds: " << interactObj->GetBounds().position.x << ", " << interactObj->GetBounds().position.y << " size: " << interactObj->GetBounds().size.x << ", " << interactObj->GetBounds().size.y);

		if (Probe.GetBounds().findIntersection(interactObj->GetBounds()))
		{
			Message("Interaction triggered!");
			obj->OnInteract();

			// checks if we are hitting a dialogue trigger
			DialogueTriggerBox* trigger = dynamic_cast<DialogueTriggerBox*>(obj);
			if (trigger && trigger->IsActive())
			{
				ActiveDialogue = trigger;
				bDialogueActive = true;
				StartDialogueScroll(trigger->GetCurrentLine());
			}
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

void TwineEngine::DrawOverworldDialogue()
{
	if (!bDialogueActive || !ActiveDialogue) return;

	Window.draw(OverworldDialogueBox);
	OverworldDialogueText.DrawText();
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

		// text scroll update that runs regardless.
		if (bOverworldScrolling)
		{
			OverworldScrollTimer += DeltaTime;
			if (OverworldScrollTimer >= OverworldScrollSpeed)
			{
				OverworldScrollTimer = 0.f;
				OverworldDisplayedChars++;
				if (OverworldDisplayedChars >= (int)OverworldFullText.size())
				{
					OverworldDisplayedChars = (int)OverworldFullText.size();
					bOverworldScrolling = false;

				}
				else
				{
					// allows for SFX to play for each char that isnt a space.
					char c = OverworldFullText[OverworldDisplayedChars - 1];
					if (c != ' ' && c != '\n')
					{
						Engine::PlaySound("snd_scroll_text.wav", 30.f);
					}
				}
				OverworldDialogueText.setString(OverworldFullText.substr(0, OverworldDisplayedChars));
			}
		}

		// block movement and enemy updates during dialogue
		if (!bDialogueActive)
		{
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

			// FOR COLLISION.
			bool bBlocked = !CurrentRoom->IsWalkable({ Left, Top }) || !CurrentRoom->IsWalkable({ Right, Top }) ||
			!CurrentRoom->IsWalkable({ Left, Bottom }) ||
			!CurrentRoom->IsWalkable({ Right, Bottom }) ||
			!CurrentRoom->IsWalkable({ MidX, Top }) ||
			!CurrentRoom->IsWalkable({ MidX, Bottom }) ||
			!CurrentRoom->IsWalkable({ Left, MidY }) ||
			!CurrentRoom->IsWalkable({ Right, MidY });

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
	}

	if (CurrentState == TwineGameMode::Battle)
	{
		if (CurrentBattle)
		{
			CurrentBattle->Update(DeltaTime);

			if (CurrentBattle->IsBattleOver())
			{
				// only get rid of the enemy assuming the player has won and not escaped (like a fraud)
				if (CurrentBattle->IsPlayerWon())
				{
					for (Enemy* e : Enemies)
					{
						if (e->IsEngaged())
						{
							e->SetVisible(false);
						}
					}
				}
				else
				{
					// since the player escaped. the enemy will be re-enabled so that it can be fought again.
					for (Enemy* e : Enemies)
					{
						if (e->IsEngaged())
						{
							e->SetEngaged(false);
						}
					}
				}

				// we return to the overworld.
				delete CurrentBattle;
				CurrentBattle = nullptr;
				CurrentState = TwineGameMode::Overworld;
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
	
	if (CurrentState != TwineGameMode::Battle)
	{
		Window.setView(Camera);
		if (CurrentRoom) CurrentRoom->Draw(Window);
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

	Window.setView(Window.getDefaultView());

	if (CurrentState == TwineGameMode::Battle)
	{
		if (CurrentBattle)
		{
			CurrentBattle->Draw(Window);
		}
	}
	else if (CurrentState == TwineGameMode::CharacterTableTest)
	{
		if (CharTableScreen)
		{
			CharTableScreen->Draw(Window);
		}
	}

	DrawOverworldDialogue();
	PlayerDebugText.DrawText();
	Window.display();
}


// TODO: MASSIVE CLEANUP.
void TwineEngine::OnKeyPressed(Keyboard::Key key)
{
	Message("From TwineEngine, Key Pressed: " << (int)key);

	// for sfx related to UI.
	// i'm aware of how sloppy this is. I will look into doing a more proper way of handling SFX in the future.
	if (CurrentState == TwineGameMode::Battle || CurrentState == TwineGameMode::CharacterTableTest)
	{
		// to avoid the UI SFX from playing while dialogue is running.
		bool bBattleMessaging = CurrentState == TwineGameMode::Battle && CurrentBattle && (CurrentBattle->IsShowingMessage() || CurrentBattle->IsScrolling());

		// avoids UI sound from playing even when pressing X.
		bool bSkippingScroll = key == Keyboard::Key::X && CurrentBattle && CurrentBattle->IsScrolling();

		if (!bBattleMessaging)
		{
			if (key == Keyboard::Key::Left || key == Keyboard::Key::Right || key == Keyboard::Key::Up || key == Keyboard::Key::Down || key == Keyboard::Key::Backspace)
			{
				Engine::PlaySound("snd_ui_select.wav", 15.0f);
			}

			if (key == Keyboard::Key::Enter || key == Keyboard::Key::Z)
			{
				Engine::PlaySound("snd_ui_select_confirm.wav", 45.0f);
			}
		}

	}

	if (CurrentState == TwineGameMode::Overworld)
	{
		// for dialogue triggers.
		if (bDialogueActive && ActiveDialogue)
		{
			if (key == Keyboard::Key::Z || key == Keyboard::Key::Enter || key == Keyboard::Key::X)
			{
				if (bOverworldScrolling)
				{
					bOverworldScrolling = false;
					OverworldDialogueText.setString(OverworldFullText);
					return;
				}

				ActiveDialogue->Dismiss();
				Message("IsFinished is now: " << ActiveDialogue->IsFinished());

				if (ActiveDialogue->IsFinished())
				{
					Message("Dialogue has finished!!");
					bDialogueActive = false;
					ActiveDialogue = nullptr;
					bOverworldScrolling = false;
					OverworldDialogueText.setString("");
				}
				else
				{
					StartDialogueScroll(ActiveDialogue->GetCurrentLine());
				}
				return;
			}
		}

		// for inteacting.
		if (key == Keyboard::Key::Z)
		{
			Message("Z pressed, spawning probe!");
			bInteractPressed = true;
		}
		
	}

	// pass all inputs to battle.
	if (CurrentState == TwineGameMode::Battle)
	{
		if (CurrentBattle)
			CurrentBattle->OnKeyPressed(key);
		return;
	}

	// pass all inputs to the character table test room.
	if (CurrentState == TwineGameMode::CharacterTableTest)
	{

		if (CharTableScreen) 
			CharTableScreen->OnKeyPressed(key);

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

	float interactX = HalfWidths - abs(OverlapX);
	float interactY = HalfHeights - abs(OverlapY);

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
	SoundManager::GetInstance().PlaySound("snd_battle_transition.wav");
	enemy->SetEngaged(true);
	CurrentState = TwineGameMode::Battle;
	GamePlayer->SetInputLocked(true);
	
	// player is always the first in the table's json.
	CharacterTable PlayerEntry = CharacterEntries[0];
	CharacterTable EnemyEntry = CharacterEntries[1]; // just in case lol.

	// find enemy entry by it's item definition.
	for (auto& entry : CharacterEntries)
	{
		if (entry.GetItemDefinition() == enemy->GetEnemyItemDefinition())
		{
			EnemyEntry = entry;
			break;
		}
	}

	// clean up any previous battles if they remain.
	if (CurrentBattle) { delete CurrentBattle; CurrentBattle = nullptr; }

	// start up a battle.
	CurrentBattle = new BattleSequence(PlayerEntry, EnemyEntry, enemy->GetBattleBackground(), 5, 55);
}
