#include "FoldScrollPanel.h"

#include <cmath>

#include "FoldButton.h"

#include "graphics/Graphics.h"

using namespace ui;

int FoldScrollPanel::GetYOffset(int group)
{
	int offset = (group + 1) * 16;
	for (int i = 0; i < group; i++) // Ignore current group
	{
		if (groups[i].expanded)
		{
			offset += groups[i].groupY;
		}
	}
	return offset;
}

FoldScrollPanel::FoldScrollPanel(Point position, Point size) :
	ScrollPanel(position, size)
{
	foldButtonHandler = [this](int id, bool expanded) {
		groups[id].expanded = expanded;
		for (Component* i : groups[id].components)
		{
			i->Position.X = std::abs(i->Position.X) * (expanded ? 1 : -1);
			i->Visible = expanded;
		}
		int yDiff = groups[id].groupY * (expanded ? 1 : -1);
		InnerSize.Y += yDiff;
		for (unsigned int i = id + 1; i < groups.size(); i++)
		{
			groups[i].button->Position.Y += yDiff;
			for (Component* j : groups[i].components)
			{
				j->Position.Y += yDiff;
			}
		}
	};
}

void FoldScrollPanel::NextGroup(String label, int groupY, bool expanded)
{
	FoldButton* button = new FoldButton(Point(0, GetYOffset(groups.size()) - 16), Point(Size.X - 6, 16), label, groups.size(), expanded); // -6 for scroll bar
	button->SetExpandCallback(foldButtonHandler);
	groups.push_back(FoldGroup{ button, std::vector<Component*>(), groupY, expanded });
	ScrollPanel::AddChild(button);
}

void FoldScrollPanel::AddChild(Component* c)
{
	if (!groups.empty())
	{
		auto& group = groups.back();
		c->Position.X *= group.expanded ? 1 : -1;
		c->Position.Y += GetYOffset(groups.size() - 1);
		c->Visible = group.expanded;
		group.components.push_back(c);
	}
	ScrollPanel::AddChild(c);
}

void FoldScrollPanel::XDraw(const Point& screenPos)
{
	Graphics * g = GetGraphics();
	g->draw_line(screenPos.X + 1, screenPos.Y - 1, screenPos.X + Size.X - 2, screenPos.Y - 1, 200, 200, 200, 255);
	g->draw_line(screenPos.X + 1, screenPos.Y + Size.Y, screenPos.X + Size.X - 2, screenPos.Y + Size.Y, 200, 200, 200, 255);
}
