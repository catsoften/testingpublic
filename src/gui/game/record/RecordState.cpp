#include "RecordState.h"

#include <cmath>
#include <algorithm>

bool RecordState::CanStopStart()
{
	return stage != RecordStage::Writing && !writing;
}

bool RecordState::IsActive()
{
	return stage == RecordStage::Recording || stage == RecordStage::Paused;
}

bool RecordState::CanEdit()
{
	return stage == RecordStage::Stopped;
}

void RecordState::TogglePause()
{
	if (stage == RecordStage::Paused)
	{
		stage = RecordStage::Recording;
	}
	else if (stage == RecordStage::Recording)
	{
		stage = RecordStage::Paused;
	}
}

void RecordState::RecalcPos(bool inclusive)
{
	int ox1 = x1 == -1 ? 0 : x1;
	int oy1 = y1 == -1 ? 0 : y1;
	int ox2 = x2 == -1 ? XRES : x2;
	int oy2 = y2 == -1 ? YRES : y2;
	x1 = std::min(std::max(ox2 > ox1 ? ox1 : ox2, 0), XRES);
	x2 = std::min(std::max(ox2 > ox1 ? ox2 : ox1, x1 + 1), XRES) + (inclusive ? 1 : 0);
	y1 = std::min(std::max(oy2 > oy1 ? oy1 : oy2, 0), YRES);
	y2 = std::min(std::max(oy2 > oy1 ? oy2 : oy1, y1 + 1), YRES) + (inclusive ? 1 : 0);
}

void RecordState::ClearCounters()
{
	frame = 0;
	nextFrame = 0;
	ratioFrame = 0;
}

void RecordState::Clear()
{
	format = RecordFormat::Gif;
	buffer = RecordBuffer::Ram;
	writeThread = false; // Dont trust that enough to be on by default
	quality = 7;
	fps = 60;
	x1 = 0;
	y1 = 0;
	x2 = XRES;
	y2 = YRES;
	RecalcPos();
	scale = 1;
	spacing = false;
	ClearCounters();
}

RecordState::RecordState()
{
	stage = RecordStage::Stopped;
	halt = false;
	writing = false;
	file = 0;
	select = 0;
	Clear();
}
