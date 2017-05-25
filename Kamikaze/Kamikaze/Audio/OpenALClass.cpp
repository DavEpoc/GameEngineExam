
//#include "stdafx.h"
#include <assert.h>
#include <cstdlib>
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

#include "WavManager.hpp"
#include "AudioWrapper.h"

#include "OpenALClass.h"
#include "FMODClass.h"

OpenALClass::~OpenALClass()
{
	//if(devices) delete devices;
	//if(defaultDeviceName) delete defaultDeviceName;

	//free devices vector
	devicesList->clear();
	devicesList->swap(*devicesList);

	//disable context
	alcMakeContextCurrent(NULL);

	//remember unbind all buffers from resources BEFORE DELETING BUFFERS or destroy all resources linked to buffer first
	//to unbind: alSourcei(mSourceId, AL_BUFFER, NULL);
	//Deleting sources first:
	for (int i = 0; i < MAX_CONTEXTS; ++i)
	{
		if (sourcesList[i])
		{
			alDeleteSources(MAX_SOURCES_PER_CONTEXT, sourcesList[i]);

			//Release context(s)
			alcDestroyContext(ContextsList[i]);
		}
	}

	alDeleteBuffers(MAX_BUFFERS, BuffersList);

	if (singleBufferStreamingData) delete singleBufferStreamingData;
	if (streamingBuffersList)
	{
		for (int i = 0; i < MAX_BUFFERS; ++i)
		{
			if(streamingBuffersList[i]) alDeleteBuffers(STREAMING_BUFFERS_NUM, streamingBuffersList[i]);
		}
	}

	//Close device
	alcCloseDevice(ActiveDevice);
}

void OpenALClass::Init()
{
	//Init Devices 

	//Enumerating Devices
	ALboolean enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
	cout << (enumeration != AL_FALSE);

	if (enumeration == AL_TRUE)
	{
		// Pass in NULL device handle to get list of devices,
		// devices contains the device names, separated by NULL
		// and terminated by two consecutive NULLs.
		devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

		// defaultDeviceName contains the name of the default
		defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

		string mydevices(devices);
		string mydefaultdevice(defaultDeviceName);

		cout << mydevices + " DEFAULT: " + mydefaultdevice << endl;

		stringstream ssin(mydevices);
		while (ssin.good())
		{
			string tmp;
			ssin >> tmp;
			devicesList->push_back(tmp);
		}

		for (unsigned int i = 0; i < devicesList->size(); i++)
		{
			cout << (*devicesList).at(i) << endl;
		}

	}
	else
	{
		cout << "could not find any audio device" << endl;
	}

	// Initialization
	ActiveDevice = alcOpenDevice(NULL); // select the "preferred device", NULL == default

	if (ActiveDevice) {
		ContextsList[activeContext] = alcCreateContext(ActiveDevice, NULL);
		alcMakeContextCurrent(ContextsList[activeContext]);
	}
	else
	{
		error = alGetError();
		cout << "could not open any audio device: error - " + error << endl;
	}

	// Check for EAX 2.0 support
	//ALboolean g_bEAX = alIsExtensionPresent("EAX2.0");
	//cout << (g_bEAX != AL_FALSE);

	/*
	//set listener position
	ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
	alListener3f(AL_POSITION, 0, 0, 1.0f);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);
	*/

	alGenBuffers(MAX_BUFFERS, BuffersList);

	// Copy .wav data into AL Buffer 0	
	cout << "BufferValue0: " << BuffersList[0] << endl;
	cout << "BufferValue1: " << BuffersList[1] << endl;

	//openAL sets it's own *source value, no need to do that
	alGenSources(MAX_SOURCES_PER_CONTEXT, sourcesList[activeContext]);
	//alGenSources(*source, source);	

	error = alGetError();
	cout << "errorSources: " << error << endl;
}

