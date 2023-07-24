#pragma once
#include "RecordWriter.h"

#include "record/msf_gif.h"

class GifWriter : public RecordWriter
{
	int x, y;
	int file;

	int delay;

	MsfGifState* gifState;

public:
	GifWriter(int delay_);

	virtual void Start(int x_, int y_, int file_) override;
	virtual void Write(uint32_t* buffer) override;
	virtual void Stop() override;
};
