#ifndef JellyGameManager_h
#define JellyGameManager_h


#include <Andromeda/System/GameManager.h>


class JellyGameManager : public Andromeda::System::GameManager
{
private:

	Andromeda::System::GameState* exampleState;

public:

	void Configure();
	void Init();
	void CleanUp();
};

#endif