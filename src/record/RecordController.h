#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <functional>
#include <mutex>

#include "common/Singleton.h"

#include "RecordState.h"
#include "formats/RecordWriter.h"

class Graphics;

class RecordController : public Singleton<RecordController>
{
	std::unique_ptr<RecordWriter> writer;

	int xs, ys, sxs, sys;
	int bufs, sbufs;
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
	RecordState rs;

	void StartRecording();
	void WriteFrame(Graphics* g);
	void StopRecording();

	void SetCallback(RecordProgressCallback newCallback);
	void CancelWrite();

	~RecordController();
};
