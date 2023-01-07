#include <iostream>
#include "Appearance.h"
#include "graphics/Graphics.h"

namespace ui
{
	Appearance::Appearance():
		texture(NULL),

		VerticalAlign(AlignMiddle),
		HorizontalAlign(AlignCentre),

		BackgroundHover(20, 20, 20),
		BackgroundInactive(0, 0, 0),
		BackgroundActive(255, 255, 255),
		BackgroundDisabled(10, 10, 10),

		TextHover(255, 255, 255),
		TextInactive(255, 255, 255),
		TextActive(0, 0, 0),
		TextDisabled(100, 100, 100),

		BorderHover(255, 255, 255),
		BorderInactive(200, 200, 200),
		BorderActive(235, 235, 235),
		BorderDisabled(100, 100, 100),

		Margin(1, 4),
		Border(1),

		icon(NoIcon)
	{}

	VideoBuffer * Appearance::GetTexture()
	{
		return texture;
	}

	void Appearance::SetTexture(VideoBuffer * texture)
	{
		delete this->texture;
		if(texture)
			this->texture = new VideoBuffer(texture);
		else
			this->texture = NULL;
	}

	ui::Colour Appearance::GetBackgroundColour(bool enabled, bool active, bool hover)
	{
		if (!enabled)
		{
			return BackgroundDisabled;
		}
		if (active)
		{
			return BackgroundActive;
		}
		return hover ? BackgroundHover : BackgroundInactive;
	}

	ui::Colour Appearance::GetTextColour(bool enabled, bool active, bool hover)
	{
		if (!enabled)
		{
			return TextDisabled;
		}
		if (active)
		{
			return TextActive;
		}
		return hover ? TextHover : TextInactive;
	}

	ui::Colour Appearance::GetBorderColour(bool enabled, bool active, bool hover)
	{
		if (!enabled)
		{
			return BorderDisabled;
		}
		if (active)
		{
			return BorderActive;
		}
		return hover ? BorderHover : BorderInactive;
	}

	Appearance::~Appearance()
	{
		delete texture;
	}

}
