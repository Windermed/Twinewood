#pragma once
#include <SFML/Graphics.hpp>
#include "Textures/GameSprite.h"
#include "UI/GameText.h"

using namespace sf;

// could be expanded upon later on.
enum class BattleAction
{
    None,
    Attack,
    Check,
    Talk,
    Escape,
    Item
};

// button used for battle.
struct BattleButton
{

    void Load(const std::string& normalPath, const std::string& hoverPath)
    {
        NormalTexture.loadFromFile(normalPath);
        NormalSprite.setTexture(NormalTexture, true);

        HoverTexture.loadFromFile(hoverPath);
        HoverSprite.setTexture(HoverTexture, true);
    }

    void SetScale(float scale)
    {
        NormalSprite.setScale({ scale, scale });
        HoverSprite.setScale({ scale, scale });
    }

    void SetPosition(Vector2f pos)
    {
        Position = pos;
        NormalSprite.setPosition(pos);
        HoverSprite.setPosition(pos);
    }

    void Draw(RenderWindow& window)
    {
        window.draw(bIsSelected ? HoverSprite : NormalSprite);
    }

    sf::Texture NormalTexture;
    sf::Sprite NormalSprite = sf::Sprite(NormalTexture);

    sf::Texture HoverTexture;
    sf::Sprite HoverSprite = sf::Sprite(HoverTexture);

    Vector2f Position;
    BattleAction Action = BattleAction::None;
    bool bIsSelected = false;

};

class BattleMenu
{
public:
    BattleMenu();

    void Draw(RenderWindow& window);
    void MoveLeft();
    void MoveRight();

    BattleAction GetSelectedAction() { return Buttons[SelectedIndex]->Action; }
    int GetSelectedIndex()  const { return SelectedIndex; }

private:
    void MakeButton(BattleAction action, const std::string& normalPath, const std::string& hoverPath, float x);
    
private:
    static const std::string BASE_PATH;

    vector<BattleButton*> Buttons;
    int SelectedIndex = 0;

    static constexpr float ButtonScale = 4.0f;
    static constexpr float ButtonStartX = 230.0f;
    static constexpr float ButtonY = 530.0f;
    static constexpr float ButtonSpacing = 220.0f;


};