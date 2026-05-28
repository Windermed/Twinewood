#include "BattleSequence.h"
#include "Core/Constants.h"

BattleSequence::BattleSequence(const CharacterTable& playerEntry, const CharacterTable& enemyEntry, const std::string& backgroundPath, float bgPosX, float bgPosY) : PlayerEntry(playerEntry), EnemyEntry(enemyEntry), Background(backgroundPath, bgPosX, bgPosY)
{
    // initialize HP from base values from character table.
    PlayerCurrentHP = playerEntry.GetBaseHP();
    PlayerMaxHP = playerEntry.GetBaseHP();
    EnemyCurrentHP = enemyEntry.GetBaseHP();
    EnemyMaxHP = enemyEntry.GetBaseHP();

    // initialize the player view in battle. this is mainly to do a camera shake when the player gets damaged.
    BattleView = View(FloatRect({ 0.f, 0.f }, { (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT }));
    EnemyBasePosition = { SCREEN_WIDTH / 2.0f, 350.0f };

    // initialize the check screen.
    std::vector<CharacterTable> CheckEntries = { enemyEntry };
    CheckScreen = new CharacterTableScreen(CheckEntries);

    // disable navigation through entries in battle. (temporary for now.)
    CheckScreen->SetAllowNavigation(false);
    // we'll empty the instructions text as battle will use it's own.
    CheckScreen->SetBackText("");
    CheckScreen->SetScreenState(TableScreenState::Detail);
    CheckScreen->SetShowInstructions(false);

    // shaders. here we go.
    if (Shader::isAvailable())
    {
        bShaderLoaded = WhiteFlashShader.loadFromFile("content/shaders/white_flash.frag", Shader::Type::Fragment);

        WhiteFlashShader.setUniform("texture", Shader::CurrentTexture);
    }

    EnemySprite.Load(enemyEntry.GetSpritePath());
    EnemySpriteOriginalColor = Color::White;
    EnemySprite.SetScale(7);
    EnemySprite.setOrigin({ 16.0f, 16.0f });

    EnemySprite.setPosition(EnemyBasePosition);

    //Message("Enemy sprite path: " << enemyEntry.GetSpritePath());

    float DialogueBoxX = 660.0f;
    float DialogueBoxY = 670.0f;

    DialogueBox.Load("content/textures/ui/battle/spr_dialogue_box.png");
    DialogueBox.setScale({ (SCREEN_WIDTH - 20.0f) / 188.0f, 4.0f });
    DialogueBox.setPosition({ DialogueBoxX, DialogueBoxY });

    PromptText = GameText(enemyEntry.GetDisplayName() + " attacks!", 28, Color::White, false);
    PromptText.setPosition({ DialogueBoxX + -450.0f, DialogueBoxY + -30.0f });
    PromptText.setCharacterSize(26);
    PromptText.SetFont(SMT2_FONT);

    PlayerNameText = GameText(playerEntry.GetDisplayName(), 24, Color::White, false);
    PlayerNameText.setPosition({ 150.0f, 820.0f });
    PlayerNameText.setScale({1.0f, 1.0f });
    PlayerNameText.SetFont(SMT1_FONT);

    PlayerHPText = GameText(" HP: " + std::to_string(PlayerCurrentHP) + " / " + std::to_string(PlayerMaxHP), 24, Color(80, 220, 80), false);
    PlayerHPText.setPosition({ 820.0f, 820.0f });
    PlayerHPText.SetFont(SMT1_FONT);

    EnemyHPText = GameText(enemyEntry.GetDisplayName() + "  HP: " + std::to_string(EnemyCurrentHP) + " / " + std::to_string(EnemyMaxHP), 22, Color(220, 80, 80), false);
    EnemyHPText.setPosition({ 480.0f, 40.0f });
    EnemyHPText.setCharacterSize(16);
    EnemyHPText.setScale({ 1.0f, 1.0f });
    EnemyHPText.SetFont(SMT1_FONT);

    DetermineFirstTurn();
}

void BattleSequence::DetermineFirstTurn()
{
    // depending on what's set to true or false in Constants. agility will be what determines who goes first.
    if (AGILITY_DETERMINES_TURN_ORDER)
    {
        TurnState = (PlayerEntry.GetStats().Agility >= EnemyEntry.GetStats().Agility) ? BattleTurnState::PlayerTurn : BattleTurnState::EnemyTurn;

    }
    else // if not, it's the player's turn.
    {
        TurnState = BattleTurnState::PlayerTurn;
    }

    bBattleStarted = false;

    // battle will always open with this.
    StartTextScroll(EnemyEntry.GetDisplayName() + " attacks!");
}

void BattleSequence::QueueMessage(const string& msg)
{
    MessageQueue.push_back(msg);
    if (!bShowingMessage)
    {
        bShowingMessage = true;
        MessageTimer = 0.0f;
        StartTextScroll(MessageQueue.front());
    }

}

void BattleSequence::AdvanceMessage()
{
    if (!MessageQueue.empty())
        MessageQueue.erase(MessageQueue.begin());

    if (!MessageQueue.empty())
    {
        MessageTimer = 0.f;
        StartTextScroll(MessageQueue.front());
    }
    else
    {
        bShowingMessage = false;
        MessageTimer = 0.f;
        bScrolling = false;

        if (bEnemyAttackPending)
        {
            bEnemyAttackPending = false;
            ExecuteEnemyAttack();
        }
        else if (!bBattleOver) // if battle isnt over, go back to the what will you do msg.
        {
            TurnState = BattleTurnState::PlayerTurn;
            StartTextScroll("What will you do?");
        }
    }
}

void BattleSequence::StartTextScroll(const string& message)
{
    FullPromptText = message;

    if (TEXT_SCROLLING_ENABLED)
    {
        DisplayedChars = 0;
        ScrollTimer = 0.f;
        bScrolling = true;
        PromptText.setString("");
    }
    else
    {
        DisplayedChars = (int)message.size();
        bScrolling = false;
        PromptText.setString(message);
    }
}

void BattleSequence::StartEnemyShake()
{
    //PlayBattleSound("snd_attack_impact_03.wav");
    bEnemyShaking = true;
    EnemyShakeTimer = 0.f;
    EnemyBasePosition = EnemySprite.getPosition();
}

void BattleSequence::StartEnemyFlash()
{
    bEnemySpriteFlashing = true;
    EnemyFlashTimer = 0.f;
    EnemyFlashTickTimer = 0.f;
    bEnemyFlashVisible = true;
}

void BattleSequence::UpdateEnemyFlash(float DeltaTime)
{
    if (!bEnemySpriteFlashing) return;

    EnemyFlashTimer += DeltaTime;
    EnemyFlashTickTimer += DeltaTime;

    if (EnemyFlashTickTimer >= 0.05f)
    {
        EnemyFlashTickTimer = 0.f;
        bEnemyFlashVisible = !bEnemyFlashVisible;

        // overlay becomes white when flashing but transparent if not.
        EnemySprite.setColor(bEnemyFlashVisible ? Color::White : EnemySpriteOriginalColor);

    }

    if (EnemyFlashTimer >= EnemyFlashDuration)
    {
        bEnemySpriteFlashing = false;
        // after flash we restore the sprite overlay back to it being transparent.
        EnemySprite.setColor(EnemySpriteOriginalColor);

        if (bBattleOver && bPlayerWon)
            bHideEnemySprite = true;
    }

}

void BattleSequence::StartScreenShake()
{
    bScreenShaking = true;
    ScreenShakeTimer = 0.f;
}

void BattleSequence::UpdateShakes(float DeltaTime)
{
    // enemy sprite shake
    if (bEnemyShaking)
    {
        EnemyShakeTimer += DeltaTime;
        if (EnemyShakeTimer >= EnemyShakeDuration)
        {
            bEnemyShaking = false;
            EnemySprite.setPosition(EnemyBasePosition);

            FloatRect SpriteBounds = EnemySprite.getGlobalBounds();
        }
        else
        {
            // oscillate left and right by using sin.
            float offset = std::sin(EnemyShakeTimer * 60.0f) * EnemyShakeMagnitude;
            EnemySprite.setPosition({ EnemyBasePosition.x + offset, EnemyBasePosition.y });
        }

        FloatRect SpriteBounds = EnemySprite.getGlobalBounds();

    }

    // for player screen shaking.
    if (bScreenShaking)
    {
        ScreenShakeTimer += DeltaTime;
        if (ScreenShakeTimer >= ScreenShakeDuration)
        {
            bScreenShaking = false;
            BattleView.setCenter({ (float)SCREEN_WIDTH / 2.f, (float)SCREEN_HEIGHT / 2.f });
        }
        else
        {
            float offsetX = ((rand() % 100) / 100.f - 0.5f) * ScreenShakeMagnitude * 2.f;
            float offsetY = ((rand() % 100) / 100.f - 0.5f) * ScreenShakeMagnitude * 2.f;
            BattleView.setCenter({ (float)SCREEN_WIDTH / 2.f + offsetX, (float)SCREEN_HEIGHT / 2.f + offsetY });
        }
    }
}
void BattleSequence::ExecutePlayerAttack()
{
    PlayBattleSound("snd_attack.wav");

    // calculating damage using the player's strength and the enemy's base defense.
    int damage = max(1, PlayerEntry.GetStats().Strength - EnemyEntry.GetBaseDefense());

    EnemyCurrentHP = max(0, EnemyCurrentHP - damage); // thats gonna hurt.
    EnemyHPText.setString(EnemyEntry.GetDisplayName() + "  HP: " + to_string(EnemyCurrentHP) + " / " + std::to_string(EnemyMaxHP));

    StartEnemyShake(); // when enemy gets hit, shake its sprite.
    StartEnemyFlash();

    CheckBattleEnd();

    if (!bBattleOver)
    {
        //TurnText.setString("ENEMY TURN"); // bro is cooked.
        QueueMessage(PlayerEntry.GetDisplayName() + " attacks \nfor " + to_string(damage) + " damage!");
        bEnemyAttackPending = true;
    }
}

void BattleSequence::ExecuteEnemyAttack()
{
    PlayBattleSound("snd_attack_impact_02.wav");

    // calculating damage using the enemy's strength stat and (gulp) the player's base defense.
    int damage = max(1, EnemyEntry.GetStats().Strength - PlayerEntry.GetBaseDefense());

    PlayerCurrentHP = max(0, PlayerCurrentHP - damage);

    PlayerHPText.setString("HP: " + to_string(PlayerCurrentHP) + " / " + to_string(PlayerMaxHP));

    QueueMessage(EnemyEntry.GetDisplayName() + " attacks \nfor " + to_string(damage) + " damage!");
    StartScreenShake(); // when player gets hit, shake the screen.

    CheckBattleEnd();

    if (!bBattleOver)
    {
        TurnState = BattleTurnState::PlayerTurn;
    }
}

void BattleSequence::CheckBattleEnd()
{
    if (EnemyCurrentHP <= 0)
    {
        PlayBattleSound("snd_death.wav");
        bBattleOver = true;
        bPlayerWon = true;
        StartEnemyFlash();
        QueueMessage(EnemyEntry.GetDisplayName() + " was defeated!"); // #packwatch
    }
    else if (PlayerCurrentHP <= 0)
    {
        PlayBattleSound("snd_death.wav");
        bBattleOver = true;
        bPlayerWon = false;
        QueueMessage("You were defeated...");  // meet potential man
    }
}

void BattleSequence::Update(float DeltaTime)
{
    UpdateShakes(DeltaTime);
    UpdateEnemyFlash(DeltaTime);

    if (bScrolling)
    {
        ScrollTimer += DeltaTime;
        if (ScrollTimer >= ScrollSpeed)
        {
            ScrollTimer = 0.0f;
            DisplayedChars++;

            if (DisplayedChars >= (int)FullPromptText.size())
            {
                DisplayedChars = (int)FullPromptText.size();
                bScrolling = false;
            }

            PromptText.setString(FullPromptText.substr(0, DisplayedChars));
        }

    }

    // only start the message auto advance timer once scrolling finishes.
    if (bShowingMessage && !bScrolling)
    {
        MessageTimer += DeltaTime;
        if (MessageTimer >= MessageDuration)
        {
            AdvanceMessage();
        }
    }

    // start the exit timer once dialogue is done and battle is over
    if (bBattleOver && !bShowingMessage && !bScrolling && !bExitPending)
    {
        bExitPending = true;
        ExitTimer = 0.f;
    }

    if (bExitPending)
    {
        ExitTimer += DeltaTime;
    }
}

void BattleSequence::OnKeyPressed(Keyboard::Key key)
{
    if (bBattleOver) return;

    // pressing X allows the player to skip text. (good if you want to avoid waiting to read the entire dialogue).
    if (bScrolling && key == Keyboard::Key::X)
    {
        DisplayedChars = (int)FullPromptText.size();
        PromptText.setString(FullPromptText);
        bScrolling = false;
        return;
    }

    // any key presses will dismiss the opening message.
    if (!bBattleStarted)
    {
        bBattleStarted = true;
        StartTextScroll("What will you do?");
        return;
    }

    // handle inputs while we're in check.
    if (bCheckMode)
    {
        if (key == Keyboard::Key::X)
            bCheckMode = false;
        return;
    }

    if (TurnState == BattleTurnState::PlayerTurn)
    {
        // block input while dialogue is displaying or enemy attack is queued
        if (bShowingMessage || bEnemyAttackPending) return;

        if (key == Keyboard::Key::Left)
        {
            Menu.MoveLeft();
        }
        if (key == Keyboard::Key::Right)
        {
            Menu.MoveRight();
        }

        if (key == Keyboard::Key::Enter || key == Keyboard::Key::Z)
        {
            switch (Menu.GetSelectedAction())
            {
            case BattleAction::Attack:
                ExecutePlayerAttack();
                break;
            case BattleAction::Check:
                bCheckMode = true;
                break;
            case BattleAction::Talk: // TEMP.
                QueueMessage("You tried to talk.. \nUnfortunately, you can't.\nnot yet at least.");
                break;
            case BattleAction::Escape: // TEMP.
                QueueMessage("Whoops gotta run!");
                bBattleOver = true;
                bHasEscaped = true;
                break;
            default:
                break;
            }
        }
    }
}

void BattleSequence::Draw(RenderWindow& window)
{
    window.setView(BattleView);
    Background.Draw(window);

    if (!bHideEnemySprite)
    {
        window.draw(EnemySprite);
    }

    // if sprite is flashing and visible. draw sprite again fully white on top.
    if (bEnemySpriteFlashing && bEnemyFlashVisible)
    {
        sf::RenderStates states;
        states.shader = &WhiteFlashShader;
        window.draw(EnemySprite, states);
    }

    window.setView(window.getDefaultView());

    if (bCheckMode)
    {
        // draw check screen fullscreen on top of our battle UI.
        if (CheckScreen)
        {
            CheckScreen->Draw(window);
        }

        GameText BackText("X: Return to battle", 16, Color(150, 150, 150), false);
        BackText.setPosition({ 40.f, (float)SCREEN_HEIGHT - 40.f });
        BackText.SetFont(SMT1_FONT);
        BackText.DrawText();

        return; // skip drawing the rest of the battle UI as its not needed.
    }

    EnemyHPText.DrawText();
    window.draw(DialogueBox);
    PromptText.DrawText();

    // action menu. this will only show when battle has started and it's the player's turn.
    if (bBattleStarted && !bBattleOver && TurnState == BattleTurnState::PlayerTurn && !bShowingMessage && !bEnemyAttackPending)
        Menu.Draw(window);

    PlayerNameText.DrawText();
    PlayerHPText.DrawText();

    
}