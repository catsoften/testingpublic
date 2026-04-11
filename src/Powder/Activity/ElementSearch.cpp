#include "ElementSearch.hpp"
#include "Common/Log.hpp"
#include "Game.hpp"
#include "Gui/Host.hpp"
#include "Gui/Icons.hpp"
#include "Gui/SdlAssert.hpp"
#include "Gui/StaticTexture.hpp"
#include "simulation/SimulationData.h"
#include "gui/game/tool/Tool.h"

namespace Powder::Activity
{
	namespace
	{
		constexpr Gui::View::Size toolButtonPadding =  2;
		constexpr Gui::View::Size toolButtonSpacing =  1;

		const std::string actionContextNamePrefix = "DEFAULT_ACTIONCONTEXT_";
	}

	ElementSearch::ElementSearch(Game &newGame) :
		View(newGame.GetHost()),
		game(newGame)
	{
		toolAtlasTexture = game.MakeToolAtlasTexture(*this);
		InitActions();
		Update();
	}

	void ElementSearch::InitActions()
	{
		auto makeActionContext = [](std::string name, std::shared_ptr<ActionContext> parentContext) {
			auto context = std::make_shared<ActionContext>();
			context->name = actionContextNamePrefix + name;
			context->parentContext = parentContext;
			return context;
		};
		rootContext = makeActionContext("ROOT", nullptr);
		// TODO-REDO_UI: somehow sync with Game
		// Game::InitDrawActions(rootContext.get(), nullptr, [this](int32_t toolSlot) {
		// 	if (lastHoveredTool)
		// 	{
		// 		game.SelectTool(toolSlot, lastHoveredTool);
		// 		Exit();
		// 		return true;
		// 	}
		// 	return false;
		// });
		SetCurrentActionContext(rootContext);
	}

	void ElementSearch::Update()
	{
		auto toLower = [](const std::string &str) {
			return std::string(ByteString(str).ToLower()); // TODO-REDO_UI
		};
		auto queryLower = toLower(query);

		enum class MatchingProperty
		{
			name,
			description,
			menuDescription,
		};
		enum class MatchKind
		{
			equals,
			beginsWith,
			contains,
		};
		struct Match
		{
			MatchingProperty matchingProperty;
			MatchKind matchKind;
			int32_t toolIndex;

			auto operator <=>(const Match &) const = default;
		};

		auto &tools = game.GetTools();
		std::vector<std::optional<Match>> matches(tools.size());
		auto found = [&](Match newMatch) {
			auto &match = matches[newMatch.toolIndex];
			if (!match)
			{
				match = newMatch;
			}
			*match = std::min(*match, newMatch);
		};

		auto matchProperty = [&](std::string infoLower, MatchingProperty matchingProperty, int32_t toolIndex) {
			if (infoLower == queryLower)
			{
				found({ matchingProperty, MatchKind::equals, toolIndex });
			}
			auto pos = infoLower.find(queryLower);
			if (pos == 0)
			{
				found({ matchingProperty, MatchKind::beginsWith, toolIndex });
			}
			if (pos != infoLower.npos)
			{
				found({ matchingProperty, MatchKind::contains, toolIndex });
			}
		};

		auto &sd = SimulationData::CRef();
		for (int32_t toolIndex = 0; toolIndex < int32_t(tools.size()); ++toolIndex)
		{
			auto &info = tools[toolIndex];
			if (!info)
			{
				continue;
			}
			matchProperty(toLower(info->tool->Name.ToUtf8()), MatchingProperty::name, toolIndex);
			matchProperty(toLower(info->tool->Description.ToUtf8()), MatchingProperty::description, toolIndex);
			auto menuSection = info->tool->MenuSection;
			if (menuSection >= 0 && menuSection < int32_t(sd.msections.size()))
			{
				matchProperty(toLower(sd.msections[menuSection].name.ToUtf8()), MatchingProperty::menuDescription, toolIndex);
			}
		}

		std::erase_if(matches, [](auto &item) {
			return !item;
		});
		std::sort(matches.begin(), matches.end());
		matchingTools.clear();
		std::transform(matches.begin(), matches.end(), std::back_inserter(matchingTools), [&](auto &match) {
			return &*tools[match->toolIndex];
		});
	}

	ElementSearch::DispositionFlags ElementSearch::GetDisposition() const
	{
		if (!matchingTools.empty())
		{
			return DispositionFlags::none;
		}
		return DispositionFlags::okDisabled;
	}

	void ElementSearch::Ok()
	{
		game.SelectTool(0, matchingTools[0]->tool.get());
		Exit();
	}

