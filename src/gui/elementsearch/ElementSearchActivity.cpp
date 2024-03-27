#include "ElementSearchActivity.h"

#include <set>
#include <map>
#include <algorithm>
#include <SDL.h>

#include "gui/interface/Engine.h"
#include "gui/interface/Textbox.h"
#include "gui/interface/ScrollPanel.h"
#include "gui/interface/Label.h"
#include "gui/game/tool/Tool.h"
#include "gui/game/Menu.h"
#include "gui/Style.h"
#include "gui/game/Favorite.h"
#include "gui/game/GameController.h"
#include "gui/game/ToolButton.h"

#include "graphics/Graphics.h"

#include "simulation/SimulationData.h"

ElementSearchActivity::ElementSearchActivity(GameController * gameController, std::vector<Tool*> tools) :
	WindowActivity(ui::Point(-1, -1), ui::IfTouchUI<ui::Point>({ 541, 304 }, { 236, 302})),
	firstResult(nullptr),
	gameController(gameController),
	tools(tools),
	toolTip(""),
	shiftPressed(false),
	ctrlPressed(false),
	altPressed(false),
	isToolTipFadingIn(false),
	toolButtonSize(ui::IfTouchUI<ui::Point>({ 36, 24 }, { 30, 18 })),
	exit(false)
{
	ui::Label * title;
	ui::Button * closeButton;
	if (ui::Engine::Ref().TouchUI)
	{
		title = new ui::Label(ui::Point(8, 8), ui::Point(80, 26), "Select Element");
		searchField = new ui::Textbox(ui::Point(88, 8), ui::Point(Size.X - 87 - 64, 26), "", "[search]");
		closeButton = new ui::Button(ui::Point(Size.X - 56, 8), ui::Point(48, 26), "Close");
	}
	else
	{
		title = new ui::Label(ui::Point(4, 5), ui::Point(Size.X - 8, 15), "Element Search");
		searchField = new ui::Textbox(ui::Point(8, 23), ui::Point(Size.X - 16, 17), "");
		closeButton = new ui::Button(ui::Point(0, Size.Y - 15), ui::Point((Size.X / 2) + 1, 15), "Close");

		ui::Button * okButton = new ui::Button(ui::Point(Size.X / 2, Size.Y - 15), ui::Point(Size.X / 2, 15), "OK");
		okButton->SetActionCallback({ [this] {
			if (GetFirstResult())
				SetActiveTool(0, GetFirstResult());
		} });
		AddComponent(okButton);
	}

	title->SetTextColour(style::Colour::InformationTitle);
	title->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	AddComponent(title);

	searchField->SetActionCallback({ [this] { SearchTools(searchField->GetText()); } });
	searchField->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	AddComponent(searchField);
	if (!ui::Engine::Ref().TouchUI)
	{
		FocusComponent(searchField);
	}

	closeButton->SetActionCallback({ [this] { exit = true; } });
	AddComponent(closeButton);

	scrollPanel = new ui::ScrollPanel(
		ui::Point(9, ui::IfTouchUI(42, 49)),
		ui::Point(Size.X - 18, Size.Y - ui::IfTouchUI(51, 69))
	);
	AddComponent(scrollPanel);

	SearchTools("");
}

void ElementSearchActivity::PlaceTool(Tool * tool, ui::Point position)
{
	position += ui::IfTouchUI<ui::Point>({ 3, 3 }, { 1, 1 });

	std::unique_ptr<VideoBuffer> tempTexture = tool->GetTexture(toolButtonSize - Vec2{4, 4});
	ToolButton * tempButton;

	if(tempTexture)
		tempButton = new ToolButton(position, toolButtonSize, "", tool->Identifier, tool->Description);
	else
		tempButton = new ToolButton(position, toolButtonSize, tool->Name, tool->Identifier, tool->Description);

	tempButton->Appearance.SetTexture(std::move(tempTexture));
	tempButton->Appearance.BackgroundInactive = tool->Colour.WithAlpha(0xFF);
	tempButton->SetActionCallback({ [this, tempButton, tool] {
		if (tempButton->GetSelectionState() >= 0 && tempButton->GetSelectionState() <= 2)
			SetActiveTool(tempButton->GetSelectionState(), tool);
	} });

	if(gameController->GetActiveTool(0) == tool)
	{
		tempButton->SetSelectionState(0);	//Primary
	}
	else if(gameController->GetActiveTool(1) == tool)
	{
		tempButton->SetSelectionState(1);	//Secondary
	}
	else if(gameController->GetActiveTool(2) == tool)
	{
		tempButton->SetSelectionState(2);	//Tertiary
	}

	scrollPanel->AddChild(tempButton);
	toolButtons.push_back(tempButton);
}

