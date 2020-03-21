#include <Andromeda/Graphics/RenderManager.h>
#include <Andromeda/Utils/Logger.h>

#include "JellyGameManager.h"
#include "Game/JellySplash.h"

void JellyGameManager::Configure()
{
	//set pc resolution
	#ifdef ANDROMEDA_GL3
	{
		//Andromeda::Graphics::RenderManager::Instance()->SetWindowSize(1920, 1080);
		Andromeda::Graphics::RenderManager::Instance()->SetWindowSize(1280, 720);
		//Andromeda::Graphics::RenderManager::Instance()->SetWindowSize(960, 544);

		//Andromeda::Graphics::RenderManager::Instance()->SetWindowSize(-1, -1);
		//Andromeda::Graphics::RenderManager::Instance()->SetFullScreen(true);
	}
	#endif	
}

void JellyGameManager::Init()
{
	exampleState = new JellySplash();
	//exampleState->Init();

	ChangeState(exampleState);
}

void JellyGameManager::CleanUp()
{
	//clean all states
	for (size_t i = 0; i < _states.size(); i++)
	{
		if (_states[i] != NULL)
		{
			_states[i]->CleanUp();
			delete _states[i];
		}
	}

	_states.clear();
}