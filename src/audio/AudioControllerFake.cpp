#include "AudioController.h"

class Sound
{

};

struct SDLData
{

};

AudioController::AudioController() :
	ready(false),
	sdlData(nullptr),
	buf(nullptr),
	lastMax(1.0f),
	sounds({ nullptr }),
	playing(0)
{

}

void AudioController::SDL_AudioCallback(void* userdata, uint8_t* stream, int len)
{

}

void AudioController::Play(int index)
{

}

int AudioController::SoundsPlaying()
{
	return 0;
}

AudioController::~AudioController()
{

}