void ElementSearchActivity::PlaceTools() // Touch UI element selector layout
{
	ui::Point current = ui::Point(0, 0);
	auto menuList = gameController->GetMenuList();
	for (size_t i = 0; i < menuList.size(); i++)
	{
		auto menu = menuList[i];
		if (menu->GetVisible() && i != SC_DECO)
		{
			current.Y += 2 + (current.X ? toolButtonSize.Y : 0);
			current.X = 0;

			ui::Label * tempLabel = new ui::Label(ui::Point(scrollPanel->Size.X - 20, current.Y - 3), ui::Point(12, 15), menu->GetIcon());
			scrollPanel->AddChild(tempLabel);
			menuLabels.push_back(tempLabel);

			tempLabel = new ui::Label(ui::Point(0, current.Y - 1), ui::Point(scrollPanel->Size.X - 20, 15), menu->GetDescription());
			tempLabel->Appearance.HorizontalAlign = ui::Appearance::AlignRight;
			scrollPanel->AddChild(tempLabel);
			menuLabels.push_back(tempLabel);
			current.Y += 11;

			for (auto tool : menu->GetToolList())
			{
				PlaceTool(tool, { scrollPanel->Size.X - toolButtonSize.X - 6 - current.X, current.Y });
				current.X += toolButtonSize.X + 1;

				if(current.X + toolButtonSize.X > scrollPanel->Size.X)
				{
					current.X = 0;
					current.Y += toolButtonSize.Y + 1;
				}
			}

			current.Y += 3;
		}
	}
	scrollPanel->InnerSize = ui::Point(scrollPanel->Size.X, current.Y + toolButtonSize.Y + 6);
}

void ElementSearchActivity::SearchTools(String query)
{
	firstResult = nullptr;
	for (auto &toolButton : toolButtons) {
		scrollPanel->RemoveChild(toolButton);
		delete toolButton;
	}
	toolButtons.clear();
	if (ui::Engine::Ref().TouchUI)
	{
		for (auto menuLabel : menuLabels) {
			scrollPanel->RemoveChild(menuLabel);
			delete menuLabel;
		}
		menuLabels.clear();
		if (!query.length())
		{
			PlaceTools();
			return;
		}
	}

	ui::Point current = ui::Point(0, 0);

	String queryLower = query.ToLower();

	struct Match
	{
		int toolIndex; // relevance by position of tool in tools vector
		int haystackOrigin; // relevance by origin of haystack
		int needlePosition; // relevance by position of needle in haystack

		bool operator <(Match const &other) const
		{
			return std::tie(haystackOrigin, needlePosition, toolIndex) < std::tie(other.haystackOrigin, other.needlePosition, other.toolIndex);
		}
	};

	std::map<int, Match> indexToMatch;
	auto push = [ &indexToMatch ](Match match) {
		auto it = indexToMatch.find(match.toolIndex);
		if (it == indexToMatch.end())
		{
			indexToMatch.insert(std::make_pair(match.toolIndex, match));
		}
		else if (match < it->second)
		{
			it->second = match;
		}
	};

	auto pushIfMatches = [ &queryLower, &push ](String infoLower, int toolIndex, int haystackRelevance) {
		if (infoLower == queryLower)
		{
			push(Match{ toolIndex, haystackRelevance, 0 });
		}
		if (infoLower.BeginsWith(queryLower))
		{
			push(Match{ toolIndex, haystackRelevance, 1 });
		}
		if (infoLower.Contains(queryLower))
		{
			push(Match{ toolIndex, haystackRelevance, 2 });
		}
	};

	std::map<Tool *, String> menudescriptionLower;
	for (auto *menu : gameController->GetMenuList())
	{
		for (auto *tool : menu->GetToolList())
		{
			menudescriptionLower.insert(std::make_pair(tool, menu->GetDescription().ToLower()));
		}
	}

	for (int toolIndex = 0; toolIndex < (int)tools.size(); ++toolIndex)
	{
		pushIfMatches(tools[toolIndex]->Name.ToLower(), toolIndex, 0);
		pushIfMatches(tools[toolIndex]->Description.ToLower(), toolIndex, 1);
		auto it = menudescriptionLower.find(tools[toolIndex]);
		if (it != menudescriptionLower.end())
		{
			pushIfMatches(it->second, toolIndex, 2);
		}
	}

	std::vector<Match> matches;
	std::transform(indexToMatch.begin(), indexToMatch.end(), std::back_inserter(matches), [](decltype(indexToMatch)::value_type const &pair) {
		return pair.second;
	});
	std::sort(matches.begin(), matches.end());
	for (auto &match : matches)
	{
		Tool *tool = tools[match.toolIndex];

		if(!firstResult)
			firstResult = tool;

		PlaceTool(tool, current);
		current.X += toolButtonSize.X + 1;

		if(current.X + toolButtonSize.X > scrollPanel->Size.X)
		{
			current.X = 0;
			current.Y += toolButtonSize.Y + 1;
		}
	}

	if (current.X == 0)
	{
		current.Y -= 19;
	}
	scrollPanel->InnerSize = ui::Point(scrollPanel->Size.X, current.Y + 20);
}

