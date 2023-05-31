#pragma once
#include "RecordWriter.h"

class OldWriter : public RecordWriter
{
	int x, y;
	int file;

	int frame;

public:
	OldWriter();

	virtual void Start(int x_, int y_, int file_) override;
	virtual void Write(uint32_t* buffer) override;
	virtual void Stop() override;
};
