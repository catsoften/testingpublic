#pragma once
#include "common/String.h"
#include "gui/interface/Window.h"
#include "simulation/Sample.h"
#include "graphics/FindingElement.h"
#include <ctime>
#include <deque>
#include <memory>
#include <vector>
#include <optional>

enum DrawMode
{
	DrawPoints, DrawLine, DrawRect, DrawFill
};

enum SelectMode
{
	SelectNone, SelectStamp, SelectCopy, SelectCut, PlaceSave
};

enum TouchMenu
{
	MenuNone, MenuMain, MenuBrushOptions, MenuQuickOptions
};

namespace ui
{
	class Button;
	class Slider;
	class SplitButton;
	class Textbox;
}

class MenuButton;
class Renderer;
class VideoBuffer;
class ToolButton;
class GameController;
class Brush;
class GameModel;
class GameView: public ui::Window
{
protected:
	bool isMouseDown;
	bool skipDraw;
	bool zoomEnabled;
	bool zoomCursorFixed;
	bool mouseInZoom;
	bool enableZoomOnTouch;
	bool drawSnap;
	bool shiftBehaviour;
	bool ctrlBehaviour;
	bool altBehaviour;
	bool showHud;
	bool showBrush;
	bool showDebug;
	int delayedActiveMenu;
	bool wallBrush;
	bool toolBrush;
	bool decoBrush;
	bool windTool;
	int toolIndex;
	int currentSaveType;
	int lastMenu;

	TouchMenu touchMenu;

	int toolTipPresence;
	String toolTip;
	bool isToolTipFadingIn;
	ui::Point toolTipPosition;
	int infoTipPresence;
	String infoTip;
	int buttonTipShow;
	String buttonTip;
	bool isButtonTipFadingIn;
	int introText;
	String introTextMessage;

	bool doScreenshot;
	int screenshotIndex;
	time_t lastScreenshotTime;
	int recordingIndex;
	bool recording;
	int recordingFolder;

	ui::Point currentPoint, lastPoint;
	GameController * c;
	Renderer * ren;
	Brush const *activeBrush;
	//UI Elements
	std::vector<ui::Button*> touchMenuButtons;

	std::vector<ui::Button*> mainMenuButtons;
	ui::Button * authorshipInfoButton{};

	std::vector<ui::Button*> brushOptionsMenuButtons;
	ui::Button * smallerBrushButton{};
	ui::Button * largerBrushButton{};
	ui::Button * pasteButton{};
	ui::Button * undoButton{};
	ui::Button * redoButton{};

	std::vector<ui::Button*> quickOptionsMenuButtons;

	std::vector<MenuButton*> menuButtons;

	std::vector<ToolButton*> toolButtons;
	std::vector<ui::Component*> notificationComponents;
	std::deque<std::pair<String, int> > logEntries;
	ui::Button * scrollBar{};
	ui::Button * searchButton;
	ui::Button * reloadButton;
	ui::SplitButton * saveSimulationButton{};
	ui::Button * touchSaveSimulationButton{};
	bool saveSimulationButtonEnabled;
	bool saveReuploadAllowed;
	ui::Button * downVoteButton;
	ui::Button * upVoteButton;
	ui::Button * tagSimulationButton;
	ui::Button * clearSimButton;
	ui::SplitButton * loginButton;
	ui::Button * simulationOptionButton;
	ui::Button * displayModeButton;
	ui::Button * pauseButton;

	ui::Button * colourPicker;
	std::vector<ToolButton*> colourPresets;

	virtual void SetTouchMenu(TouchMenu newTouchMenu) = 0;

	DrawMode drawMode;
	ui::Point drawPoint1;
	ui::Point drawPoint2;

	SelectMode selectMode;
	ui::Point selectPoint1;
	ui::Point selectPoint2;

	ui::Point currentMouse;
	ui::Point mousePosition;

	std::unique_ptr<VideoBuffer> placeSaveThumb;
	Mat2<int> placeSaveTransform = Mat2<int>::Identity;
	Vec2<int> placeSaveTranslate = Vec2<int>::Zero;
	void TranslateSave(Vec2<int> addToTranslate);
	void TransformSave(Mat2<int> mulToTransform);
	void ApplyTransformPlaceSave();

	SimulationSample sample;

	virtual void updateToolButtonScroll() = 0;

	virtual void SetSaveButtonTooltips() = 0;
	virtual void SetSaveButtonShowSplit(bool split) = 0;
	virtual ui::Button * GetSaveButton() = 0;

	void enableShiftBehaviour();
	void disableShiftBehaviour();
	void enableCtrlBehaviour();
	void disableCtrlBehaviour();
	void enableAltBehaviour();
	void disableAltBehaviour();
	void UpdateDrawMode();
	void UpdateToolStrength();

	Vec2<int> PlaceSavePos() const;

	std::optional<FindingElement> FindingElementCandidate() const;

public:
	GameView();
	virtual ~GameView();

