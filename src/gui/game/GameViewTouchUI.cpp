#include "GameView.h"

#include "GameController.h"
#include "GameModel.h"
#include "Tool.h"

#include "client/Client.h"
#include "graphics/Graphics.h"
#include "graphics/Renderer.h"
#include "simulation/SimulationData.h"

#include "gui/interface/Button.h"
#include "gui/interface/Engine.h"
#include "gui/interface/SplitButton.h"

void GameView::InitUI()
{
	int currentX = 1;
	int currentY = Size.Y-31;
	//Set up UI

	searchButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(35, 30), "", "Find & open a simulation. Hold Ctrl to load offline saves.");  //Open
	searchButton->SetIcon(IconOpen);
	searchButton->SetTogglable(false);
	searchButton->SetActionCallback({
		[this] {
			if (CtrlBehaviour())
				c->OpenLocalBrowse();
			else
				c->OpenSearch("");
		},
		[this] { c->OpenLocalBrowse(); }
	});
	AddComponent(searchButton);
	currentX+=36;

	reloadButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(30, 30), "", "Reload the simulation");
	reloadButton->SetIcon(IconReload);
	reloadButton->SetActionCallback({ [this] { c->ReloadSim(); }, [this] { c->OpenSavePreview(); } });
	AddComponent(reloadButton);
	currentX+=31;

	touchSaveSimulationButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(150, 30), "[untitled simulation]", "");
	touchSaveSimulationButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	touchSaveSimulationButton->SetIcon(IconSave);
	touchSaveSimulationButton->SetActionCallback({
		[this] {
			if (Client::Ref().GetAuthUser().UserID)
				c->OpenSaveWindow();
			else
				c->OpenLocalSaveWindow(false);
		},
		[this] { c->OpenLocalSaveWindow(false); }
	});
	SetSaveButtonTooltips();
	AddComponent(touchSaveSimulationButton);
	currentX+=151;

	upVoteButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(50, 30), "", "Like this save");
	upVoteButton->SetIcon(IconVoteUp);
	upVoteButton->Appearance.Margin.Top+=3;
	upVoteButton->Appearance.Margin.Left+=2;
	AddComponent(upVoteButton);
	currentX+=51;

	downVoteButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(30, 30), "", "Dislike this save");
	downVoteButton->SetIcon(IconVoteDown);
	downVoteButton->Appearance.Margin.Bottom+=3;
	downVoteButton->Appearance.Margin.Left+=2;
	AddComponent(downVoteButton);
	currentX+=31;

	tagSimulationButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(180, 30), "[no tags set]", "");
	tagSimulationButton->SetIcon(IconTag);
	tagSimulationButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tagSimulationButton->SetActionCallback({ [this] { c->OpenTags(); } });
	AddComponent(tagSimulationButton);
	currentX+=181;

