#include "GameView.h"

#include "DecorationTool.h"
#include "Favorite.h"
#include "GameController.h"
#include "GameModel.h"
#include "Menu.h"
#include "MenuButton.h"
#include "Tool.h"
#include "ToolButton.h"

#include "client/Client.h"
#include "graphics/Graphics.h"
#include "gui/dialogues/ConfirmPrompt.h"
#include "gui/interface/Button.h"
#include "gui/interface/SplitButton.h"
#include "simulation/SimulationData.h"

GameViewBasic::GameViewBasic()
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

void GameViewBasic::SetTouchMenu(TouchMenu newTouchMenu)
{

}

void GameViewBasic::updateToolButtonScroll()
{
	if (toolButtons.size())
	{
		int x = currentMouse.X;
		int y = currentMouse.Y;

		int offsetDelta = 0;

		int newInitialX = WINDOWW - 56;
		int totalWidth = (toolButtons[0]->Size.X + 1) * toolButtons.size();
		int scrollSize = (int)(((float)(XRES - BARSIZE))/((float)totalWidth) * ((float)XRES - BARSIZE));

		if (scrollSize > XRES - 1)
			scrollSize = XRES - 1;

		if (totalWidth > XRES - 15)
		{
			int mouseX = x;

			float overflow = 0;
			float mouseLocation = 0;

			if (mouseX > XRES)
				mouseX = XRES;

			// if (mouseX < 15) // makes scrolling a little nicer at edges but apparently if you put hundreds of elements in a menu it makes the end not show ...
			// 	mouseX = 15;

			scrollBar->Visible = true;

			scrollBar->Position.X = (int)(((float)mouseX / (float)XRES) * (float)(XRES - scrollSize)) + 1;

			overflow = (float)(totalWidth - (XRES - BARSIZE));
			mouseLocation = (float)(XRES - 3)/(float)((XRES - 2) - mouseX); // mouseLocation adjusted slightly in case you have 200 elements in one menu

			newInitialX += (int)(overflow/mouseLocation);
		}
		else
		{
			scrollBar->Visible = false;
		}

		scrollBar->Size.X = scrollSize - 1;

		offsetDelta = toolButtons[0]->Position.X - newInitialX;

		for (auto *button : toolButtons)
		{
			button->Position.X -= offsetDelta;
		}

		// Ensure that mouseLeave events are make their way to the buttons should they move from underneath the mouse pointer
		if (toolButtons[0]->Position.Y < y && toolButtons[0]->Position.Y + toolButtons[0]->Size.Y > y)
		{
			for (auto *button : toolButtons)
			{
				auto inside = button->Position.X < x && button->Position.X + button->Size.X > x;
				if (inside && !button->MouseInside)
				{
					button->MouseInside = true;
					button->OnMouseEnter(x, y);
				}
				if (!inside && button->MouseInside)
				{
					button->MouseInside = false;
					button->OnMouseLeave(x, y);
				}
			}
		}
	}
}

void GameViewBasic::SetSaveButtonTooltips()
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

void GameViewBasic::SetSaveButtonShowSplit(bool split)
{
	saveSimulationButton->SetShowSplit(split);
}

ui::Button * GameViewBasic::GetSaveButton()
{
	return saveSimulationButton;
}

void GameViewBasic::NotifyMenuListChanged(GameModel * sender)
{
	int currentY = WINDOWH-48;//-(sender->GetMenuList().size()*16);
	for (size_t i = 0; i < menuButtons.size(); i++)
	{
		RemoveComponent(menuButtons[i]);
		delete menuButtons[i];
	}
	menuButtons.clear();
	for (size_t i = 0; i < toolButtons.size(); i++)
	{
		RemoveComponent(toolButtons[i]);
		delete toolButtons[i];
	}
	toolButtons.clear();
	std::vector<Menu*> menuList = sender->GetMenuList();
	for (int i = (int)menuList.size()-1; i >= 0; i--)
	{
		if (menuList[i]->GetVisible())
		{
			String tempString = "";
			tempString += menuList[i]->GetIcon();
			String description = menuList[i]->GetDescription();
			if (i == SC_FAVORITES && !Favorite::Ref().AnyFavorites())
				description += " (Use ctrl+shift+click to toggle the favorite status of an element)";
			auto *tempButton = new MenuButton(ui::Point(WINDOWW-16, currentY), ui::Point(15, 15), tempString, description);
			tempButton->Appearance.Margin = ui::Border(0, 2, 3, 2);
			tempButton->menuID = i;
			tempButton->needsClick = i == SC_DECO;
			tempButton->SetTogglable(true);
			auto mouseEnterCallback = [this, tempButton] {
				// don't immediately change the active menu, the actual set is done inside GameView::OnMouseMove
				// if we change it here it causes components to be removed, which causes the window to stop sending events
				// and then the previous menusection button never gets sent the OnMouseLeave event and is never unhighlighted
				if(!(tempButton->needsClick || c->GetMouseClickRequired()) && !GetMouseDown())
					SetActiveMenuDelayed(tempButton->menuID);
			};
			auto actionCallback = [this, tempButton, mouseEnterCallback] {
				if (tempButton->needsClick || c->GetMouseClickRequired())
					c->SetActiveMenu(tempButton->menuID);
				else
					mouseEnterCallback();
			};
			tempButton->SetActionCallback({ actionCallback, nullptr, mouseEnterCallback });
			currentY-=16;
			AddComponent(tempButton);
			menuButtons.push_back(tempButton);
		}
	}
}

