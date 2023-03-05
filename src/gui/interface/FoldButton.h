#pragma once

#include <functional>

#include "Component.h"

#include "common/String.h"

namespace ui
{
	class FoldButton : public Component
	{
	public:
		using ExpandCallback = std::function<void(int, bool)>;

	protected:
		String text;
		int id;
		bool expanded;

		ExpandCallback expandCallback = nullptr;

		bool isMouseInside = false;
		bool isMouseDown = false;

	public:
		FoldButton(Point position, Point size, String text_, int id_, bool expanded_);

		String GetText();
		void SetText(String text_);

		bool GetExpanded();
		void SetExpanded(bool expanded_);

		void SetExpandCallback(FoldButton::ExpandCallback expandCallback_);

		void Draw(const Point& screenPos) override;
		void OnMouseMoved(int localx, int localy, int dx, int dy) override;
		void OnMouseDown(int x, int y, unsigned button) override;
		void OnMouseUp(int x, int y, unsigned button) override;
	};
}
