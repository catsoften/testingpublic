#include "AudioController.h"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#include "bzip2/bz2wrap.h"
#include "elements.wav.bz2.h"

#include "OffsetTable.h"

#include "Sound.h"

struct SDLData
{
	SDL_AudioSpec spec;
	SDL_AudioDeviceID device;
};

AudioController::AudioController() :
	ready(false),
	sdlData(std::make_unique<SDLData>()),
	buf(nullptr),
	lastMax(1.0f),
	sounds({ nullptr }),
	playing(0)
{
	std::vector<char> elements_wav;
	if (BZ2WDecompress(elements_wav, reinterpret_cast<const char *>(elements_wav_bz2), elements_wav_bz2_size) != BZ2WDecompressOk)
	{
		std::cerr << "AudioController::AudioController->BZ2WDecompress" << std::endl;
		return;
	}

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		std::cerr << "AudioController::AudioController->SDL_InitSubSystem: " << SDL_GetError() << std::endl;
		return;
	}

	if (!SDL_LoadWAV_RW(SDL_RWFromConstMem(elements_wav.data(), elements_wav.size()), 1, &sdlData->spec, reinterpret_cast<uint8_t**>(&audio_buf), &audio_len))
	{
		std::cerr << "AudioController::AudioController->SDL_LoadWAV_RW: " << SDL_GetError() << std::endl;
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return;
	}
	audio_len /= 2; // 8 to 16 bit conversion

	sdlData->spec.callback = &SDL_AudioCallback;
	sdlData->spec.userdata = this;

	sdlData->device = SDL_OpenAudioDevice(NULL, 0, &sdlData->spec, NULL, 0);
	if (!sdlData->device)
	{
		std::cerr << "AudioController::AudioController->SDL_OpenAudioDevice: " << SDL_GetError() << std::endl;
		SDL_FreeWAV(reinterpret_cast<uint8_t*>(audio_buf));
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return;
	}

	ready = true;
}

void AudioController::SDL_AudioCallback(void* userdata, uint8_t* stream, int len)
{
	auto& ctl = *reinterpret_cast<AudioController*>(userdata);

	if (!ctl.buf)
	{
		ctl.buf = std::make_unique<float[]>(len / 2);
	}
	std::fill(ctl.buf.get(), ctl.buf.get() + len / 2, 0.0f);

	for (auto& i : ctl.sounds)
	{
		if (i)
		{
			if (i->HasSamples())
			{
				i->AddSamples(ctl.buf.get(), len / 2);
			}
			if (!i->HasSamples())
			{
				i = nullptr;
				ctl.playing--;
			}
		}
	}

	float max = 1.0f;
	for (int i = 0; i < len / 2; i++)
	{
		max = std::max(max, std::abs(ctl.buf[i]));
	}
	float thisMax = std::max(ctl.lastMax, max);
	ctl.lastMax = max;
	for (int i = 0; i < len / 2; i++)
	{
		reinterpret_cast<int16_t*>(stream)[i] = static_cast<int16_t>(std::tanh(ctl.buf[i] / thisMax) * static_cast<float>(SDL_MAX_SINT16));
	}

	if (!ctl.playing)
	{
		SDL_PauseAudioDevice(ctl.sdlData->device, 1);
	}
}

void AudioController::Play(int index)
{
	if (ready && index >= 0 && index <= 199)
	{
		SDL_LockAudioDevice(sdlData->device);
		for (auto& i : sounds)
		{
			if (!i)
			{
				i = std::make_unique<Sound>(audio_buf, elements_wav_offsets[index], elements_wav_offsets[index + 1]);
				playing++;
				SDL_PauseAudioDevice(sdlData->device, 0);
				break;
			}
		}
		SDL_UnlockAudioDevice(sdlData->device);
	}
}

int AudioController::SoundsPlaying()
{
	return playing;
}

AudioController::~AudioController()
{
	if (ready)
	{
		SDL_CloseAudioDevice(sdlData->device);
		SDL_FreeWAV(reinterpret_cast<uint8_t*>(audio_buf));
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	}
}
