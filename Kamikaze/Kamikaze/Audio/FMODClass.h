#pragma once
class FMODClass : public AudioWrapper
{
public:
	// do have i to init also m_pSoundList[MAX_CHANNELS] and m_oChannels[MAX_CHANNELS]?
	FMODClass() : m_uiSoundFreeSpace(MAX_CHANNELS), m_uiActiveChannel(0), m_uiMode(FMOD_DEFAULT), m_pSystem(nullptr), m_uiActiveSound(0) {}
	~FMODClass();
	void createSound(const char* pFile);
	void selectSoundFromList(unsigned int activesound);
	void selectLastCreatedSoundFromList();

	void Init();
	void Loading(LOADINGTYPE loadtype);
	void Pause();
	void Play();
	void Stop();
	void Channel(unsigned int channel);
	void setLooping(bool value);

	bool isChannelPlayingSound() //,unsigned int sound
	{
		//m_oChannels[channel]->getCurrentSound(&m_pSoundList[sound]);
		bool isPlaying;
		m_oChannels[m_uiActiveChannel]->isPlaying(&isPlaying);
		return isPlaying;
	}

	void setVolume(float volume)
	{
		m_oChannels[m_uiActiveChannel]->setVolume(volume);
	}
private:
	FMOD::System *m_pSystem;
	FMOD::Sound *m_pSoundList[MAX_CHANNELS];
	unsigned int m_uiActiveSound;

	FMOD_MODE m_uiMode; //static,streaming
	FMOD::Channel* m_oChannels[MAX_CHANNELS];
	unsigned int m_uiActiveChannel;
	unsigned int m_uiSoundFreeSpace;
};