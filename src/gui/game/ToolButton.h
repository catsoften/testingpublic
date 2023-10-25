#pragma once
#include "gui/interface/Button.h"

#include "Config.h"

class Tool;

class ToolButton: public ui::Button
{
	int currentSelection;
	ByteString toolIdentifier;
public:
	static constexpr ui::Point DEFAULT_SIZE = TOUCH_UI ? ui::Point(36, 24) : ui::Point(30, 18);

	ToolButton(ui::Point position, ui::Point size, String text, ByteString toolIdentifier, String toolTip = String());
	void OnMouseUnclick(int x, int y, unsigned int button) override;
	void OnMouseUp(int x, int y, unsigned int button) override;
	void OnMouseClick(int x, int y, unsigned int button) override;
	void Draw(const ui::Point& screenPos) override;
	void SetSelectionState(int state);
	int GetSelectionState();
	Tool *tool;
	Rect<int> ClipRect = RectSized(Vec2<int>::Zero, Vec2<int>::Zero);
};
