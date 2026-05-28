#include "BattleMenu.h"

const std::string BattleMenu::BASE_PATH = "content/textures/ui/battle/";

void BattleMenu::MakeButton(BattleAction action, const std::string& normalPath, const std::string& hoverPath, float x)
{
    BattleButton* btn = new BattleButton();

    btn->Action = action;
    btn->Load(BASE_PATH + normalPath, BASE_PATH + hoverPath);
    btn->SetScale(ButtonScale);
    btn->SetPosition({ x, ButtonY });

    Buttons.push_back(btn);

    //bool normalLoaded = btn.NormalSprite.Load(BASE_PATH + normalPath);
    //bool hoverLoaded = btn.HoverSprite.Load(BASE_PATH + hoverPath);

    //Message("Button " << normalPath << " loaded: " << normalLoaded << " / " << hoverLoaded);
}
BattleMenu::BattleMenu()
{
    Buttons.reserve(4);
    // sets up button to use in battle.

    MakeButton(BattleAction::Attack, "spr_attack_button.png", "spr_attack_button_hover.png", ButtonStartX);
    MakeButton(BattleAction::Check, "spr_check_button.png", "spr_check_button_hover.png", ButtonStartX + ButtonSpacing);
    MakeButton(BattleAction::Talk, "spr_talk_button.png", "spr_talk_button_hover.png", ButtonStartX + ButtonSpacing * 2.f);
    MakeButton(BattleAction::Escape, "spr_escape_button.png", "spr_escape_button_hover.png", ButtonStartX + ButtonSpacing * 3.f);

    // by default, we will select the Attack button on start.
    Buttons[0]->bIsSelected = true;
}

void BattleMenu::MoveLeft()
{
    Buttons[SelectedIndex]->bIsSelected = false;
    SelectedIndex = (SelectedIndex - 1 + (int)Buttons.size()) % (int)Buttons.size();
    Buttons[SelectedIndex]->bIsSelected = true;
}

void BattleMenu::MoveRight()
{
    Buttons[SelectedIndex]->bIsSelected = false;
    SelectedIndex = (SelectedIndex + 1) % (int)Buttons.size();
    Buttons[SelectedIndex]->bIsSelected = true;
}

void BattleMenu::Draw(RenderWindow& window)
{
    for (auto& btn : Buttons)
        btn->Draw(window);
}