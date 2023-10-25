#include "GameView.h"

#include "GameController.h"

#include "client/Client.h"
#include "graphics/Graphics.h"

#include "gui/interface/Button.h"
#include "gui/interface/SplitButton.h"

void GameView::InitUI()
{
	int currentX = 1;
	//Set up UI

	scrollBar = new ui::Button(ui::Point(0,YRES+21), ui::Point(XRES, 2), "");
	scrollBar->Appearance.BorderHover = ui::Colour(200, 200, 200);
	scrollBar->Appearance.BorderActive = ui::Colour(200, 200, 200);
	scrollBar->Appearance.HorizontalAlign = ui::Appearance::AlignCentre;
	scrollBar->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	AddComponent(scrollBar);

	searchButton = new ui::Button(ui::Point(currentX, Size.Y-16), ui::Point(17, 15), "", "Find & open a simulation. Hold Ctrl to load offline saves.");  //Open
	searchButton->SetIcon(IconOpen);
	searchButton->SetTogglable(false);
	searchButton->SetActionCallback({ [this] {
		if (CtrlBehaviour())
			c->OpenLocalBrowse();
		else
			c->OpenSearch("");
	} });
	AddComponent(searchButton);
	currentX+=18;

	reloadButton = new ui::Button(ui::Point(currentX, Size.Y-16), ui::Point(17, 15), "", "Reload the simulation");
	reloadButton->SetIcon(IconReload);
	reloadButton->Appearance.Margin.Left+=2;
	reloadButton->SetActionCallback({ [this] { c->ReloadSim(); }, [this] { c->OpenSavePreview(); } });
	AddComponent(reloadButton);
	currentX+=18;

	saveSimulationButton = new ui::SplitButton(ui::Point(currentX, Size.Y-16), ui::Point(150, 15), "[untitled simulation]", "", "", 19);
	saveSimulationButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	saveSimulationButton->SetIcon(IconSave);
	saveSimulationButton->SetSplitActionCallback({
		[this] {
			if (CtrlBehaviour() || !Client::Ref().GetAuthUser().UserID)
				c->OpenLocalSaveWindow(true);
			else
				c->SaveAsCurrent();
		},
		[this] {
			if (CtrlBehaviour() || !Client::Ref().GetAuthUser().UserID)
				c->OpenLocalSaveWindow(false);
			else
				c->OpenSaveWindow();
		}
	});
	SetSaveButtonTooltips();
	AddComponent(saveSimulationButton);
	currentX+=151;

	upVoteButton = new ui::Button(ui::Point(currentX, Size.Y-16), ui::Point(39, 15), "", "Like this save");
	upVoteButton->SetIcon(IconVoteUp);
	upVoteButton->Appearance.Margin.Top+=4;
	upVoteButton->Appearance.Margin.Left+=2;
	AddComponent(upVoteButton);
	currentX+=38;

	downVoteButton = new ui::Button(ui::Point(currentX, Size.Y-16), ui::Point(15, 15), "", "Dislike this save");
	downVoteButton->SetIcon(IconVoteDown);
	downVoteButton->Appearance.Margin.Bottom+=2;
	downVoteButton->Appearance.Margin.Left+=2;
	AddComponent(downVoteButton);
	currentX+=16;

	tagSimulationButton = new ui::Button(ui::Point(currentX, Size.Y-16), ui::Point(227, 15), "[no tags set]", "Add simulation tags");
	tagSimulationButton->SetIcon(IconTag);
	tagSimulationButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	//currentX+=252;
	tagSimulationButton->SetActionCallback({ [this] { c->OpenTags(); } });
	AddComponent(tagSimulationButton);

	clearSimButton = new ui::Button(ui::Point(Size.X-159, Size.Y-16), ui::Point(17, 15), "", "Erase everything");
	clearSimButton->SetIcon(IconNew);
	clearSimButton->Appearance.Margin.Left+=2;
	clearSimButton->SetActionCallback({ [this] { c->ClearSim(); } });
	AddComponent(clearSimButton);

	loginButton = new ui::SplitButton(ui::Point(Size.X-141, Size.Y-16), ui::Point(92, 15), "[sign in]", "Sign into simulation server", "Edit Profile", 19);
	loginButton->SetIcon(IconLogin);
	loginButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	loginButton->SetSplitActionCallback({
		[this] { c->OpenLogin(); },
		[this] { c->OpenProfile(); }
	});
	AddComponent(loginButton);

	simulationOptionButton = new ui::Button(ui::Point(Size.X-48, Size.Y-16), ui::Point(15, 15), "", "Simulation options");
	simulationOptionButton->SetIcon(IconSimulationSettings);
	simulationOptionButton->Appearance.Margin.Left+=2;
	simulationOptionButton->SetActionCallback({ [this] { c->OpenOptions(); } });
	AddComponent(simulationOptionButton);

	displayModeButton = new ui::Button(ui::Point(Size.X-32, Size.Y-16), ui::Point(15, 15), "", "Renderer options");
	displayModeButton->SetIcon(IconRenderSettings);
	displayModeButton->Appearance.Margin.Left+=2;
	displayModeButton->SetActionCallback({ [this] { c->OpenRenderOptions(); } });
	AddComponent(displayModeButton);

	pauseButton = new ui::Button(ui::Point(Size.X-16, Size.Y-16), ui::Point(15, 15), "", "Pause/Resume the simulation");  //Pause
	pauseButton->SetIcon(IconPause);
	pauseButton->SetTogglable(true);
	pauseButton->SetActionCallback({ [this] { c->SetPaused(pauseButton->GetToggleState()); } });
	AddComponent(pauseButton);

	ui::Button * tempButton = new ui::Button(ui::Point(WINDOWW-16, WINDOWH-32), ui::Point(15, 15), 0xE065, "Search for elements");
	tempButton->Appearance.Margin = ui::Border(0, 2, 3, 2);
	tempButton->SetActionCallback({ [this] { c->OpenElementSearch(); } });
	AddComponent(tempButton);

	colourPicker = new ui::Button(ui::Point((XRES/2)-8, YRES+1), ui::Point(16, 16), "", "Pick Colour");
	colourPicker->SetActionCallback({ [this] { c->OpenColourPicker(); } });

	int currentY = 1;

	tempButton = new ui::Button(ui::Point(WINDOWW-16, currentY), ui::Point(15, 15), "P", "Sand effect");
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetPrettyPowder(); } });
	tempButton->SetStateFunction([this] { return c->GetPrettyPowder(); });
	AddComponent(tempButton);
	currentY+=16;

	tempButton = new ui::Button(ui::Point(WINDOWW-16, currentY), ui::Point(15, 15), "G", "Draw gravity field \bg(ctrl+g)");
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetGravityGrid(); } });
	tempButton->SetStateFunction([this] { return c->GetGravityGrid(); });
	AddComponent(tempButton);
	currentY+=16;

	tempButton = new ui::Button(ui::Point(WINDOWW-16, currentY), ui::Point(15, 15), "D", "Draw decorations \bg(ctrl+b)");
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetDecoration(); } });
	tempButton->SetStateFunction([this] { return c->GetDecoration(); });
	AddComponent(tempButton);
	currentY+=16;

	tempButton = new ui::Button(ui::Point(WINDOWW-16, currentY), ui::Point(15, 15), "N", "Newtonian Gravity \bg(n)");
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->ToggleNewtonianGravity(); } });
	tempButton->SetStateFunction([this] { return c->GetNewtonianGravity(); });
	AddComponent(tempButton);
	currentY+=16;

	tempButton = new ui::Button(ui::Point(WINDOWW-16, currentY), ui::Point(15, 15), "A", "Ambient heat \bg(u)");
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->ToggleAHeat(); } });
	tempButton->SetStateFunction([this] { return c->GetAHeatEnable(); });
	AddComponent(tempButton);
	currentY+=16;

	tempButton = new ui::Button(ui::Point(WINDOWW-16, currentY), ui::Point(15, 15), "C", "Show Console \bg(~)");
	tempButton->SetActionCallback({ [this] { c->ShowConsole(); } });
	AddComponent(tempButton);
	currentY+=16;
}

