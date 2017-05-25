#ifndef SOUND_H
#define SOUND_H

/* Sound:
* Link sound to ship action
*/

#include "Audio/AudioWrapper.h"
#include "Audio/OpenALClass.h"
#include "Audio/FMODClass.h"

struct ShipSound {
	enum { LEFT, RIGHT, GO, FIRE, DIE, N_SOUNDS };
	FMODClass FMODsounds;

	ShipSound() {
		FMODsounds.Init();
		FMODsounds.Loading(FMODClass::Static);
		initSounds();
	}

	void initSounds();
	void playSound(int shipAction)
	{
		if (shipAction == LEFT || shipAction == RIGHT)
		{ 
			bool isPlayingLeft;
			bool isPlayingRight;

			FMODsounds.Channel(LEFT);
			isPlayingLeft = FMODsounds.isChannelPlayingSound(); 
			FMODsounds.Channel(RIGHT);
			isPlayingRight = FMODsounds.isChannelPlayingSound();

			if (!isPlayingLeft && !isPlayingRight)
			{
				FMODsounds.Channel(shipAction);
				FMODsounds.selectSoundFromList(shipAction);
				FMODsounds.Play();
				FMODsounds.setVolume(0.1f);
			}
		}
		else
		{
			FMODsounds.Channel(shipAction); //select channel for active sound
			FMODsounds.Stop();
			FMODsounds.selectSoundFromList(shipAction);
			FMODsounds.Play();
		}
	}
};

#endif // SOUND_H

/*
FMODClass myFMODClass;
myFMODClass.Init();
myFMODClass.Loading(FMODClass::Streaming);
myFMODClass.createSound("Audio/AudioWavFiles/BARK12.WAV");
myFMODClass.selectLastCreatedSoundFromList();
myFMODClass.Channel(2);
myFMODClass.setLooping(false);
myFMODClass.Play();
*/