// Begin main menu

	// Main Menu
	ui::Button * tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(37, 30), "", "Menu");
	tempButton->SetIcon(IconMainMenu);
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { SetTouchMenu(touchMenu == MenuMain ? MenuNone : MenuMain); } });
	tempButton->SetStateFunction([this] { return touchMenu == MenuMain; });
	AddComponent(tempButton);

	currentY = Size.Y-MENUSIZE-38;

	// Stamp Browser
	tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(130, 30), "Stamp Browser", "");
	tempButton->SetIcon(IconStamp);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetActionCallback({
		[this] { c->OpenStamps(); },
		[this] { TryLoadLatestStamp(); }
	});
	AddComponent(tempButton);
	mainMenuButtons.push_back(tempButton);
	currentY-=31;

	// Show HUD
	tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(130, 30), "Show HUD", "");
	tempButton->SetIcon(IconHUD);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { showHud = !showHud; } });
	tempButton->SetStateFunction([this] { return showHud; });
	AddComponent(tempButton);
	mainMenuButtons.push_back(tempButton);
	currentY-=31;

	// Debug HUD
	tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(130, 30), "Debug HUD", "");
	tempButton->SetIcon(IconDebugHUD);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { SetDebugHUD(!GetDebugHUD()); } });
	tempButton->SetStateFunction([this] { return GetDebugHUD(); });
	AddComponent(tempButton);
	mainMenuButtons.push_back(tempButton);
	currentY-=31;

	// Toggle Grid
	tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(70, 30), "Grid", "");
	tempButton->SetIcon(IconGrid);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { ren->SetGridSize(ren->GetGridSize() ? 0 : 1); } }); // Quickly toggle cell grid
	tempButton->SetStateFunction([this] { return ren->GetGridSize(); });
	AddComponent(tempButton);
	mainMenuButtons.push_back(tempButton);

	// Smaller Grid
	tempButton = new ui::Button(ui::Point(currentX + 70, currentY), ui::Point(30, 30), "-", "");
	tempButton->SetActionCallback({ [this] { c->AdjustGridSize(-1); } });
	AddComponent(tempButton);
	mainMenuButtons.push_back(tempButton);

	// Larger Grid
	tempButton = new ui::Button(ui::Point(currentX + 100, currentY), ui::Point(30, 30), "+", "");
	tempButton->SetActionCallback({ [this] { c->AdjustGridSize(1); } });
	AddComponent(tempButton);
	mainMenuButtons.push_back(tempButton);
	currentY-=31;

	// Find Element
	tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(130, 30), "Find Element", "");
	tempButton->SetIcon(IconFind);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { ToggleFind(); } });
	tempButton->SetStateFunction([this] { return (bool)(ren->findingElement); });
	AddComponent(tempButton);
	mainMenuButtons.push_back(tempButton);
	currentY-=31;

	// Single-Step Frame
	tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(130, 30), "Single-Step Frame", "");
	tempButton->SetIcon(IconFrameStep);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetActionCallback({ [this] { c->FrameStep(); } });
	AddComponent(tempButton);
	mainMenuButtons.push_back(tempButton);
	currentY-=31;

	// Login
	loginButton = new ui::SplitButton(ui::Point(currentX, currentY), ui::Point(130, 30), "[sign in]", "Sign into simulation server", "Edit Profile", 23);
	loginButton->SetIcon(IconLogin);
	loginButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	loginButton->Appearance.Margin.Left+=2;
	loginButton->SetSplitActionCallback({
		[this] { c->OpenLogin(); },
		[this] { c->OpenProfile(); }
	});
	AddComponent(loginButton);
	mainMenuButtons.push_back(loginButton);
	currentY-=31;

	// Exit Game
	tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(130, 30), "Exit Game", "");
	tempButton->SetIcon(IconExit);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetActionCallback({ [] { ui::Engine::Ref().ConfirmExit(); } });
	AddComponent(tempButton);
	mainMenuButtons.push_back(tempButton);
	currentY-=31;

	// Authorship Info
	authorshipInfoButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(130, 30), "Authorship Info", "");
	authorshipInfoButton->SetIcon(Icon1984);
	authorshipInfoButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	authorshipInfoButton->Appearance.Margin.Left+=2;
	authorshipInfoButton->SetActionCallback({ [this] { ShowAuthorshipInfo(); } });
	AddComponent(authorshipInfoButton);
	touchMenuButtons.push_back(authorshipInfoButton); // Do not show with main menu by default
	currentY-=31;

	for (auto i : mainMenuButtons)
	{
		touchMenuButtons.push_back(i);
	}

