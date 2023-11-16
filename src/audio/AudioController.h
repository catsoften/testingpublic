#pragma once

#include <cstdint>
#include <array>
#include <memory>
#include <atomic>

class Sound;
struct SDLData; // Renderer builds don't have access to SDL

class AudioController
{
	bool ready;

	std::unique_ptr<SDLData> sdlData;
	int16_t* audio_buf;
	uint32_t audio_len;

	std::unique_ptr<float[]> buf;
	float lastMax;

	std::array<std::unique_ptr<Sound>, 10000> sounds;
	std::atomic<int> playing;

public:
	AudioController();

	static void SDL_AudioCallback(void* userdata, uint8_t* stream, int len);

	void Play(int index);

	int SoundsPlaying();

	~AudioController();
};
