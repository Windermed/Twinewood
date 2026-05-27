#pragma once
#include <SFML/Graphics.hpp>
#include "Core/Interactable.h"
#include "Core/Object.h"

using namespace sf;
using namespace std;

// base prop class. it's meant for any objects in the world that are meant to be interactable.
class Prop : public Object, public Interactable
{
public:
	void OnInteract() override
	{
		
	}
};