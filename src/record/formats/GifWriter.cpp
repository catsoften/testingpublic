#include "GifWriter.h"
#include "Config.h"

#include <iostream>
#include <fstream>

#include "common/String.h"

#define MSF_GIF_IMPL
#include "record/msf_gif.h"

GifWriter::GifWriter(int delay_) :
	delay(delay_)
{
	gifState = new MsfGifState;
}

void GifWriter::Start(int x_, int y_, int file_)
{
	x = x_;
	y = y_;
	file = file_;
	msf_gif_begin(gifState, x, y);
}

void GifWriter::Write(uint32_t* buffer)
{
	msf_gif_frame(gifState, (uint8_t*)buffer, delay, 16, x * 4);
}

void GifWriter::Stop()
{
	MsfGifResult result = msf_gif_end(gifState);
	try
	{
		std::ofstream outFile(ByteString::Build("recordings", PATH_SEP, file, ".gif"), std::ios::binary | std::ios::trunc);
		outFile.write((const char*)result.data, result.dataSize);
	}
	catch (const std::exception& e)
	{
		std::cerr << "GifWriter::Stop: " << e.what() << std::endl;
	}
	msf_gif_free(result);
}

GifWriter::~GifWriter()
{
	delete gifState;
}
