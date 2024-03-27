#include "LoginView.h"
#include "Config.h"
#include "LoginModel.h"
#include "LoginController.h"
#include "graphics/Graphics.h"
#include "gui/interface/Button.h"
#include "gui/interface/Engine.h"
#include "gui/interface/Label.h"
#include "gui/interface/RichLabel.h"
#include "gui/interface/Textbox.h"
#include "gui/Style.h"
#include "client/Client.h"
#include "Misc.h"
#include <SDL.h>

constexpr auto defaultSize = ui::Point(200, 87);
constexpr auto touchDefaultSize = defaultSize + Vec2{ 0, 28 };

LoginView::LoginView():
	ui::Window(ui::Point(-1, -1), ui::IfTouchUI(touchDefaultSize, defaultSize))
{
	targetSize.SetTarget(float(Size.Y));
	targetSize.SetValue(float(Size.Y));

	titleLabel = new ui::Label(ui::Point(4, 5), ui::Point(Size.X - 16, 16), "Server login");
	titleLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	titleLabel->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	AddComponent(titleLabel);

	usernameField = new ui::Textbox(ui::Point(8, 25), ui::IfTouchUI<ui::Point>({ Size.X - 16, 26 }, { Size.X - 16, 17 }), Client::Ref().GetAuthUser().Username.FromUtf8(), "[username]");
	usernameField->Appearance.icon = IconUsername;
	usernameField->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	usernameField->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	AddComponent(usernameField);

	passwordField = new ui::Textbox(ui::IfTouchUI<ui::Point>({ 8, 55 }, { 8, 46 }), ui::IfTouchUI<ui::Point>({ Size.X - 16, 26 }, { Size.X - 16, 17 }), "", "[password]");
	passwordField->Appearance.icon = IconPassword;
	passwordField->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	passwordField->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	passwordField->SetHidden(true);
	AddComponent(passwordField);

	infoLabel = new ui::RichLabel(ui::IfTouchUI<ui::Point>({ 6, 85 }, { 6, 67 }), ui::Point(Size.X - 12, 16), "");
	infoLabel->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	infoLabel->Appearance.VerticalAlign = ui::Appearance::AlignTop;
	infoLabel->SetMultiline(true);
	infoLabel->Visible = false;
	AddComponent(infoLabel);

	cancelButton = new ui::Button(ui::IfTouchUI<ui::Point>({ 0, Size.Y - 26 }, { 0, Size.Y - 17 }), ui::IfTouchUI<ui::Point>({ 101, 26 }, { 101, 17 }), "Sign out");
	cancelButton->Appearance.HorizontalAlign = ui::Appearance::AlignLeft;
	cancelButton->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	cancelButton->SetActionCallback({ [this] {
		c->Logout();
	} });
	AddComponent(cancelButton);

	loginButton = new ui::Button(ui::IfTouchUI<ui::Point>({ Size.X - 100, Size.Y - 26 }, { Size.X - 100, Size.Y - 17 }), ui::IfTouchUI<ui::Point>({ 100, 26 }, { 100, 17 }), "Sign in");
	loginButton->Appearance.HorizontalAlign = ui::Appearance::AlignRight;
	loginButton->Appearance.VerticalAlign = ui::Appearance::AlignMiddle;
	loginButton->Appearance.TextInactive = style::Colour::ConfirmButton;
	loginButton->SetActionCallback({ [this] {
		c->Login(usernameField->GetText().ToUtf8(), passwordField->GetText().ToUtf8());
	} });
	AddComponent(loginButton);
	SetOkayButton(loginButton);

	if (!ui::Engine::Ref().TouchUI)
	{
		FocusComponent(usernameField);
	}
}

void LoginView::OnKeyPress(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt)
{
	if (repeat)
		return;
	switch(key)
	{
	case SDLK_TAB:
		if(IsFocused(usernameField))
			FocusComponent(passwordField);
		else
			FocusComponent(usernameField);
		break;
	}
}

void LoginView::OnTryExit(ExitMethod method)
{
	CloseActiveWindow();
}

void LoginView::NotifyStatusChanged(LoginModel * sender)
{
	auto statusText = sender->GetStatusText();
	auto notWorking = sender->GetStatus() != loginWorking;
	auto userID = Client::Ref().GetAuthUser().UserID;
	if (!statusText.size() && !userID && notWorking)
	{
		statusText = String::Build("Don't have an account? {a:", SERVER, "/Register.html", "|\btRegister here\x0E}.");
	}
	infoLabel->Visible = statusText.size();
	infoLabel->SetText(statusText);
	infoLabel->AutoHeight();
	loginButton->Enabled = notWorking;
	cancelButton->Enabled = notWorking && userID;
	usernameField->Enabled = notWorking;
	passwordField->Enabled = notWorking;
	if (infoLabel->Visible)
	{
		targetSize.SetTarget(float(defaultSize.Y + infoLabel->Size.Y));
	}
	else
	{
		targetSize.SetTarget(float(defaultSize.Y));
	}
	if (sender->GetStatus() == loginSucceeded)
	{
		c->Exit();
	}
}

void LoginView::OnTick()
{
	c->Tick();
	Size.Y = int(targetSize.GetValue());
	loginButton->Position.Y = Size.Y - ui::IfTouchUI(26, 17);
	cancelButton->Position.Y = Size.Y - ui::IfTouchUI(26, 17);
}

void LoginView::OnDraw()
{
	Graphics * g = GetGraphics();
	g->DrawFilledRect(RectSized(Position - Vec2{ 1, 1 }, Size + Vec2{ 2, 2 }), 0x000000_rgb);
	g->DrawRect(RectSized(Position, Size), 0xFFFFFF_rgb);
}