void ElementSearchActivity::SetActiveTool(int selectionState, Tool * tool)
{
	if (ctrlPressed && shiftPressed && !altPressed)
	{
		Favorite::Ref().AddFavorite(tool->Identifier);
		gameController->RebuildFavoritesMenu();
	}
	else if (ctrlPressed && altPressed && !shiftPressed &&
	         tool->Identifier.BeginsWith("DEFAULT_PT_"))
	{
		gameController->SetActiveTool(3, tool);
	}
	else
		gameController->SetActiveTool(selectionState, tool);
	exit = true;
}

void ElementSearchActivity::OnDraw()
{
	Graphics * g = GetGraphics();
	g->DrawFilledRect(RectSized(Position - Vec2{ 1, 1 }, Size + Vec2{ 2, 2 }), 0x000000_rgb);
	g->DrawRect(RectSized(Position, Size), 0xFFFFFF_rgb);

	g->BlendRect(
		RectSized(Position + scrollPanel->Position - Vec2{1, 1}, scrollPanel->Size + Vec2{2, 2}),
		0xFFFFFF_rgb .WithAlpha(180)
	);
	if (toolTipPresence && toolTip.length())
	{
		g->BlendText({ 10, WINDOWH-MENUSIZE-12 }, toolTip, 0xFFFFFF_rgb .WithAlpha(toolTipPresence>51?255:toolTipPresence*5));
	}
}

void ElementSearchActivity::OnTick()
{
	if (exit)
		Exit();

	if (isToolTipFadingIn)
	{
		isToolTipFadingIn = false;
		toolTipPresence.SetTarget(120);
	}
	else
	{
		toolTipPresence.SetTarget(0);
	}
}

void ElementSearchActivity::OnKeyPress(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt)
{
	if (repeat)
		return;
	switch (key)
	{
	case SDLK_KP_ENTER:
	case SDLK_RETURN:
		if(firstResult)
			gameController->SetActiveTool(0, firstResult);
	case SDLK_ESCAPE:
	case SDLK_AC_BACK:
		exit = true;
		break;
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		shiftPressed = true;
		break;
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		ctrlPressed = true;
		break;
	case SDLK_LALT:
	case SDLK_RALT:
		altPressed = true;
		break;
	}
}

void ElementSearchActivity::OnKeyRelease(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt)
{
	if (repeat)
		return;
	switch (key)
	{
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		shiftPressed = false;
		break;
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		ctrlPressed = false;
		break;
	case SDLK_LALT:
	case SDLK_RALT:
		altPressed = false;
		break;
	}
}

void ElementSearchActivity::ToolTip(ui::Point senderPosition, String toolTip)
{
	this->toolTip = toolTip;
	this->isToolTipFadingIn = true;
}

ElementSearchActivity::~ElementSearchActivity() {
}

