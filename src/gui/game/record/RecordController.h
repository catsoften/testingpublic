#ifndef RECORDCONTROLLER_H
#define RECORDCONTROLLER_H
#include "Config.h"

#include "RecordState.h"

#include <cstdint>
#include <vector>
#include <functional>
#include <mutex>

#include "gif/msf_gif.h"

struct WebPAnimEncoder;

class Renderer;

class RecordController
{
	RecordState& state;
	RecordState& rs = state;
	WebPAnimEncoder* enc;
	MsfGifState* gifState;

	int xs, ys;
	int bufs;
	std::vector<uint32_t*> bufferData;
	std::mutex bufferDataMutex;

	bool stopWrite = false;

	void StartRecordingCurrent();
	void WriteFrameCurrent(uint32_t* buffer);
	void StopRecordingCurrent();

	using RecordProgressCallback = std::function<void ()>;
	RecordProgressCallback callback;
	std::mutex callbackMutex;

	void StartWriteThread();
	void WaitForFrames();
	void CallCallback();
	void FreeRemaining();

public:
	RecordController(RecordState& state);

	void StartRecording();
	void WriteFrame(Renderer* ren);
	void StopRecording();

	void SetCallback(RecordProgressCallback newCallback);
	void CancelWrite();
};

#endif /* RECORDCONTROLLER_H */
