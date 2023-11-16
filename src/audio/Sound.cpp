#include "Sound.h"

#include <SDL2/SDL.h>

Sound::Sound(int16_t* audio_buf_, uint32_t startPos_, uint32_t endPos_) :
	audio_buf(audio_buf_),
	pos(startPos_),
	endPos(endPos_)
{

}

bool Sound::HasSamples()
{
	return pos != endPos;
}

void Sound::AddSamples(float* stream, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (pos != endPos)
		{
			stream[i] += static_cast<float>(audio_buf[pos++]) / static_cast<float>(SDL_MAX_SINT16);
		}
		else
		{
			return;
		}
	}
}
