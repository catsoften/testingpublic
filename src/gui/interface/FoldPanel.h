#pragma once

#include "Panel.h"

#include "common/String.h"

#include <functional>

namespace ui
{
	class FoldPanel: public Panel
	{
	public:
		using ExpandCallback = std::function<void(bool)>;

	protected:
		int expandedY;
		bool expanded;

		ExpandCallback expandCallback;

		bool isMouseInside, isMouseDown;
		String text;

	public:
		FoldPanel(Point position, Point size, String text_);

		bool GetExpanded();
		void SetExpanded(bool expanded_);

		void SetExpandCallback(FoldPanel::ExpandCallback expandCallback_);

		String GetText();
		void SetText(String text_);

		void AddChild(Component* c);

		void Draw(const Point& screenPos) override;
		void XOnMouseMoved(int localx, int localy, int dx, int dy) override;
		void XOnMouseDown(int x, int y, unsigned button) override;
		void XOnMouseUp(int x, int y, unsigned button) override;
	};
}
