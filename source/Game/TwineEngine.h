#pragma once
#include "Engine/Engine.h"

// base game engine class - this should be renamed and used for your own project.
class TwineEngine : public Engine
{
public:
	TwineEngine() {}

	// initialize the engine instance.
	static void Init()
	{
		if (!EngineInstance)
		{
			EngineInstance = new TwineEngine();

			// initialize after the instance is set.
			static_cast<TwineEngine*>(EngineInstance)->Initialize();
		}
	}

protected:

	/* replacement for the default constructor. just call this on Init() */
	void Initialize();

	void Input() override;
	void Update(float DeltaTime) override;
	void Draw() override;

	const char* GetName() const override { return "TwineEngine"; }

private:
	// demo sprites
	GameSprite DemoSprite;
	float RotationAngle = 0.0f;

	// demo text.
	GameText TitleText;
	GameText SubtitleText;
	GameText TeaserText;

};