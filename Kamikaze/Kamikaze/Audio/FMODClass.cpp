
//#include "stdafx.h"
#include <assert.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
//#include <cstdlib>

#include <windows.h>

#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h" // Only if you want error checking

#include "al.h" 
#include "alc.h" 
#include "EFX-Util.h"
//#include "alut.h"

using namespace std;

#include "AudioWrapper.h"

#include "OpenALClass.h"
#include "FMODClass.h"


//#include "stdafx.h"
//#include "FMODClass.h"

//FMODClass
FMODClass::~FMODClass()
{
	//FMOD has release(); method to destroy objects, dont use delete or new!
	//channels do not require freeing;
	//sounds created with m_pSystem->create.... must be released
	//system must be closed and then released

	for (int i = 0; i < MAX_CHANNELS; ++i)
	{
		if (m_pSoundList[i])
		{
			m_pSoundList[i]->release();
		}
	}

	m_pSystem->close();
	m_pSystem->release();
}

void FMODClass::createSound(const char *pFile)
{
	if (m_uiSoundFreeSpace == 0) m_uiSoundFreeSpace = MAX_CHANNELS;
	if (m_pSoundList[MAX_CHANNELS - m_uiSoundFreeSpace] != nullptr) { m_oChannels[m_uiActiveChannel]->stop(); m_pSoundList[m_uiSoundFreeSpace - 1]->release(); }

	//do i have to use createStream? isnt mode chosing if static or streaming?
	m_pSystem->createSound(pFile, m_uiMode, 0, &m_pSoundList[MAX_CHANNELS - m_uiSoundFreeSpace]);
	--m_uiSoundFreeSpace;

}

void FMODClass::selectSoundFromList(unsigned int activesound)
{
	assert(m_pSoundList[activesound] != nullptr && "no such sound in list");
	m_uiActiveSound = activesound;
}

void FMODClass::selectLastCreatedSoundFromList()
{
	assert(m_pSoundList[MAX_CHANNELS - (m_uiSoundFreeSpace + 1)] != nullptr && "there are no sounds in list");
	m_uiActiveSound = MAX_CHANNELS - (m_uiSoundFreeSpace + 1);
}

void FMODClass::Loading(LOADINGTYPE loadtype)
{
	if (loadtype == Static)
	{
		m_uiMode = FMOD_DEFAULT;
	}
	else if (loadtype == Streaming)
	{
		m_uiMode = FMOD_CREATESTREAM;
	}
}

void FMODClass::Play()
{
	//if is not played play it else unstop it
	m_pSystem->playSound(m_pSoundList[m_uiActiveSound], 0, false, &m_oChannels[m_uiActiveChannel]);
}

void FMODClass::Pause()
{
	//m_pSystem->playSound(m_pActiveSound, 0, true, 0); //??
	m_oChannels[m_uiActiveChannel]->setPaused(true);
}

void FMODClass::Stop()
{
	m_oChannels[m_uiActiveChannel]->stop();
}

void FMODClass::Channel(unsigned int channel)
{
	assert(channel < MAX_CHANNELS && "invalid channel selected");
	m_uiActiveChannel = channel;
}

void FMODClass::setLooping(bool bLoop)
{
	if (!bLoop)
		m_pSoundList[m_uiActiveSound]->setMode(FMOD_LOOP_OFF);
	else
	{
		m_pSoundList[m_uiActiveSound]->setMode(FMOD_LOOP_NORMAL);
		m_pSoundList[m_uiActiveSound]->setLoopCount(-1);
	}
}

void FMODClass::Init()
{
	if (FMOD::System_Create(&m_pSystem) != FMOD_OK)
	{
		// Report Error
		cout << "fmod couldn't initialize properly";
		return;
	}

	int driverCount = 0;
	m_pSystem->getNumDrivers(&driverCount);

	if (driverCount == 0)
	{
		// Report Error
		cout << "fmod couldn't find any driver";
		return;
	}

	// Initialize our Instance with 36 Channels
	m_pSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL, NULL);
}