void OpenALClass::selectContext(unsigned int activecontext)
{
	if (activecontext != activeContext)
	{
		assert(activecontext < MAX_CONTEXTS && "there is no such context in context's list");

		if (ContextsList[activecontext] == nullptr) ContextsList[activecontext] = alcCreateContext(ActiveDevice, NULL);
		alcMakeContextCurrent(ContextsList[activecontext]);
		activeContext = activecontext;
		//create sources for new context
		if (sourcesList[activecontext] == nullptr)  sourcesList[activeContext] = new ALuint[MAX_SOURCES_PER_CONTEXT]();
	}
	else
	{
		cout << "selected context is already the active context" << endl;
	}
}

void OpenALClass::createSound(const char* pFile)
{
	ALsizei size;
	ALsizei frequency;
	ALenum  format;
	unsigned char* data = nullptr;

	//data passes pointer reference
	loadWavFile(pFile, &format, data, &size, &frequency);

	if ((error = alGetError()) != AL_NO_ERROR)
	{
		cout << "error in loading wav: " + error << endl;
	}

	//sound *newSound = new sound(format, data, size, frequency);
	soundsList->push_back(sound(format, data, size, frequency));
}

void OpenALClass::selectSoundFromList(unsigned int activesound)
{
	if (activesound != activeSound)
	{
		assert(soundsList->size() != 0 && "there are no sounds in sound's list");
		assert(activesound < soundsList->size() && "there is no such index in sound's list");
		activeSound = activesound;
	}
	else
	{
		cout << "selected sound is already the active sound" << endl;
	}
}

void OpenALClass::selectLastCreatedSoundFromList()
{
	if ((soundsList->size() - 1) != activeSound)
	{
		assert(soundsList->size() != 0 && "there are no sounds in sound's list");
		activeSound = soundsList->size() - 1;
	}
	else
	{
		cout << "selected sound is already the active sound" << endl;
	}
}

//selects active buffer
void OpenALClass::Channel(unsigned int activebuffer)
{
	if (activebuffer != activeBuffer)
	{
		assert(activebuffer < MAX_BUFFERS && "there is no such buffer in buffer's list");
		activeBuffer = activebuffer;
	}
	else
	{
		cout << "selected buffer is already the active buffer" << endl;
	}
}

//load on streaming buffers or static
void OpenALClass::Loading(LOADINGTYPE loadtype)
{
	if (loadtype == Static)
	{
		isStreaming = false;
	}
	else if (loadtype == Streaming)
	{
		if (!streamingBuffersList) streamingBuffersList = new ALuint*[MAX_BUFFERS]();

		if (!streamingBuffersList[activeBuffer])
		{
			streamingBuffersList[activeBuffer] = new ALuint[STREAMING_BUFFERS_NUM]();
			alGenBuffers(STREAMING_BUFFERS_NUM, streamingBuffersList[activeBuffer]);
		}

		if (!singleBufferStreamingData) singleBufferStreamingData = new unsigned char[STREAMING_BUFFER_SIZE]();
		isStreaming = true;
	}
}

