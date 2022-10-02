#include "RecordMenu.h"

#include "common/String.h"
#include "common/Platform.h"
#include "graphics/Graphics.h"
#include "gui/Style.h"
#include "gui/interface/Colour.h"
#include "gui/interface/Appearance.h"
#include "gui/interface/Label.h"
#include "gui/interface/Button.h"
#include "gui/interface/Checkbox.h"
#include "gui/interface/Textbox.h"
#include "gui/interface/DropDown.h"
#include "gui/interface/Slider.h"
#include "gui/dialogues/InformationMessage.h"
#include "gui/dialogues/ErrorMessage.h"
#include "gui/dialogues/ConfirmPrompt.h"

#include <cmath>
#include <algorithm>
#include <ctime>

bool RecordMenu::RecordingActive(RecordStage stage)
{
	return stage == RecordStage::Recording || stage == RecordStage::Paused;
}

void RecordMenu::ToggleRecording()
{
	newStage = newStage == RecordStage::Stopped ? RecordStage::Recording : RecordStage::Stopped;
}

void RecordMenu::UpdateTitle()
{
	if (state.writing)
	{
		titleLabel->SetText(String::Build("Record Menu - ", ((int)state.frame) + 1, "/", (int)state.nextFrame));
	}
	else
	{
		titleLabel->SetText("Record Menu");
	}
}

void RecordMenu::StateChanged()
{
	auto& rs = state;
	bool rec = RecordingActive(newStage);
	bool pas = newStage == RecordStage::Paused;
	bool comEn = !rec && rs.stage != RecordStage::Writing;
	ui::Colour comCl = comEn ? ui::Colour(255, 255, 255) : ui::Colour(100, 100, 100);

	ui::Appearance dropdownEnabled;
	dropdownEnabled.BackgroundHover = ui::Colour(20, 20, 20);
	dropdownEnabled.BackgroundInactive = ui::Colour(0, 0, 0);
	dropdownEnabled.BorderHover = comCl;
	dropdownEnabled.TextInactive = comCl;
	dropdownEnabled.TextHover = comCl;
	dropdownEnabled.BorderInactive = ui::Colour(200, 200, 200) ;

	ui::Appearance dropdownDisabled = dropdownEnabled;
	dropdownDisabled.BackgroundHover = ui::Colour(10, 10, 10);
	dropdownDisabled.BackgroundInactive = ui::Colour(10, 10, 10);
	dropdownDisabled.BorderInactive = ui::Colour(100, 100, 100);

	ui::Appearance dropApp = comEn ? dropdownEnabled : dropdownDisabled;

	// Title Label
	UpdateTitle();

	// Select Button
	if (rs.format == RecordFormat::Old && comEn)
	{
		rs.x1 = 0;
		rs.y1 = 0;
		rs.x2 = XRES;
		rs.y2 = YRES;
		rs.RecalcPos();
	}
	bool positionChanged = rs.x1 != 0 || rs.y1 != 0 || rs.x2 != XRES || rs.y2 != YRES;
	selectButton->Enabled = comEn && rs.format != RecordFormat::Old;
	selectButton->Appearance.BackgroundInactive = positionChanged ? ui::Colour(47, 47, 23) : ui::Colour(0, 0, 0);
	selectButton->Appearance.BackgroundHover = positionChanged ? ui::Colour(63, 63, 31) : ui::Colour(20, 20, 20);

	// FPS Label
	fpsLabel->SetTextColour(comCl);

	// FPS Textbox
	fpsTextbox->Enabled = comEn;
	fpsTextbox->ReadOnly = !comEn;
	fpsTextbox->SetText(String::Build(rs.fps));
	fpsTextbox->SetTextColour(comCl);

	// Scale Label
	scaleLabel->SetTextColour(comCl);

	// Scale Dropdown
	bool allowScaleBuffer = rs.format != RecordFormat::Old;
	if (!allowScaleBuffer && comEn)
	{
		rs.scale = 1;
	}
	scaleDropdown->Enabled = comEn && allowScaleBuffer;
	scaleDropdown->SetOption(rs.spacing ? -8 : rs.scale);
	scaleDropdown->Appearance = allowScaleBuffer ? dropApp : dropdownDisabled;

	// Format Label
	formatLabel->SetTextColour(comCl);

	// Format Dropdown
	formatDropdown->Enabled = comEn;
	formatDropdown->SetOption(rs.format);
	formatDropdown->Appearance = dropApp;

	// Buffer Label
	bufferLabel->SetTextColour(comCl);

	// Buffer Dropdown
	if (!allowScaleBuffer && comEn)
	{
		rs.buffer = RecordBuffer::Off;
	}
	bufferDropdown->Enabled = comEn && allowScaleBuffer;
	bufferDropdown->SetOption(rs.buffer);
	bufferDropdown->Appearance = allowScaleBuffer ? dropApp : dropdownDisabled;

	// Write Thread Label
	writeThreadLabel->SetTextColour(rs.buffer == RecordBuffer::Off ? ui::Colour(100, 100, 100) : comCl);

	// Write Thread Checkbox
	bool allowWT = rs.buffer != RecordBuffer::Off;
	if (!allowWT && comEn)
	{
		rs.writeThread = false;
	}
	writeThreadCheckbox->Enabled = allowWT ? comEn : false;
	writeThreadCheckbox->SetChecked(rs.writeThread);

	// Quality Label
	if (rs.format == RecordFormat::WebP)
	{
		qualityLabel->SetText(rs.quality == 10 ? "Compres. (M):" : String::Build("Compres. (", rs.quality, "):"));
	}
	else // Why do nested conditional operators have to look so bad
	{
		qualityLabel->SetText("Compres. (-):");
	}
	qualityLabel->SetTextColour(comEn && rs.format == RecordFormat::WebP ? ui::Colour(255, 255, 255) : ui::Colour(100, 100, 100));

	// Quality Slider
	qualitySlider->Enabled = comEn && rs.format == RecordFormat::WebP;
	qualitySlider->SetValue(rs.format == RecordFormat::WebP ? rs.quality : 10);
	if (qualitySlider->Enabled)
	{
		qualitySlider->SetColour(
			rs.quality == 10 ? ui::Colour(0, 95, 0) : ui::Colour(0, 0, 0),
			rs.quality == 10 ? ui::Colour(0, 95, 0) : ui::Colour(0, 63, 0)
		);
	}
	else
	{
		qualitySlider->SetColour(ui::Colour(10, 10, 10), ui::Colour(10, 10, 10));
	}

	// Reset Button
	resetButton->Enabled = comEn;

	// Pause Button
	pauseButton->Enabled = rec;
	pauseButton->SetText(pas ? String::Build("Unpause") : String::Build("Pause"));
	pauseButton->Appearance.BackgroundInactive = pas ? ui::Colour(47, 47, 95) : ui::Colour(15, 15, 31);
	pauseButton->Appearance.BackgroundHover = pas ? ui::Colour(63, 63, 127) : ui::Colour(31, 31, 63);

	// Close Button

	// Start Button
	if (rs.stage == RecordStage::Writing)
	{
		startButton->SetText("Cancel");
	}
	else
	{
		startButton->SetText(rec ? String::Build("Stop") : String::Build("Start"));
	}
	startButton->Appearance.BackgroundInactive = rec ? ui::Colour(95, 47, 47) : ui::Colour(31, 15, 15);
	startButton->Appearance.BackgroundHover = rec ? ui::Colour(127, 63, 63) : ui::Colour(63, 31, 31);
}

