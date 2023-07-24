#pragma once
#include "RecordWriter.h"

struct WebPAnimEncoder;

class WebPWriter : public RecordWriter
{
	int x, y;
	int file;

	int delay;
	int quality;

	int frame;
	WebPAnimEncoder* enc;

public:
	WebPWriter(int delay_, int quality_);

	virtual void Start(int x_, int y_, int file_) override;
	virtual void Write(uint32_t* buffer) override;
	virtual void Stop() override;
};
