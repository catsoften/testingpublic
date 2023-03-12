#include "ModOptionsView.h"

#include "OptionsController.h"
#include "OptionsModel.h"
#include <cstring>

#include <cstdio>
#ifdef WIN
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#endif
#include "SDLCompat.h"

#include "gui/Style.h"
#include "gui/interface/Button.h"
#include "gui/interface/Label.h"
#include "gui/interface/DropDown.h"
#include "gui/interface/Engine.h"
#include "gui/interface/Checkbox.h"

#include "graphics/Graphics.h"

ModOptionsView::ModOptionsView() { // Originally 320, 340
	const ui::Colour CHECKBOX_SUBTEXT_COLOR(150, 150, 150);	// Gray shade for subtext for checkboxes

	auto autowidth = [this](ui::Component *c) {
		c->Size.X = Size.X - c->Position.X - 12;
	};
	
	ui::Label * tempLabel = new ui::Label(ui::Point(4, 1), ui::Point(Size.X-8, 22), "Mod Options");
	tempLabel->SetTextColour(style::Colour::InformationTitle);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	autowidth(tempLabel);
	AddComponent(tempLabel);

	class Separator : public ui::Component {
		public:
		Separator(ui::Point position, ui::Point size) : Component(position, size){}
		virtual ~Separator(){}

		void Draw(const ui::Point& screenPos) override {
			GetGraphics()->drawrect(screenPos.X, screenPos.Y, Size.X, Size.Y, 255, 255, 255, 180);
		}		
	};
	
	Separator *tmpSeparator = new Separator(ui::Point(0, 22), ui::Point(Size.X, 1));
	AddComponent(tmpSeparator);

	int currentY = 6;
	scrollPanel = new ui::ScrollPanel(ui::Point(1, 23), ui::Point(Size.X-2, Size.Y-39));
	
	AddComponent(scrollPanel);

	// Mod settings
	// ------------------
	currentY += 5;
	autoOppositeTool = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), "Opposite Tool", "");
	autowidth(autoOppositeTool);
	autoOppositeTool->SetActionCallback({[this] { c->SetOppositeToolEnabled(autoOppositeTool->GetChecked()); }});

	currentY += 12;
	tempLabel = new ui::Label(ui::Point(autoOppositeTool->Position.X + 12, currentY), ui::Point(1, 16), "- Right click uses opposite tool (ie HEAT / COOL)");
	tempLabel->SetTextColour(CHECKBOX_SUBTEXT_COLOR);

	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(autoOppositeTool);

	// ------------------
	// currentY += 20;
	secretModShortcut = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), "Secret Mod Shortcut", "");
	autowidth(secretModShortcut);
	secretModShortcut->SetActionCallback({[this] { c->SetSecretModShortcut(secretModShortcut->GetChecked()); }});

	// currentY += 12;
	tempLabel = new ui::Label(ui::Point(autoOppositeTool->Position.X + 12, currentY), ui::Point(1, 16), "- Ctrl-A views save info. Pretend to be jacob1.");
	tempLabel->SetTextColour(CHECKBOX_SUBTEXT_COLOR);

	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	// Removed per request of jacob1
	// scrollPanel->AddChild(tempLabel);
	// scrollPanel->AddChild(secretModShortcut);

	// ------------------
	currentY += 20;
	crosshairInBrush = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), "Display Crosshair in Brush", "");
	autowidth(crosshairInBrush);
	crosshairInBrush->SetActionCallback({[this] { c->SetCrosshairInBrush(crosshairInBrush->GetChecked()); }});

	currentY += 12;
	tempLabel = new ui::Label(ui::Point(crosshairInBrush->Position.X + 12, currentY), ui::Point(1, 16), "- Displays a crosshair at the center of large brushes");
	tempLabel->SetTextColour(CHECKBOX_SUBTEXT_COLOR);

	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(crosshairInBrush);

	// ------------------
	currentY += 20;
	hollowBrushes = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), "Hollow Brushes", "");
	autowidth(hollowBrushes);
	hollowBrushes->SetActionCallback({[this] { c->SetHollowBrushes(hollowBrushes->GetChecked()); }});

	currentY += 12;
	tempLabel = new ui::Label(ui::Point(hollowBrushes->Position.X + 12, currentY), ui::Point(1, 16), "- Adds hollow brush variants, toggle with SHIFT-TAB");
	tempLabel->SetTextColour(CHECKBOX_SUBTEXT_COLOR);

	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(hollowBrushes);

    currentY += 20;
    tmpSeparator = new Separator(ui::Point(0, currentY), ui::Point(Size.X, 1));
	scrollPanel->AddChild(tmpSeparator);
    currentY += 5;

	// ------------------
	autoHideHUD = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), "Auto-hide HUD", "");
	autowidth(autoHideHUD);
	autoHideHUD->SetActionCallback({[this] { c->SetAutoHideHUD(autoHideHUD->GetChecked()); }});

	currentY += 12;
	tempLabel = new ui::Label(ui::Point(autoHideHUD->Position.X + 12, currentY), ui::Point(1, 16), "- Fades the HUD when mouse is near the top of the screen.");
	tempLabel->SetTextColour(CHECKBOX_SUBTEXT_COLOR);

	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
	scrollPanel->AddChild(autoHideHUD);

    // ------------------
	currentY += 20;
	dimGlowMode = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), "Dim glow effects", "");
	autowidth(dimGlowMode);
	dimGlowMode->SetActionCallback({[this] { c->SetDimGlowMode(dimGlowMode->GetChecked()); }});

	currentY += 12;
    tempLabel = new ui::Label(ui::Point(dimGlowMode->Position.X + 12, currentY), ui::Point(1, 16),
		"- Dimmer glow effects on certain particles (mostly energy).");
    tempLabel->SetTextColour(CHECKBOX_SUBTEXT_COLOR);

	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
    scrollPanel->AddChild(dimGlowMode);

	// ------------------
	currentY += 20;
	drawingFrqLimit = new ui::DropDown(ui::Point(Size.X-95, currentY), ui::Point(80, 16));
	scrollPanel->AddChild(drawingFrqLimit);
	int max = ui::Engine::Ref().GetMaxDrawingFrequencyLimit();
	drawingFrqLimit->AddOption(std::pair<String, int>(String::Build("Auto (", max, ")"), max));
	if (max > 120) drawingFrqLimit->AddOption(std::pair<String, int>("240 Hz", 240));
	if (max > 60)  drawingFrqLimit->AddOption(std::pair<String, int>("120 Hz", 120));
	if (max > 30)  drawingFrqLimit->AddOption(std::pair<String, int>("60 Hz", 60));
	drawingFrqLimit->AddOption(std::pair<String, int>("30 Hz", 30));
	drawingFrqLimit->AddOption(std::pair<String, int>("20 Hz", 20));
	drawingFrqLimit->AddOption(std::pair<String, int>("10 Hz", 10));
	drawingFrqLimit->AddOption(std::pair<String, int>("5 Hz", 5));
	drawingFrqLimit->SetActionCallback({ [this] { c->SetDrawingFrequencyLimit(drawingFrqLimit->GetOption().second); } });

	tempLabel = new ui::Label(ui::Point(8, currentY), ui::Point(Size.X - 96, 16), "Redraw frequency (simulation isn't affected)");
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);

	// ------------------
	currentY += 20;
	fasterRenderer = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), "Faster Renderer", "");
	autowidth(fasterRenderer);
	fasterRenderer->SetActionCallback({[this] { c->SetFasterRenderer(fasterRenderer->GetChecked()); }});

	currentY += 12;
    tempLabel = new ui::Label(ui::Point(fasterRenderer->Position.X + 12, currentY), ui::Point(1, 16),
		"- Use fancy optimizations that might break your renderer.");
    tempLabel->SetTextColour(CHECKBOX_SUBTEXT_COLOR);

	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
    scrollPanel->AddChild(fasterRenderer);

	// ------------------
	/* currentY += 20;
	soundEnabled = new ui::Checkbox(ui::Point(8, currentY), ui::Point(1, 16), "Sound", "");
	autowidth(soundEnabled);
	soundEnabled->SetActionCallback({[this] { c->SetSoundEnabled(soundEnabled->GetChecked()); }});

	currentY += 12;
    tempLabel = new ui::Label(ui::Point(soundEnabled->Position.X + 12, currentY), ui::Point(1, 16),
		"- Enable sound (such as NOTE and lua scripts).");
    tempLabel->SetTextColour(CHECKBOX_SUBTEXT_COLOR);

	autowidth(tempLabel);
	tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	scrollPanel->AddChild(tempLabel);
    scrollPanel->AddChild(soundEnabled); */ // *ULTIMATA97*

    // ------------------

	ui::Button * tempButton = new ui::Button(ui::Point(0, Size.Y-16), ui::Point(Size.X, 16), "OK");
	tempButton->SetActionCallback({ [this] { c->Exit(); } });
	AddComponent(tempButton);
	SetCancelButton(tempButton);
	SetOkayButton(tempButton);
	currentY+=20;
	scrollPanel->InnerSize = ui::Point(Size.X, currentY);
}

void ModOptionsView::NotifySettingsChanged(OptionsModel * sender) {
	autoOppositeTool->SetChecked(sender->GetAutoSelectOppositeTool());
	secretModShortcut->SetChecked(sender->GetSecretModShortcut());
	crosshairInBrush->SetChecked(sender->GetCrosshairInBrush());
	hollowBrushes->SetChecked(sender->GetHollowBrushes());
	autoHideHUD->SetChecked(sender->GetAutoHideHUD());
    dimGlowMode->SetChecked(sender->GetDimGlowMode());
	drawingFrqLimit->SetOption(sender->GetDrawingFrequencyLimit());
	fasterRenderer->SetChecked(sender->GetFasterRenderer());
	//soundEnabled->SetChecked(sender->GetSoundEnabled()); *ULTIMATA97*
}

ModOptionsView::~ModOptionsView() {}
