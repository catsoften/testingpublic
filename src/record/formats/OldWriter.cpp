#include "OldWriter.h"
#include "Config.h"

#include "Format.h"
#include "common/Platform.h"
#include "common/String.h"
#include "graphics/Graphics.h"

OldWriter::OldWriter()
{ }

void OldWriter::Start(int x_, int y_, int file_)
{
	frame = 0;
	x = x_;
	y = y_;
	file = file_;
	Platform::MakeDirectory(ByteString::Build("recordings", PATH_SEP, file).c_str());
}

void OldWriter::Write(uint32_t* buffer)
{
	VideoBuffer screenshot((pixel*)buffer, x, y);
	std::vector<char> data = format::VideoBufferToPPM(screenshot);
	ByteString path = ByteString::Build("recordings", PATH_SEP, file, PATH_SEP, "frame_", Format::Width(frame++, 6), ".ppm");
	Platform::WriteFile(data, path);
}

void OldWriter::Stop()
{ }