//links current active sound to current active buffer
void OpenALClass::linkSoundToBuffer()
{

	//stop all sources which are using active buffer first
	for (int context = 0; context < MAX_CONTEXTS; ++context)
	{
		if (sourcesList[context])
		{
			for (int source = 0; source < MAX_SOURCES_PER_CONTEXT; ++source)
			{
				ALint sourceState = -1;
				alGetSourcei(sourcesList[context][source], AL_SOURCE_STATE, &sourceState);

				ALint runningBufferID = -1;
				alGetSourcei(sourcesList[context][source], AL_BUFFER, &runningBufferID);

				if (sourceState == AL_PLAYING && runningBufferID == BuffersList[activeBuffer]) alSourceStop(sourcesList[context][source]);

				//stop sources using a rotating streaming buffer if streaming
				if (sourceState == AL_PLAYING && isStreaming)
				{
					for (int streamingBuf = 0; streamingBuf < STREAMING_BUFFERS_NUM; ++streamingBuf)
					{
						if (runningBufferID == streamingBuffersList[activeBuffer][streamingBuf]) alSourceStop(sourcesList[context][source]);
					}
				}
			}
		}
	}

	sound currentSound = soundsList->at(activeSound);

	if (isStreaming)
	{
		currentSoundDataPosition[activeBuffer] = 0;

		cout << "soundSize: " << currentSound.size << endl;

		//fill streamingbuffers with data
		for (int i = 0; i < STREAMING_BUFFERS_NUM && currentSoundDataPosition[activeBuffer] < currentSound.size; ++i)
		{
			currentSoundDataPosition[activeBuffer] = i*STREAMING_BUFFER_SIZE;
			int k = 0;
			for (k; k < STREAMING_BUFFER_SIZE && currentSoundDataPosition[activeBuffer] < currentSound.size; ++k)
			{
				singleBufferStreamingData[k] = currentSound.data[i*STREAMING_BUFFER_SIZE + k];
				++currentSoundDataPosition[activeBuffer];
			}

			ALsizei chunckDataSize = k < STREAMING_BUFFER_SIZE ? k : STREAMING_BUFFER_SIZE;
			cout << "chunckDataSize: " << chunckDataSize << endl;

			alBufferData(streamingBuffersList[activeBuffer][i], currentSound.format, (const ALvoid*)singleBufferStreamingData, chunckDataSize, currentSound.frequency);

			cout << "currentSoundDataPosition: " << currentSoundDataPosition[activeBuffer] << endl;
		}
	}
	else
	{
		//replace buffer sound data with active sound data
		alBufferData(BuffersList[activeBuffer], currentSound.format, (const ALvoid*)currentSound.data, currentSound.size, currentSound.frequency);
	}

	if ((error = alGetError()) != AL_NO_ERROR)
	{
		cout << "error in while setting buffer/s: " + error << endl;
	}
}

void OpenALClass::selectSource(unsigned int activesurce)
{
	assert(activesurce < MAX_SOURCES_PER_CONTEXT && "there is no such source in source's list");
	activeSource = activesurce;
}

void OpenALClass::linkBufferToSource()
{
	if (isStreaming)
	{
		alSourceQueueBuffers(sourcesList[activeContext][activeSource], STREAMING_BUFFERS_NUM, streamingBuffersList[activeBuffer]);
		//cout << "bufferIDS: 1) " << streamingBuffersList[activeBuffer][0] << " 2) " << streamingBuffersList[activeBuffer][1] << " 3) " << streamingBuffersList[activeBuffer][2] << endl;
	}
	else
	{
		alSourcei(sourcesList[activeContext][activeSource], AL_BUFFER, BuffersList[activeBuffer]);
	}

	if ((error = alGetError()) != AL_NO_ERROR)
	{
		cout << "could not link buffer/s to source error: " << error << endl;
	}
}

//Pause active source in active context
void OpenALClass::Pause()
{
	cout << "Pause" << endl;
	alSourcePause(sourcesList[activeContext][activeSource]);
}

