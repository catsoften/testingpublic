#pragma once

#include "Button.h"

namespace ui
{

class SplitButton : public Button
{
	bool rightDown;
	bool leftDown;
	bool showSplit;
	int splitPosition;
	String toolTip2;

	struct SplitButtonAction
	{
		std::function<void ()> left, right;
	};
	SplitButtonAction actionCallback;

public:
	SplitButton(Point position, Point size, String buttonText, String toolTip, String toolTip2, int split);
	virtual ~SplitButton() = default;

	void SetRightToolTip(String tooltip);
	bool GetShowSplit();
	void SetShowSplit(bool split);
	inline SplitButtonAction const &GetSplitActionCallback() { return actionCallback; }
	inline void SetSplitActionCallback(SplitButtonAction const &action) { actionCallback = action; }
	void SetToolTip(int x, int y);
	void OnMouseClick(int x, int y, unsigned int button) override;
	void OnMouseHover(int x, int y) override;
	void OnMouseEnter(int x, int y) override;
	void TextPosition(String ButtonText) override;
	void SetToolTips(String newToolTip1, String newToolTip2);
	void OnMouseDown(int x, int y, unsigned int button) override;
	void DoRightAction();
	void DoLeftAction();
	void Draw(const Point& screenPos) override;
};

}