	//Breaks MVC, but any other way is going to be more of a mess.
	ui::Point GetMousePosition();
	void SetSample(SimulationSample sample);
	void SetHudEnable(bool hudState);
	bool GetHudEnable();
	void SetBrushEnable(bool hudState);
	bool GetBrushEnable();
	void SetDebugHUD(bool mode);
	bool GetDebugHUD();
	bool GetPlacingSave();
	bool GetPlacingZoom();
	void SetActiveMenuDelayed(int activeMenu) { delayedActiveMenu = activeMenu; }
	bool CtrlBehaviour(){ return ctrlBehaviour; }
	bool ShiftBehaviour(){ return shiftBehaviour; }
	bool AltBehaviour(){ return altBehaviour; }
	SelectMode GetSelectMode() { return selectMode; }
	void ShowAuthorshipInfo();
	void ToggleFind();
	void BeginStampSelection();
	void BeginCopy();
	void BeginCut();
	void BeginPaste();
	void TryLoadLatestStamp();
	ByteString TakeScreenshot(int captureUI, int fileType);
	int Record(bool record);

	//all of these are only here for one debug lines
	bool GetMouseDown() { return isMouseDown; }
	bool GetDrawingLine() { return drawMode == DrawLine && isMouseDown && selectMode == SelectNone; }
	bool GetDrawSnap() { return drawSnap; }
	ui::Point GetLineStartCoords() { return drawPoint1; }
	ui::Point GetLineFinishCoords() { return currentMouse; }
	ui::Point GetCurrentMouse() { return currentMouse; }
	ui::Point lineSnapCoords(ui::Point point1, ui::Point point2);
	ui::Point rectSnapCoords(ui::Point point1, ui::Point point2);

	void AttachController(GameController * _c){ c = _c; }
	void NotifyRendererChanged(GameModel * sender);
	void NotifySimulationChanged(GameModel * sender);
	void NotifyPausedChanged(GameModel * sender);
	void NotifySaveChanged(GameModel * sender);
	void NotifyBrushChanged(GameModel * sender);
	virtual void NotifyMenuListChanged(GameModel * sender) = 0;
	virtual void NotifyToolListChanged(GameModel * sender) = 0;
	void NotifyActiveToolsChanged(GameModel * sender);
	void NotifyUserChanged(GameModel * sender);
	void NotifyZoomChanged(GameModel * sender);
	void NotifyColourSelectorVisibilityChanged(GameModel * sender);
	void NotifyColourSelectorColourChanged(GameModel * sender);
	void NotifyColourPresetsChanged(GameModel * sender);
	void NotifyColourActivePresetChanged(GameModel * sender);
	void NotifyPlaceSaveChanged(GameModel * sender);
	void NotifyTransformedPlaceSaveChanged(GameModel *sender);
	void NotifyNotificationsChanged(GameModel * sender);
	void NotifyLogChanged(GameModel * sender, String entry);
	void NotifyToolTipChanged(GameModel * sender);
	void NotifyInfoTipChanged(GameModel * sender);
	void NotifyLastToolChanged(GameModel * sender);


	virtual void ToolTip(ui::Point senderPosition, String toolTip) = 0;

	void OnMouseMove(int x, int y, int dx, int dy) override;
	void OnMouseDown(int x, int y, unsigned button) override;
	void OnMouseUp(int x, int y, unsigned button) override;
	void OnMouseWheel(int x, int y, int d) override;
	void OnKeyPress(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override;
	void OnKeyRelease(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override;
	void OnTick(float dt) override;
	void OnDraw() override;
	void OnBlur() override;
	void OnFileDrop(ByteString filename) override;

	//Top-level handlers, for Lua interface
	void DoExit() override;
	void DoDraw() override;
	void DoMouseMove(int x, int y, int dx, int dy) override;
	void DoMouseDown(int x, int y, unsigned button) override;
	void DoMouseUp(int x, int y, unsigned button) override;
	void DoMouseWheel(int x, int y, int d) override;
	void DoTextInput(String text) override;
	void DoTextEditing(String text) override;
	void DoKeyPress(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override;
	void DoKeyRelease(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override;

	void SkipIntroText();
};

class GameViewBasic : public GameView
{
	void SetTouchMenu(TouchMenu newTouchMenu) override;
	void updateToolButtonScroll() override;
	void SetSaveButtonTooltips() override;
	void SetSaveButtonShowSplit(bool split) override;
	ui::Button * GetSaveButton() override;

public:
	GameViewBasic();
	void NotifyMenuListChanged(GameModel * sender) override;
	void NotifyToolListChanged(GameModel * sender) override;
	void ToolTip(ui::Point senderPosition, String toolTip) override;
};

class GameViewTouchUI : public GameView
{
	void SetTouchMenu(TouchMenu newTouchMenu) override;
	void updateToolButtonScroll() override;
	void SetSaveButtonTooltips() override;
	void SetSaveButtonShowSplit(bool split) override;
	ui::Button * GetSaveButton() override;

public:
	GameViewTouchUI();
	void NotifyMenuListChanged(GameModel * sender) override;
	void NotifyToolListChanged(GameModel * sender) override;
	void ToolTip(ui::Point senderPosition, String toolTip) override;
};

