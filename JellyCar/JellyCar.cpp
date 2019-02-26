#include <Andromeda/System/GameManager.h>
#include <Andromeda/System/GameLoader.h>
#include "JellyGameManager.h"

using namespace Andromeda::System;

int main(int argc, char *argv[])
{
	JellyGameManager* jellyGame = new JellyGameManager();
	GameLoader* loader = GameLoader::Create(jellyGame);

	loader->Run();

	return 0;
}