void RecordMenu::ExitSetRecord()
{
	auto& rs = state;
	if (rs.stage != RecordStage::Writing)
	{
		if (!RecordingActive(rs.stage) && RecordingActive(newStage))
		{
			con.StartRecording();
		}
		else
		{
			rs.stage = newStage;
		}
	}
	con.SetCallback(nullptr);
	rs.halt = false;
	CloseActiveWindow();
	SelfDestruct();
}

void RecordMenu::OnTryExit(ExitMethod method)
{
	ExitSetRecord();
}

void RecordMenu::OnDraw()
{
	Graphics* g = GetGraphics();
	g->clearrect(Position.X-2, Position.Y-2, Size.X+3, Size.Y+3);
	g->drawrect(Position.X, Position.Y, Size.X, Size.Y, 200, 200, 200, 255);
}

RecordMenu::RecordMenu(RecordState& recordState, RecordController& recordCon) :
	ui::Window(ui::Point(-1, -1), ui::Point(160, 186)),
	con(recordCon),
	state(recordState)
{
	auto& rs = state;
	newStage = rs.stage;
	rs.halt = true;

	// Title Label
	titleLabel = new ui::Label(ui::Point(4, 5), ui::Point(Size.X - 8, 15), "Record Menu");
	titleLabel->SetTextColour(style::Colour::InformationTitle);
	titleLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;

	// Help Button
	ui::Button* helpButton = new ui::Button(ui::Point(Size.X - 20, 4), ui::Point(16, 16), "?");
	helpButton->SetActionCallback({ []() {
		new InformationMessage("Record Menu Help", String::Build("\bo",
			"Select Area:\n\bw\t",
			"Select the area of the screen to record (inclusive).\n\bo",
			"FPS (Max 60):\n\bw\t",
			"Framerate of final recording, dropping frames if necessary. Not affected by lag, but assumes game should be running at 60fps (Ignores tpt.setfpscap).\n\bo",
			"Scale:\n\bl\t",
			"*** Causes extreme lag and memory usage with large areas ***\n\bw\t",
			"Duplicates pixels to make recording larger. Useful if blurry when being upscaled. \"8+Spacing\" adds black areas between pixels (like zoom window).\n\bo",
			"Format:\n\bw\t",
			"Output image format. WebP is much slower at processing frames with long recordings. Gif framerate may be incorrect due to precision limitations (10ms increments). Old disables all settings except FPS.\n\bo",
			"Buffer:\n\bw\t",
			"Stores image data before processing to improve performance while recording. Final image created after recording stops. Ram stores data in memory and is fast, but may use large amounts of ram, especially with the scale option. Disk temporarily stores data in the recordings folder. Size depends on area and scale options.\n\bo",
			"Write Thread:\n\bw\t",
			"Start writing frames to final recording on a separate thread immediately. Can reduce write time with similar game performance if multiple cores are available.\n\bo"
			"Compression Strength (WebP only):\n\bw\t",
			"Amount of effort put into compression. Larger values result in smaller files at the cost of time. Max value (M) enables additional size reduction. \n\bu\t",
			"Note: WebP images are always stored with lossless compression.\n\bo",
			"Reset:\n\bw\t"
			"Reset all settings (including record area) to defaults.\n\bo",
			"Pause:\n\bw\t",
			"Temporarily stops recording.\n\bo",
			"\uE06B:\n\bw\t",
			"Bagel"
			), true);
	} });

	// Select Button
	selectButton = new ui::Button(ui::Point(8, 25), ui::Point(67, 16), "Select Area");
	selectButton->SetActionCallback({ [this]() {
		state.select = 1;
		con.SetCallback(nullptr);
		CloseActiveWindow();
		SelfDestruct();
	} });

	// FPS Label
	fpsLabel = new ui::Label(ui::Point(85, 25), ui::Point(27, 16), "FPS:");
	fpsLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;

	// FPS Textbox
	fpsTextbox = new ui::Textbox(ui::Point(112, 25), ui::Point(40, 16), "60", "[fps]");
	fpsTextbox->SetInputType(ui::Textbox::ValidInput::Number);
	fpsTextbox->SetLimit(4);
	fpsTextbox->SetActionCallback({ [this]() {
		try {
			if (fpsTextbox->GetText().size() > 0)
			{
				state.fps = std::min(fpsTextbox->GetText().ToNumber<int>(), 60);
				StateChanged();
			}
		}
		catch (const std::exception& e)
		{
			new ErrorMessage("Could not set FPS", "Invalid value provided.");
		}
	} });

	// Scale Label
	scaleLabel = new ui::Label(ui::Point(8, 45), ui::Point(67, 16), "Scale:");
	scaleLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;

	// Scale Dropdown
	scaleDropdown = new ui::DropDown(ui::Point(85, 45), ui::Point(67, 16));
	scaleDropdown->AddOption(std::pair<String, int>("1", 1));
	scaleDropdown->AddOption(std::pair<String, int>("2", 2));
	scaleDropdown->AddOption(std::pair<String, int>("4", 4));
	scaleDropdown->AddOption(std::pair<String, int>("8", 8)); // Limited max size due to memory usage
	scaleDropdown->AddOption(std::pair<String, int>("8+Spacing", -8));
	scaleDropdown->SetActionCallback({ [this]() {
		int option = scaleDropdown->GetOption().second;
		state.scale = std::abs(option);
		state.spacing = option < 0;
		StateChanged();
	} });

	// Format Label
	formatLabel = new ui::Label(ui::Point(8, 65), ui::Point(67, 16), "Format:");
	formatLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;

	// Format Dropdown
	formatDropdown = new ui::DropDown(ui::Point(85, 65), ui::Point(67, 16));
	formatDropdown->AddOption(std::pair<String, int>("Gif", RecordFormat::Gif));
	formatDropdown->AddOption(std::pair<String, int>("WebP", RecordFormat::WebP));
	formatDropdown->AddOption(std::pair<String, int>("Old", RecordFormat::Old));
	formatDropdown->SetActionCallback({ [this]() {
		state.format = RecordFormat(formatDropdown->GetOption().second);
		StateChanged();
	} });

	// Buffer Label
	bufferLabel = new ui::Label(ui::Point(8, 85), ui::Point(67, 16), "Buffer:");
	bufferLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;

	// Buffer Dropdown
	bufferDropdown = new ui::DropDown(ui::Point(85, 85), ui::Point(67, 16));
	bufferDropdown->AddOption(std::pair<String, int>("Off", RecordBuffer::Off));
	bufferDropdown->AddOption(std::pair<String, int>("Ram", RecordBuffer::Ram));
	bufferDropdown->AddOption(std::pair<String, int>("Disk", RecordBuffer::Disk));
	bufferDropdown->SetActionCallback({ [this]() {
		state.buffer = RecordBuffer(bufferDropdown->GetOption().second);
		StateChanged();
	} });

	// Write Thread Label
	writeThreadLabel = new ui::Label(ui::Point(8, 105), ui::Point(67, 16), "Write Thread:");
	writeThreadLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;

	// Write Thread Checkbox
	writeThreadCheckbox = new ui::Checkbox(ui::Point(85, 105), ui::Point(16, 16), "", "");
	writeThreadCheckbox->SetActionCallback({ [this]() {
		if (writeThreadCheckbox->Enabled)
		{
			state.writeThread = writeThreadCheckbox->GetChecked();
			StateChanged();
		}
		else
		{
			writeThreadCheckbox->SetChecked(state.writeThread);
		}
	} });

	// Quality Label (Actually compression strength in lossless mode)
	qualityLabel = new ui::Label(ui::Point(8, 125), ui::Point(67, 16), "Compres. (-):");
	qualityLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;

	// Quality Slider
	qualitySlider = new ui::Slider(ui::Point(85, 125), ui::Point(67, 16), 10);
	qualitySlider->SetActionCallback({ [this]() {
		if (qualitySlider->Enabled)
		{
			state.quality = qualitySlider->GetValue();
			StateChanged();
		}
		else
		{
			qualitySlider->SetValue(state.quality);
		}
	} });

	// Reset Button
	resetButton = new ui::Button(ui::Point(8, 145), ui::Point(67, 16), "Reset");
	resetButton->SetActionCallback({ [this]() {
		state.Clear();
		StateChanged();
	} });

	// Pause Button
	pauseButton = new ui::Button(ui::Point(85, 145), ui::Point(67, 16), "Pause");
	pauseButton->Appearance.BackgroundHover = ui::Colour(63, 63, 127);
	pauseButton->SetActionCallback({ [this]() {
		newStage = newStage == RecordStage::Recording ? RecordStage::Paused : RecordStage::Recording;
		StateChanged();
	} });

	// Close Button
	closeButton = new ui::Button(ui::Point(0, Size.Y - 17), ui::Point((Size.X / 2) + 1, 17), "Close");
	closeButton->Appearance.BorderInactive = ui::Colour(200, 200, 200);
	closeButton->SetActionCallback({ [this]() {
		ExitSetRecord();
	} });

	// Start Button
	startButton = new ui::Button(ui::Point(Size.X / 2, Size.Y - 17), ui::Point(Size.X / 2, 17), "Start");
	startButton->Appearance.BorderInactive = ui::Colour(200, 200, 200);
	startButton->SetActionCallback({ [this]() {
		if (state.stage == RecordStage::Writing)
		{
			con.CancelWrite();
		}
		else if (RecordingActive(state.stage) && RecordingActive(newStage))
		{
			if (!ConfirmPrompt::Blocking("Stop Recording", "This will immediately end the recording."))
			{
				return;
			}
			con.StopRecording();
			newStage = state.stage;
		}
		else
		{
			ToggleRecording();
		}
		StateChanged();
	} });

	StateChanged();

	AddComponent(titleLabel);
	AddComponent(helpButton);
	AddComponent(selectButton);
	AddComponent(fpsLabel);
	AddComponent(fpsTextbox);
	AddComponent(scaleLabel);
	AddComponent(scaleDropdown);
	AddComponent(formatLabel);
	AddComponent(formatDropdown);
	AddComponent(bufferLabel);
	AddComponent(bufferDropdown);
	AddComponent(writeThreadLabel);
	AddComponent(writeThreadCheckbox);
	AddComponent(qualityLabel);
	AddComponent(qualitySlider);
	AddComponent(resetButton);
	AddComponent(pauseButton);
	AddComponent(closeButton);
	AddComponent(startButton);

	SetOkayButton(closeButton);
	MakeActiveWindow();
	con.SetCallback([this]() {
		if (state.stage == RecordStage::Stopped)
		{
			newStage = state.stage;
			StateChanged();
		}
		else
		{
			UpdateTitle();
		}
	});
}