//Play active source in active context
void OpenALClass::Play()
{
	//PlaySource
	alSourcePlay(sourcesList[activeContext][activeSource]);

	sound currentSound = soundsList->at(activeSound);
	if (isStreaming)
	{
		//if audio file is to large and i need to refill buffers in streamingBuffersList to complete the sound
		ALuint unqueuedBufferID = -1;
		ALint val;

		int totalNumOfExtraBuffersExecuted = 0;

		//IMPORTANT: this loop should be a coroutine to work properly along with Pause/Stop commands
		while (currentSoundDataPosition[activeBuffer] < currentSound.size)
		{
			/*
			int runningBufferID;
			alGetSourcei(sourcesList[activeContext][activeSource], AL_BUFFER, &runningBufferID);
			cout << "runningBufferID: " << runningBufferID << endl;
			*/

			//cout << "enter first while" << endl;
			alGetSourcei(sourcesList[activeContext][activeSource], AL_BUFFERS_PROCESSED, &val);
			if (val <= 0) continue;

			while (val--)
			{
				cout << "enter second while" << endl;
				cout << "currentSoundDataPositionExtra: " << currentSoundDataPosition[activeBuffer] << endl;

				//read next part of file
				int j = 0;
				for (j; j < STREAMING_BUFFER_SIZE && currentSoundDataPosition[activeBuffer] < currentSound.size; ++j)
				{
					singleBufferStreamingData[j] = currentSound.data[currentSoundDataPosition[activeBuffer]];
					++currentSoundDataPosition[activeBuffer];
				}

				ALsizei chunckDataSize = j < STREAMING_BUFFER_SIZE ? j : STREAMING_BUFFER_SIZE;
				cout << "extraChunckDataSize: " << chunckDataSize << endl;
				cout << "extrabuffersCurrentChar: " << currentSoundDataPosition[activeBuffer] << endl;

				//Unqueue buffer already played,get unqueuedBufferID
				alSourceUnqueueBuffers(sourcesList[activeContext][activeSource], 1, &unqueuedBufferID);

				cout << "UNQUEUEDBUFFERID: " << unqueuedBufferID << endl;

				if (alGetError() != AL_NO_ERROR)
				{
					cout << "Error Unqueuing buffer" << endl;
				}

				//fill this buffer with new datas
				alBufferData(unqueuedBufferID, currentSound.format, (const ALvoid*)singleBufferStreamingData, chunckDataSize, currentSound.frequency);

				if (alGetError() != AL_NO_ERROR)
				{
					cout << "Error refilling buffer" << endl;
				}

				//Queue the buffer again
				alSourceQueueBuffers(sourcesList[activeContext][activeSource], 1, &unqueuedBufferID);

				if (alGetError() != AL_NO_ERROR)
				{
					cout << "Error re-buffering already played buffer" << endl;
				}
				else
				{
					++totalNumOfExtraBuffersExecuted;
				}
			}

			/*
			//Make sure the source is still playing, and restart it if needed
			alGetSourcei(sourcesList[activeContext][activeSource], AL_SOURCE_STATE, &val);
			if (val != AL_PLAYING) alSourcePlay(sourcesList[activeContext][activeSource]);
			*/
		}

		//wait untill sound ends
		do
		{
			alGetSourcei(sourcesList[activeContext][activeSource], AL_SOURCE_STATE, &val);
		} while (val == AL_PLAYING);

		//if loop enabled restart sound
		if (isLooping) //currentSoundDataPosition[activeBuffer] == currentSound.size
		{
			//linkSoundToBuffer() already sets currentSoundDataPosition[activeBuffer] = 0;

			//unqueue buffers before refilling them
			alSourceUnqueueBuffers(sourcesList[activeContext][activeSource], STREAMING_BUFFERS_NUM, streamingBuffersList[activeBuffer]);

			//refill
			linkSoundToBuffer();

			//Queue them again
			alSourceQueueBuffers(sourcesList[activeContext][activeSource], STREAMING_BUFFERS_NUM, streamingBuffersList[activeBuffer]);

			Play();
		}
		cout << "totalNumOfExtraBuffersExecuted: " << totalNumOfExtraBuffersExecuted << endl;
	}
}

//Stop active source in active context
void OpenALClass::Stop()
{
	cout << "Stop" << endl;
	alSourceStop(sourcesList[activeContext][activeSource]);
}

//Set loop for active source in active context
void OpenALClass::setLooping(bool value)
{
	isLooping = value;
	if (!isStreaming)
	{
		int bValue = value ? AL_TRUE : AL_FALSE;
		alSourcei(sourcesList[activeContext][activeSource], AL_LOOPING, bValue);
	}
}