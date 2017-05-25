#pragma once

//Include Audio
#include <assert.h>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <windows.h>

#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h" // Only if you want error checking

#include "al.h" 
#include "alc.h" 
#include "EFX-Util.h"

using namespace std;

//FMOD
#define MAX_CHANNELS 36

//openAL
#define MAX_CONTEXTS 20
#define MAX_BUFFERS 20
#define MAX_SOURCES_PER_CONTEXT 20

#define STREAMING_BUFFER_SIZE 4096
#define STREAMING_BUFFERS_NUM 3

class AudioWrapper
{
public:
	enum LOADINGTYPE { Static, Streaming };

	virtual ~AudioWrapper() {};

	virtual void Init() = 0;
	virtual void Loading(LOADINGTYPE loadtype) = 0;

	virtual void Pause() = 0;
	virtual void Play() = 0;
	virtual void Stop() = 0;
	virtual void Channel(unsigned int channel) = 0;

	virtual void setLooping(bool value) = 0;
};