void GameViewBasic::NotifyToolListChanged(GameModel * sender)
{
	for (size_t i = 0; i < menuButtons.size(); i++)
	{
		if (menuButtons[i]->menuID==sender->GetActiveMenu())
		{
			menuButtons[i]->SetToggleState(true);
		}
		else
		{
			menuButtons[i]->SetToggleState(false);
		}
	}
	for (size_t i = 0; i < toolButtons.size(); i++)
	{
		RemoveComponent(toolButtons[i]);
		delete toolButtons[i];
	}
	toolButtons.clear();
	std::vector<Tool*> toolList = sender->GetToolList();
	int currentX = 0;
	for (size_t i = 0; i < toolList.size(); i++)
	{
		auto *tool = toolList[i];
		auto tempTexture = tool->GetTexture(Vec2(26, 14));
		ToolButton * tempButton;

		//get decotool texture manually, since it changes depending on it's own color
		if (sender->GetActiveMenu() == SC_DECO)
			tempTexture = ((DecorationTool*)tool)->GetIcon(tool->ToolID, Vec2(26, 14));

		if (tempTexture)
			tempButton = new ToolButton(ui::Point(currentX, YRES+1), ui::Point(30, 18), "", tool->Identifier, tool->Description);
		else
			tempButton = new ToolButton(ui::Point(currentX, YRES+1), ui::Point(30, 18), tool->Name, tool->Identifier, tool->Description);

		tempButton->ClipRect = RectSized(Vec2(1, RES.Y + 1), Vec2(RES.X - 1, 18));

		//currentY -= 17;
		currentX -= 31;
		tempButton->tool = tool;
		tempButton->SetActionCallback({ [this, tempButton] {
			auto *tool = tempButton->tool;
			if (ShiftBehaviour() && CtrlBehaviour() && !AltBehaviour())
			{
				if (tempButton->GetSelectionState() == 0)
				{
					if (Favorite::Ref().IsFavorite(tool->Identifier))
					{
						Favorite::Ref().RemoveFavorite(tool->Identifier);
					}
					else
					{
						Favorite::Ref().AddFavorite(tool->Identifier);
					}
					c->RebuildFavoritesMenu();
				}
				else if (tempButton->GetSelectionState() == 1)
				{
					auto identifier = tool->Identifier;
					if (Favorite::Ref().IsFavorite(identifier))
					{
						Favorite::Ref().RemoveFavorite(identifier);
						c->RebuildFavoritesMenu();
					}
					else if (identifier.BeginsWith("DEFAULT_PT_LIFECUST_"))
					{
						new ConfirmPrompt("Remove custom GOL type", "Are you sure you want to remove " + identifier.Substr(20).FromUtf8() + "?", { [this, identifier]() {
							c->RemoveCustomGOLType(identifier);
						} });
					}
				}
			}
			else
			{
				if (CtrlBehaviour() && AltBehaviour() && !ShiftBehaviour())
				{
					if (tool->Identifier.Contains("_PT_"))
					{
						tempButton->SetSelectionState(3);
					}
				}

				if (tempButton->GetSelectionState() >= 0 && tempButton->GetSelectionState() <= 3)
					c->SetActiveTool(tempButton->GetSelectionState(), tool);
			}
		} });

		tempButton->Appearance.SetTexture(std::move(tempTexture));

		tempButton->Appearance.BackgroundInactive = toolList[i]->Colour.WithAlpha(0xFF);

		if(sender->GetActiveTool(0) == toolList[i])
		{
			tempButton->SetSelectionState(0);	//Primary
		}
		else if(sender->GetActiveTool(1) == toolList[i])
		{
			tempButton->SetSelectionState(1);	//Secondary
		}
		else if(sender->GetActiveTool(2) == toolList[i])
		{
			tempButton->SetSelectionState(2);	//Tertiary
		}
		else if(sender->GetActiveTool(3) == toolList[i])
		{
			tempButton->SetSelectionState(3);	//Replace mode
		}

		tempButton->Appearance.HorizontalAlign = ui::Appearance::AlignCentre;
		tempButton->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
		AddComponent(tempButton);
		toolButtons.push_back(tempButton);
	}
	if (sender->GetActiveMenu() != SC_DECO)
		lastMenu = sender->GetActiveMenu();

	updateToolButtonScroll();
}

void GameViewBasic::ToolTip(ui::Point senderPosition, String toolTip)
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
