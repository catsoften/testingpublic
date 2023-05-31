#pragma once

#include <cstdint>

class RecordWriter
{
public:
	RecordWriter() = default;

	virtual void Start(int x_, int y_, int file_) = 0;
	virtual void Write(uint32_t* buffer) = 0;
	virtual void Stop() = 0;

	virtual ~RecordWriter() = default;
};