	void ElementSearch::Gui()
	{
		auto &g = GetHost();
		int32_t toolButtonsPerRow = g.IfTouchUI(14, 7);
		int32_t maxRows = g.IfTouchUI(10, 12); // and an extra half row is shown to indicate that scrolling is possible

		auto elementSearch = ScopedDialog("elementSearch", "Element search"); // TODO-REDO_UI-TRANSLATE
		BeginTextbox("query", query, "[name, description, or category]", TextboxFlags::none); // TODO-REDO_UI-TRANSLATE
		SetSize(Common{});
		if (focusQuery)
		{
			GiveInputFocus();
			focusQuery = false;
		}
		if (EndTextbox())
		{
			Update();
		}
		auto tools = ScopedScrollpanel("tools");
		SetAlignment(Gui::Alignment::top);
		SetMaxSizeSecondary(MaxSizeFitParent{});
		SetParentFillRatio(0);
		SetSize((Game::toolTextureDataSize.Y + 2 * toolButtonPadding + toolButtonSpacing) * (2 * maxRows + 1) / 2 + toolButtonSpacing + 2);
		SetSizeSecondary((Game::toolTextureDataSize.X + 2 * toolButtonPadding + toolButtonSpacing) * toolButtonsPerRow + toolButtonSpacing + 2);
		SetPadding(toolButtonPadding);
		SetSpacing(toolButtonSpacing);
		auto r = GetRect();
		lastHoveredTool = nullptr;
		int32_t buttonIndex = 0;
		int32_t rowIndex = 0;
		bool inHPanel = false;

		auto addTool = [&](const GameToolInfo &info, bool rightAlign)
		{
			if (!inHPanel)
			{
				BeginHPanel(rowIndex++);
				if (rightAlign)
				{
					SetAlignment(Gui::Alignment::right);
					SetOrder(Order::rightToLeft);
				}
				else
				{
					SetAlignment(Gui::Alignment::left);
				}
				SetSpacing(1);
				SetParentFillRatio(0);
				inHPanel = true;
			}

			{
				auto cell = ScopedComponent(buttonIndex);
				SetSize(Game::toolTextureDataSize.X + 2 * toolButtonPadding);
				if (game.GuiToolButton(*this, info, *toolAtlasTexture, true))
				{
					// QueueToolTip(info->tool->Description.ToUtf8(), ...) // TODO-REDO_UI
					lastHoveredTool = info.tool.get();
				}
				buttonIndex++;
			}

			if (buttonIndex % toolButtonsPerRow == 0 && inHPanel)
			{
				EndPanel();
				inHPanel = false;
			}
		};

		if (g.GetTouchUI() && query.empty())
		{
			auto &sd = SimulationData::CRef();
			auto &tools = game.GetTools();

			static const std::array<StringView, 16> iconOverrides = {{ // TODO-REDO_UI: move this where menu sections are defined
				StringView(Gui::iconWalls),
				StringView(Gui::iconElectronic),
				StringView(Gui::iconPowered),
				StringView(Gui::iconSensor),
				StringView(Gui::iconForce),
				StringView(Gui::iconExplosive),
				StringView(Gui::iconGas),
				StringView(Gui::iconLiquid),
				StringView(Gui::iconPowder),
				StringView(Gui::iconSolid),
				StringView(Gui::iconRadioactive),
				StringView(Gui::iconStar),
				StringView(Gui::iconGol),
				StringView(Gui::iconTool),
				StringView(Gui::iconFavorite),
				StringView(Gui::iconDeco),
			}};
			Assert(sd.msections.size() == iconOverrides.size());

			auto addMenu = [&](int32_t i) {
				buttonIndex = 0;
				for (auto &info : tools)
				{
					if (info && info->tool->MenuVisible && info->tool->MenuSection == i)
					{
						addTool(*info, true);
					}
				}
				if (inHPanel)
				{
					EndPanel();
					inHPanel = false;
				}
			};

			for (int32_t i = 0; i < int32_t(sd.msections.size()); i++)
			{
				TextSeparator(rowIndex++, " " + sd.msections[i].name.ToAscii() + " " + std::string(iconOverrides[i]) + " ");
				addMenu(i);
			}
		}
		else
		{
			while (buttonIndex < int32_t(matchingTools.size()))
			{
				addTool(*matchingTools[buttonIndex], false);
			}
		}
		if (inHPanel)
		{
			EndPanel();
		}
		g.DrawRect(r, 0xFFFFFFFF_argb);
	}

	bool ElementSearch::HandleEvent(const SDL_Event &event)
	{
		auto handledByView = View::HandleEvent(event);
		if (MayBeHandledExclusively(event) && handledByView && !lastHoveredTool)
		{
			return true;
		}
		auto handledByInputMapper = InputMapper::HandleEvent(event);
		if (MayBeHandledExclusively(event) && handledByInputMapper)
		{
			return true;
		}
		return false;
	}

	void ElementSearch::SetOnTop(bool newOnTop)
	{
		if (!newOnTop)
		{
			EndAllInputs();
		}
	}
}
