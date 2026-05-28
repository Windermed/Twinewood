#pragma once
#include <SFML/Graphics.hpp>
#include "Tables/CharacterTable.h"
#include "Sounds/SoundManager.h"
#include "UI/GameText.h"
#include "UI/CharacterTableScreen.h"
#include "Textures/GameSprite.h"
#include "Battle/BattleBackground.h"
#include "Battle/BattleMenu.h"

using namespace sf;
enum class BattleTurnState
{
    PlayerTurn,
    EnemyTurn
};

// the GIANT elephant in the room. introducing... the ACTUAL battle class.
// may my sleep recover from this.
class BattleSequence  // address me.
{
public:
    BattleSequence(const CharacterTable& playerEntry, const CharacterTable& enemyEntry, const string& backgroundPath, float bgPosX = 0.0f, float bgPosY = 0.0f);

    ~BattleSequence() { delete CheckScreen; }

    void Update(float DeltaTime);
    void Draw(RenderWindow& window);
    void OnKeyPressed(Keyboard::Key key);

    bool IsBattleOver() const { return bExitPending && ExitTimer >= ExitDelay; }
    bool IsPlayerWon() const { return bBattleOver && !bHasEscaped && bPlayerWon; }

    bool IsShowingMessage() const { return bShowingMessage; }
    bool IsScrolling() const { return bScrolling; }
private:

    void ExecutePlayerAttack();
    void ExecuteEnemyAttack();
    void CheckBattleEnd();
    void DetermineFirstTurn();

    void QueueMessage(const string& msg);
    void AdvanceMessage();
    void StartTextScroll(const string& message);

    // when enemy or a player gets attackd.
    void StartEnemyShake();
    void StartScreenShake();
    void UpdateShakes(float DeltaTime);

    void PlayBattleSound(const string& fileName, float volume = 100.f)
    {
        SoundManager::GetInstance().PlaySoundPooled(fileName, volume);
    }
    void StartEnemyFlash();
    void UpdateEnemyFlash(float DeltaTime);

private:
    CharacterTable PlayerEntry;
    CharacterTable EnemyEntry;

    // the HP and MP used during runtime. not to be confused with the values from character table.
    int PlayerCurrentHP = 0;
    int PlayerMaxHP = 0;
    int EnemyCurrentHP = 0;
    int EnemyMaxHP = 0;

    bool bBattleStarted = false;
    BattleTurnState TurnState = BattleTurnState::PlayerTurn;
    bool bBattleOver = false;
    bool bEnemyAttackPending = false;
    bool bHideEnemySprite = false;
    bool bPlayerWon = false;
    bool bHasEscaped = false;

    vector<string> MessageQueue;
    bool  bShowingMessage = false;
    float MessageTimer = 0.0f;
    float MessageDuration = 0.7f;

    sf::Shader WhiteFlashShader;
    bool bShaderLoaded = false;

    // UI elements
    BattleBackground Background;
    BattleMenu Menu;
    GameSprite EnemySprite;
    GameSprite DialogueBox;
    GameText PromptText;
    GameText PlayerNameText;
    GameText PlayerHPText;
    GameText EnemyHPText;

    Color EnemySpriteOriginalColor = Color::White;

    // enemy hit FX
    bool bEnemyShaking = false;
    float EnemyShakeTimer = 0.0f;
    float EnemyShakeDuration = 0.3f;
    float EnemyShakeMagnitude = 6.0f;
    Vector2f EnemyBasePosition;

    bool bEnemySpriteFlashing = false;
    float EnemyFlashTimer = 0.0f;
    float EnemyFlashDuration = 0.4f;
    float EnemyFlashTickTimer = 0.0f;
    bool bEnemyFlashVisible = true;

    // player hit — screen shake
    bool bScreenShaking = false;
    float ScreenShakeTimer = 0.0f;
    float ScreenShakeDuration = 0.3f;
    float ScreenShakeMagnitude = 8.0f;
    View BattleView;

    // text scrolling
    string FullPromptText;
    int DisplayedChars = 0;
    float ScrollTimer = 0.0f;
    float ScrollSpeed = 0.03f;
    bool bScrolling = false;

    float ExitTimer = 0.f;

    // seconds to wait after the defeat msg appears.
    float ExitDelay = 2.f;

    bool bExitPending = false;

    // for check, we will use a single entry list.
    bool bCheckMode = false;
    CharacterTableScreen* CheckScreen = nullptr;

};