// End main menu

	currentX+=38;
	currentY = Size.Y-31;

	clearSimButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(30, 30), "", "Erase everything");
	clearSimButton->SetIcon(IconNew);
	clearSimButton->Appearance.Margin.Left+=2;
	clearSimButton->SetActionCallback({ [this] { c->ClearSim(); } });
	AddComponent(clearSimButton);
	currentX+=31;

	simulationOptionButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(30, 30), "", "Simulation options");
	simulationOptionButton->SetIcon(IconSimulationSettings);
	simulationOptionButton->Appearance.Margin.Left+=2;
	simulationOptionButton->SetActionCallback({ [this] { c->OpenOptions(); } });
	AddComponent(simulationOptionButton);
	currentX+=31;

	displayModeButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(30, 30), "", "Renderer options");
	displayModeButton->SetIcon(IconRenderSettings);
	displayModeButton->Appearance.Margin.Left+=2;
	displayModeButton->SetActionCallback({ [this] { c->OpenRenderOptions(); } });
	AddComponent(displayModeButton);
	currentX+=31;

	pauseButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(30, 30), "", "Pause/Resume the simulation");
	pauseButton->SetIcon(IconPause);
	pauseButton->Appearance.Margin.Left+=2;
	pauseButton->SetTogglable(true);
	pauseButton->SetActionCallback({ [this] { c->SetPaused(pauseButton->GetToggleState()); } });
	AddComponent(pauseButton);

	// Element Selection Menu
	currentY-=61;
	tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(30, 60), "", "Select Element");
	tempButton->SetIcon(IconPrettyPowder);
	tempButton->SetActionCallback({ [this] { c->OpenElementSearch(); } });
	AddComponent(tempButton);

	colourPicker = new ui::Button(ui::Point((XRES/2)-8, YRES+1), ui::Point(16, 16), "", "Pick Colour");
	colourPicker->SetActionCallback({ [this] { c->OpenColourPicker(); } });

	// Zoom Window
	tempButton = new ui::Button(ui::Point(currentX, 94), ui::Point(30, 30), "", "Zoom Window");
	tempButton->SetIcon(IconZoom);
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] {
		if (zoomEnabled || enableZoomOnTouch)
		{
			c->SetZoomEnabled(false);
			enableZoomOnTouch = false;
		}
		else
		{
			enableZoomOnTouch = true;
		}
	} });
	tempButton->SetStateFunction([this] { return zoomEnabled || enableZoomOnTouch; });
	AddComponent(tempButton);

	// Toggle Erase Tool
	tempButton = new ui::Button(ui::Point(currentX, 63), ui::Point(30, 30), "", "Erase Tool");
	tempButton->SetIcon(IconErase);
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] {
		if (c->GetActiveTool(ToolSelection::ToolPrimary)->Identifier == "DEFAULT_PT_NONE")
		{
			c->SetActiveTool(ToolSelection::ToolPrimary, c->GetActiveTool(ToolSelection::ToolSecondary));
			c->SetActiveTool(ToolSelection::ToolSecondary, "DEFAULT_PT_NONE");
		}
		else
		{
			c->SetActiveTool(ToolSelection::ToolSecondary, c->GetActiveTool(ToolSelection::ToolPrimary));
			c->SetActiveTool(ToolSelection::ToolPrimary, "DEFAULT_PT_NONE");
		}
	} });
	tempButton->SetStateFunction([this] { return c->GetActiveTool(ToolSelection::ToolPrimary)->Identifier == "DEFAULT_PT_NONE"; });
	AddComponent(tempButton);

