#ifndef AudioHelper_H
#define AudioHelper_H

#include <vector>
#include <string> 

#include <Andromeda/Audio/Sound.h>
#include <Andromeda/Audio/AudioManager.h>

using namespace Andromeda::Audio;

class AudioHelper
{
private:

	static AudioHelper* _instance;

	std::vector<std::string> _soundList;
	std::vector<std::string> _musicList;

	//audio manager
	AudioManager* _audioManager;

	//hit sounds
	std::vector<Sound*> _hitSounds;

	//background music
	Sound* _music;
	bool _musicPlaying;

	//car engine sound
	Sound* _carSlow;
	Sound* _carFast;

	//sound levels
	float _carVolume;
	float _soundsVolume;
	float _musicVolume;

public:

	enum AudioHelperSoundEnum
	{
		Car,
		Sounds,
		Music
	};

private:

	AudioHelper();

	void Init();
	void LoadSounds();

public:

	~AudioHelper();

	static AudioHelper* Instance();

	void PlayHitSound();

	void PlayMusic();
	void StopMusic();

	void PlayFastEngine();
	void PlaySlowEngine();

	void StopEngineSound();

	void SetVolume(AudioHelperSoundEnum sound,float volume);
	float GetVolume(AudioHelperSoundEnum sound);

	void LoadSetting();
	void SaveSettings();
};

#endif