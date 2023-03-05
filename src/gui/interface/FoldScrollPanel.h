#pragma once

#include <vector>

#include "ScrollPanel.h"

#include "FoldButton.h"

#include "common/String.h"
#include "graphics/Graphics.h"

namespace ui
{
	class FoldScrollPanel : public ScrollPanel
	{
	protected:
		struct FoldGroup
		{
			FoldButton* button;
			std::vector<Component*> components;
			int groupY;
			bool expanded;
		};

		std::vector<FoldGroup> groups;

		int GetYOffset(int group);

		FoldButton::ExpandCallback foldButtonHandler;

	public:
		FoldScrollPanel(Point position, Point size);

		void NextGroup(String label, int groupY, bool expanded);

		void AddChild(Component* c);

		virtual void XDraw(const Point& screenPos) override;
	};
}