// Begin brush options menu

	currentY = 32;

	// Brush Menu
	tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(30, 30), "", "Brush Options");
	tempButton->SetIcon(IconBrushOptionsMenu);
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { SetTouchMenu(touchMenu == MenuBrushOptions ? MenuNone : MenuBrushOptions); } });
	tempButton->SetStateFunction([this] { return touchMenu == MenuBrushOptions; });
	AddComponent(tempButton);

	// Circle Brush
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(50, 30), "", "");
	tempButton->SetIcon(IconBrushCircle);
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetBrushID(0); } });
	tempButton->SetStateFunction([this] { return c->GetBrushID() == 0; });
	AddComponent(tempButton);
	brushOptionsMenuButtons.push_back(tempButton);

	// Square Brush
	tempButton = new ui::Button(ui::Point(currentX - 81, currentY), ui::Point(40, 30), "", "");
	tempButton->SetIcon(IconBrushSquare);
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetBrushID(1); } });
	tempButton->SetStateFunction([this] { return c->GetBrushID() == 1; });
	AddComponent(tempButton);
	brushOptionsMenuButtons.push_back(tempButton);

	// Triangle Brush
	tempButton = new ui::Button(ui::Point(currentX - 41, currentY), ui::Point(40, 30), "", "");
	tempButton->SetIcon(IconBrushTriangle);
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetBrushID(2); } });
	tempButton->SetStateFunction([this] { return c->GetBrushID() == 2; });
	AddComponent(tempButton);
	brushOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Smaller Brush
	smallerBrushButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(65, 30), "-", "");
	smallerBrushButton->SetActionCallback({ [this] { c->AdjustBrushSize(-1); } });
	AddComponent(smallerBrushButton);
	brushOptionsMenuButtons.push_back(smallerBrushButton);

	// Larger Brush
	largerBrushButton = new ui::Button(ui::Point(currentX - 66, currentY), ui::Point(65, 30), "+", "");
	largerBrushButton->SetActionCallback({ [this] { c->AdjustBrushSize(1); } });
	AddComponent(largerBrushButton);
	brushOptionsMenuButtons.push_back(largerBrushButton);
	currentY+=31;

	// Replace Mode
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(65, 30), "Replace", "");
	tempButton->SetIcon(IconReplace);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetReplaceModeFlags(c->GetReplaceModeFlags()^REPLACE_MODE); } });
	tempButton->SetStateFunction([this] { return c->GetReplaceModeFlags()&REPLACE_MODE; });
	AddComponent(tempButton);
	brushOptionsMenuButtons.push_back(tempButton);

	// Specific Delete Mode
	tempButton = new ui::Button(ui::Point(currentX - 66, currentY), ui::Point(65, 30), "Specific\nDelete", "");
	tempButton->SetIcon(IconSpecificDelete);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetReplaceModeFlags(c->GetReplaceModeFlags()^SPECIFIC_DELETE); } });
	tempButton->SetStateFunction([this] { return c->GetReplaceModeFlags()&SPECIFIC_DELETE; });
	AddComponent(tempButton);
	brushOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Copy
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(40, 30), "", "");
	tempButton->SetIcon(IconCopy);
	tempButton->SetActionCallback({ [this] { BeginCopy(); } });
	AddComponent(tempButton);
	brushOptionsMenuButtons.push_back(tempButton);

	// Cut
	tempButton = new ui::Button(ui::Point(currentX - 91, currentY), ui::Point(40, 30), "", "");
	tempButton->SetIcon(IconCut);
	tempButton->SetActionCallback({ [this] { BeginCut(); } });
	AddComponent(tempButton);
	brushOptionsMenuButtons.push_back(tempButton);

	// Paste
	pasteButton = new ui::Button(ui::Point(currentX - 51, currentY), ui::Point(50, 30), "Paste", "");
	pasteButton->SetIcon(IconStamp);
	pasteButton->SetActionCallback({ [this] { BeginPaste(); } });
	AddComponent(pasteButton);
	brushOptionsMenuButtons.push_back(pasteButton);
	currentY+=31;

	// Create Stamp
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Create Stamp", "");
	tempButton->SetIcon(IconStamp);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetActionCallback({ [this] { BeginStampSelection(); } });
	AddComponent(tempButton);
	brushOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Undo
	undoButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(65, 30), "Undo", "");
	undoButton->SetIcon(IconUndo);
	undoButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	undoButton->Appearance.Margin.Left+=2;
	undoButton->SetActionCallback({ [this] { c->HistoryRestore(); } });
	undoButton->Enabled = false;
	brushOptionsMenuButtons.push_back(undoButton);
	AddComponent(undoButton);

	// Redo
	redoButton = new ui::Button(ui::Point(currentX - 66, currentY), ui::Point(65, 30), "Redo", "");
	redoButton->SetIcon(IconRedo);
	redoButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	redoButton->Appearance.Margin.Left+=2;
	redoButton->SetActionCallback({ [this] { c->HistoryForward(); } });
	redoButton->Enabled = false;
	brushOptionsMenuButtons.push_back(redoButton);
	AddComponent(redoButton);
	currentY+=31;

	for (auto i : brushOptionsMenuButtons)
	{
		touchMenuButtons.push_back(i);
	}

// End brush options menu

