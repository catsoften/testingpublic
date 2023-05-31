#include "WebPWriter.h"
#include "Config.h"

#include <iostream>
#include <fstream>

#include "common/String.h"

#include "webp/types.h"
#include "webp/encode.h"
#include "webp/mux_types.h"
#include "webp/mux.h"

WebPWriter::WebPWriter(int delay_, int quality_) :
	delay(delay_),
	quality(quality_),
	frame(0)
{ }

void WebPWriter::Start(int x_, int y_, int file_)
{
	x = x_;
	y = y_;
	file = file_;
	WebPAnimEncoderOptions enc_options;
	WebPAnimEncoderOptionsInit(&enc_options);
	enc_options.anim_params.bgcolor = 0xFF000000;
	enc_options.anim_params.loop_count = 0;
	enc_options.minimize_size = quality == 10;
	enc = WebPAnimEncoderNew(x, y, &enc_options);
}

void WebPWriter::Write(uint32_t* buffer)
{
	WebPConfig config;
	WebPConfigInit(&config);
	config.lossless = true;
	config.quality = (float)(quality * 10);
	WebPPicture pic;
	WebPPictureInit(&pic);
	pic.use_argb = true;
	pic.width = x;
	pic.height = y;
	pic.argb = buffer;
	pic.argb_stride = x;
	WebPAnimEncoderAdd(enc, &pic, frame++ * delay, &config);
	WebPPictureFree(&pic);
}

void WebPWriter::Stop()
{
	WebPAnimEncoderAdd(enc, NULL, frame * delay, NULL);
	WebPData webp_data;
	WebPDataInit(&webp_data);
	WebPAnimEncoderAssemble(enc, &webp_data);
	try
	{
		std::ofstream outFile(ByteString::Build("recordings", PATH_SEP, file, ".webp"), std::ios::binary | std::ios::trunc);
		outFile.write((const char*)webp_data.bytes, webp_data.size);
	}
	catch (const std::exception& e)
	{
		std::cerr << "WebPWriter::Stop: " << e.what() << std::endl;
	}
	WebPDataClear(&webp_data);
}

WebPWriter::~WebPWriter()
{
	WebPAnimEncoderDelete(enc);
}
