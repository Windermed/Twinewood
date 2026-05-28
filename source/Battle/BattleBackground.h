#pragma once
#include <SFML/Graphics.hpp>
#include "Textures/GameSprite.h"

using namespace sf;

// background used in battles.
class BattleBackground
{
public:
    BattleBackground(const std::string& spritePath = "", float posX = 0.0f, float posY = 0.0f)
    {
        BGShape.setSize({ (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT });
        BGShape.setFillColor(Color(20, 24, 40));

        if (!spritePath.empty())
        {
            BGTexture.loadFromFile(spritePath);
            BGSprite.setTexture(BGTexture, true);
            BGSprite.setScale({ 2.0f, 2.16f });
            BGSprite.setOrigin({ 0.0f, 0.0f });
            BGSprite.setPosition({ posX, posY });
            bHasSprite = true;
        }
    }

    void Draw(RenderWindow& window)
    {
        if (bHasSprite)
        {
            window.draw(BGSprite);
        }
        else
        {
            window.draw(BGShape);
        }
    }

private:
    bool bHasSprite = false;
    GameSprite BGSprite;
    RectangleShape BGShape;
    Texture BGTexture;
};