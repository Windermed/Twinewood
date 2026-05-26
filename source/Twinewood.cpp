#include "Game/TwineEngine.h"

int main()
{
	// Gets a new instance of the engine directly and runs it.
	TwineEngine::Init();
	TwineEngine::GetEngine()->Run();
	TwineEngine::Shutdown();

	// Quit in the usual way when the engine is stopped
	return 0;
}