// Begin quick options menu

	currentY = 1;

	// Quick Options Menu
	tempButton = new ui::Button(ui::Point(currentX, currentY), ui::Point(30, 30), "", "Quick Options");
	tempButton->SetIcon(IconQuickOptionsMenu);
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { SetTouchMenu(touchMenu == MenuQuickOptions ? MenuNone : MenuQuickOptions); } });
	tempButton->SetStateFunction([this] { return touchMenu == MenuQuickOptions; });
	AddComponent(tempButton);

	// Sand Effect
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Sand Effect", "");
	tempButton->SetIcon(IconPrettyPowder);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetPrettyPowder(); } });
	tempButton->SetStateFunction([this] { return c->GetPrettyPowder(); });
	AddComponent(tempButton);
	quickOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Draw Gravity Grid
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Draw Gravity Field", "");
	tempButton->SetIcon(IconGravityField);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetGravityGrid(); } });
	tempButton->SetStateFunction([this] { return c->GetGravityGrid(); });
	AddComponent(tempButton);
	quickOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Draw Decorations
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Draw Decorations", "");
	tempButton->SetIcon(IconDecoration);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->SetDecoration(); } });
	tempButton->SetStateFunction([this] { return c->GetDecoration(); });
	AddComponent(tempButton);
	quickOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Newtonian Gravity
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Newtonian Gravity", "");
	tempButton->SetIcon(IconNewtonianGravity);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->ToggleNewtonianGravity(); } });
	tempButton->SetStateFunction([this] { return c->GetNewtonianGravity(); });
	AddComponent(tempButton);
	quickOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Ambient Heat
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Ambient Heat", "");
	tempButton->SetIcon(IconAmbientHeat);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetTogglable(true);
	tempButton->SetActionCallback({ [this] { c->ToggleAHeat(); } });
	tempButton->SetStateFunction([this] { return c->GetAHeatEnable(); });
	AddComponent(tempButton);
	quickOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Show Console
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Show Console", "");
	tempButton->SetIcon(IconConsole);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetActionCallback({ [this] { c->ShowConsole(); } });
	AddComponent(tempButton);
	quickOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Reset Spark
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Reset Spark", "");
	tempButton->SetIcon(IconResetSpark);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetActionCallback({ [this] { c->ResetSpark(); } });
	AddComponent(tempButton);
	quickOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Reset Ambient Heat
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Reset Ambient Heat", "");
	tempButton->SetIcon(IconAmbientHeat);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetActionCallback({ [this] { c->ResetAHeat(); } });
	AddComponent(tempButton);
	quickOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Reset Air
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Reset Air", "");
	tempButton->SetIcon(IconResetAir);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetActionCallback({ [this] { c->ResetAir(); } });
	AddComponent(tempButton);
	quickOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	// Invert Air
	tempButton = new ui::Button(ui::Point(currentX - 131, currentY), ui::Point(130, 30), "Invert Air", "");
	tempButton->SetIcon(IconInvertAir);
	tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	tempButton->Appearance.Margin.Left+=2;
	tempButton->SetActionCallback({ [this] { c->InvertAirSim(); } });
	AddComponent(tempButton);
	quickOptionsMenuButtons.push_back(tempButton);
	currentY+=31;

	for (auto i : quickOptionsMenuButtons)
	{
		touchMenuButtons.push_back(i);
	}

// End quick options menu

	for (auto i : touchMenuButtons)
	{
		i->Position.X *= -1;
		i->Visible = false;
	}
}

void GameView::SetTouchMenu(TouchMenu newTouchMenu)
{
	touchMenu = newTouchMenu;

	auto setButtonVisible = [] (ui::Button* b, bool v) {
		if ((v && b->Position.X < 0) || (!v && b->Position.X > 0))
		{
			b->Position.X *= -1;
		}
		b->Visible = v;
	};

	for (auto i : touchMenuButtons)
	{
		setButtonVisible(i, false);
	}

	switch (touchMenu)
	{
		case MenuNone:
			break;

		case MenuMain:
			for (auto i : mainMenuButtons)
			{
				setButtonVisible(i, true);
			}
			if (Client::Ref().GetAuthUser().UserElevation != User::ElevationNone)
			{
				setButtonVisible(authorshipInfoButton, true);
			}
			break;

		case MenuBrushOptions:
			for (auto i : brushOptionsMenuButtons)
			{
				setButtonVisible(i, true);
			}
			break;

		case MenuQuickOptions:
			for (auto i : quickOptionsMenuButtons)
			{
				setButtonVisible(i, true);
			}
			break;
	}
}

void GameView::SetSaveButtonTooltips()
{
	if (!Client::Ref().GetAuthUser().UserID)
		touchSaveSimulationButton->SetToolTip("Save the simulation to your hard drive. Login to save online.");
	else
		touchSaveSimulationButton->SetToolTip("Upload the simulation. Hold to save offline.");
}

void GameView::ToolTip(ui::Point senderPosition, String toolTip)
{
	int offsetX = BARSIZE + 10 + (Graphics::TextSize(toolTip).X - 1);

	if (
		(touchMenu == MenuMain         && senderPosition.Y > Size.Y-MENUSIZE-270) ||
		(touchMenu == MenuBrushOptions && senderPosition.Y > 30 && senderPosition.Y < 220) ||
		(touchMenu == MenuQuickOptions && senderPosition.Y < 310)
	)
	{
		offsetX += 131;
	}
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
		toolTipPosition = ui::Point(Size.X-offsetX, senderPosition.Y+3);
		if(toolTipPosition.Y+10 > Size.Y-MENUSIZE)
			toolTipPosition = ui::Point(Size.X-offsetX, Size.Y-MENUSIZE-10);
		isToolTipFadingIn = true;
	}
	// element tooltips
	else
	{
		this->toolTip = toolTip;
		toolTipPosition = ui::Point(Size.X-offsetX, Size.Y-MENUSIZE-10);
		isToolTipFadingIn = true;
	}
}
