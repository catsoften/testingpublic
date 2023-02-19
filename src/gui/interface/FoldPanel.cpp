#include "FoldPanel.h"

#include "graphics/Graphics.h"

using namespace ui;

FoldPanel::FoldPanel(Point position, Point size, String text_):
	Panel(position, size),
	expandedY(size.Y),
	expanded(true),
	expandCallback(nullptr),
	isMouseInside(false),
	isMouseDown(false),
	text(text_)
{
}

bool FoldPanel::GetExpanded()
{
	return expanded;
}

void FoldPanel::SetExpanded(bool expanded_)
{
	expanded = expanded_;
	Size.Y = expanded ? expandedY : 16;

	if (expandCallback)
	{
		expandCallback(expanded);
	}
}

void FoldPanel::SetExpandCallback(ExpandCallback expandCallback_)
{
	expandCallback = expandCallback_;
}

String FoldPanel::GetText()
{
	return text;
}

void FoldPanel::SetText(String text_)
{
	text = text_;
}

void FoldPanel::AddChild(Component* c)
{
	c->Position.Y += 16;
	Panel::AddChild(c);
}

void FoldPanel::Draw(const Point& screenPos)
{
	Graphics * g = GetGraphics();

	//g->fillrect(screenPos.X, screenPos.Y, Size.X, Size.Y, 255, 0, 0, 31);

	ui::Colour textColour = Appearance.GetTextColour(Enabled, isMouseDown, isMouseInside);
	ui::Colour borderColour = Appearance.GetBorderColour(Enabled, isMouseDown, isMouseInside);
	ui::Colour backgroundColour = Appearance.GetBackgroundColour(Enabled, isMouseDown, isMouseInside);
	g->drawrect(screenPos.X, screenPos.Y, Size.X, 16, borderColour.Red, borderColour.Green, borderColour.Blue, borderColour.Alpha);
	g->fillrect(screenPos.X + 1, screenPos.Y + 1, Size.X - 2, 14, backgroundColour.Red, backgroundColour.Green, backgroundColour.Blue, backgroundColour.Alpha);
	//g->fillrect(screenPos.X + 8, screenPos.Y, Size.X - 8, 16, 0, 255, 0, 31);
	g->drawchar(screenPos.X + 10, screenPos.Y + 4, expanded ? 0xE06D : 0xE06C , textColour.Red, textColour.Green, textColour.Blue, textColour.Alpha);
	g->drawtext(screenPos.X + 26, screenPos.Y + 4, text, textColour.Red, textColour.Green, textColour.Blue, textColour.Alpha);

	if (expanded)
	{
		//g->fillrect(screenPos.X, screenPos.Y + 16, Size.X, Size.Y - 16, 0, 0, 255, 31);
		Panel::Draw(screenPos);
	}
}

void FoldPanel::XOnMouseMoved(int localx, int localy, int dx, int dy)
{
	isMouseInside = localx > 0 && localx < Size.X && localy > 0 && localy < 16 ? true : false;
}

void FoldPanel::XOnMouseDown(int x, int y, unsigned button)
{
	if (button == 1 && isMouseInside)
	{
		isMouseDown = true;
	}
}

void FoldPanel::XOnMouseUp(int x, int y, unsigned button)
{
	if (button == 1)
	{
		if (Enabled && isMouseInside && isMouseDown)
		{
			SetExpanded(!expanded);
		}
		isMouseDown = false;
	}
}
