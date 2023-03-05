#include "FoldButton.h"

#include "graphics/Graphics.h"

using namespace ui;

FoldButton::FoldButton(Point position, Point size, String text_, int id_, bool expanded_) :
	Component(position, size),
	text(text_),
	id(id_),
	expanded(expanded_)
{
}

String FoldButton::GetText()
{
	return text;
}

void FoldButton::SetText(String text_)
{
	text = text_;
}

bool FoldButton::GetExpanded()
{
	return expanded;
}

void FoldButton::SetExpanded(bool expanded_)
{
	expanded = expanded_;
	if (expandCallback)
	{
		expandCallback(id, expanded);
	}
}

void FoldButton::SetExpandCallback(ExpandCallback expandCallback_)
{
	expandCallback = expandCallback_;
}

void FoldButton::Draw(const Point& screenPos)
{
	Graphics * g = GetGraphics();

	ui::Colour textColour = Appearance.GetTextColour(Enabled, isMouseDown, isMouseInside);
	ui::Colour borderColour = Appearance.GetBorderColour(Enabled, isMouseDown, isMouseInside);
	ui::Colour backgroundColour = Appearance.GetBackgroundColour(Enabled, isMouseDown, isMouseInside);

	g->fillrect(screenPos.X, screenPos.Y, Size.X + 6, Size.Y, borderColour.Red, borderColour.Green, borderColour.Blue, borderColour.Alpha); // +6 for scroll bar
	g->fillrect(screenPos.X + 6, screenPos.Y + 2, Size.X - 12 + 6, Size.Y - 4, backgroundColour.Red, backgroundColour.Green, backgroundColour.Blue, backgroundColour.Alpha);

	g->drawtext(screenPos.X + 9, screenPos.Y + 4, String::Build(expanded ? "\uE06D " : "\uE06C ", text), textColour.Red, textColour.Green, textColour.Blue, textColour.Alpha);
}

void FoldButton::OnMouseMoved(int localx, int localy, int dx, int dy)
{
	isMouseInside = localx >= 0 && localx <= Size.X && localy >= 0 && localy <= Size.Y;
}

void FoldButton::OnMouseDown(int x, int y, unsigned button)
{
	if (Enabled && button == 1 && isMouseInside)
	{
		isMouseDown = true;
	}
}

void FoldButton::OnMouseUp(int x, int y, unsigned button)
{
	if (Enabled && button == 1)
	{
		if (isMouseInside && isMouseDown)
		{
			SetExpanded(!expanded);
		}
		isMouseDown = false;
	}
}
