#ifndef TEXTPROMPT_H_
#define TEXTPROMPT_H_

#include "gui/interface/Window.h"
#include "gui/interface/Textbox.h"

#include <functional>

class TextPrompt : public ui::Window
{
	struct TextDialogueCallback
	{
		std::function<void (String const &)> text;
		std::function<void ()> cancel;
	};

	TextDialogueCallback callback;

protected:
	ui::Textbox * textField;

public:
	TextPrompt(String title, String message, String text, ui::Textbox::ValidInput inputType, String placeholder, bool multiline, TextDialogueCallback callback_ = {});
	virtual ~TextPrompt() = default;

	static String Blocking(String title, String message, String text, ui::Textbox::ValidInput inputType, String placeholder, bool multiline);
	void OnDraw() override;
};

#endif /* TEXTPROMPT_H_ */