void GameView::SetTouchMenu(TouchMenu newTouchMenu)
{

}

void GameView::SetSaveButtonTooltips()
{
	if (!Client::Ref().GetAuthUser().UserID)
		saveSimulationButton->SetToolTips("Overwrite the open simulation on your hard drive.", "Save the simulation to your hard drive. Login to save online.");
	else if (ctrlBehaviour)
		saveSimulationButton->SetToolTips("Overwrite the open simulation on your hard drive.", "Save the simulation to your hard drive.");
	else if (saveSimulationButton->GetShowSplit())
		saveSimulationButton->SetToolTips("Re-upload the current simulation", "Modify simulation properties");
	else
		saveSimulationButton->SetToolTips("Re-upload the current simulation", "Upload a new simulation. Hold Ctrl to save offline.");
}

void GameView::ToolTip(ui::Point senderPosition, String toolTip)
{
	// buttom button tooltips
	if (senderPosition.Y > Size.Y-17)
	{
		if (selectMode == PlaceSave || selectMode == SelectNone)
		{
			buttonTip = toolTip;
			isButtonTipFadingIn = true;
		}
	}
	// quickoption and menu tooltips
	else if(senderPosition.X > Size.X-BARSIZE)// < Size.Y-(quickOptionButtons.size()+1)*16)
	{
		this->toolTip = toolTip;
		toolTipPosition = ui::Point(Size.X-27-(Graphics::TextSize(toolTip).X - 1), senderPosition.Y+3);
		if(toolTipPosition.Y+10 > Size.Y-MENUSIZE)
			toolTipPosition = ui::Point(Size.X-27-(Graphics::TextSize(toolTip).X - 1), Size.Y-MENUSIZE-10);
		isToolTipFadingIn = true;
	}
	// element tooltips
	else
	{
		this->toolTip = toolTip;
		toolTipPosition = ui::Point(Size.X-27-(Graphics::TextSize(toolTip).X - 1), Size.Y-MENUSIZE-10);
		isToolTipFadingIn = true;
	}
}
