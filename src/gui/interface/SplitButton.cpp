#include "SplitButton.h"

#include "graphics/Graphics.h"

#include "gui/interface/Window.h"

namespace ui
{

SplitButton::SplitButton(Point position, Point size, String buttonText, String toolTip, String toolTip2, int split) :
	Button(position, size, buttonText, toolTip),
	showSplit(true),
	splitPosition(split),
	toolTip2(toolTip2)
{

}

void SplitButton::SetRightToolTip(String tooltip)
{
	toolTip2 = tooltip;
}

bool SplitButton::GetShowSplit()
{
	return showSplit;
}

void SplitButton::SetShowSplit(bool split)
{
	showSplit = split;
}

void SplitButton::SetToolTip(int x, int y)
{
	if(x >= splitPosition || !showSplit)
	{
		if(toolTip2.length()>0 && GetParentWindow())
		{
			GetParentWindow()->ToolTip(Position, toolTip2);
		}
	}
	else if(x < splitPosition)
	{
		if(toolTip.length()>0 && GetParentWindow())
		{
			GetParentWindow()->ToolTip(Position, toolTip);
		}
	}
}

void SplitButton::OnMouseClick(int x, int y, unsigned int button)
{
	if(isButtonDown)
	{
		if(leftDown)
			DoLeftAction();
		else if(rightDown)
			DoRightAction();
	}
	Button::OnMouseClick(x, y, button);
}

void SplitButton::OnMouseHover(int x, int y)
{
	SetToolTip(x, y);
}

void SplitButton::OnMouseEnter(int x, int y)
{
	isMouseInside = true;
	if(!Enabled)
		return;
	SetToolTip(x, y);
}

void SplitButton::TextPosition(String ButtonText)
{
	Button::TextPosition(ButtonText);
	textPosition.X += 3;
}

void SplitButton::SetToolTips(String newToolTip1, String newToolTip2)
{
	toolTip = newToolTip1;
	toolTip2 = newToolTip2;
}

void SplitButton::OnMouseDown(int x, int y, unsigned int button)
{
	Button::OnMouseDown(x, y, button);
	if (MouseDownInside)
	{
		rightDown = false;
		leftDown = false;
		if(x - Position.X >= splitPosition)
			rightDown = true;
		else if(x - Position.X < splitPosition)
			leftDown = true;
	}
}

void SplitButton::DoRightAction()
{
	if(!Enabled)
		return;
	if (actionCallback.right)
		actionCallback.right();
}

void SplitButton::DoLeftAction()
{
	if(!Enabled)
		return;
	if (actionCallback.left)
		actionCallback.left();
}

void SplitButton::Draw(const Point& screenPos)
{
	Button::Draw(screenPos);
	Graphics * g = GetGraphics();
	drawn = true;

	if(showSplit)
		g->DrawLine(screenPos + Vec2{ splitPosition, 1 }, screenPos + Vec2{ splitPosition, Size.Y-2 }, 0xB4B4B4_rgb);
}

}
