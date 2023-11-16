#pragma once

#include <cstdint>

class Sound
{
	int16_t* audio_buf;

	uint32_t pos;
	uint32_t endPos;

public:
	Sound(int16_t* audio_buf_, uint32_t startPos_, uint32_t endPos_);

	bool HasSamples();
	void AddSamples(float* stream, int len);
};
