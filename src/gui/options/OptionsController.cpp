#include "OptionsController.h"

#include "ModOptionsView.h"
#include "OptionsView.h"
#include "OptionsModel.h"

#include "Controller.h"

OptionsController::OptionsController(GameModel * gModel_, bool modOption, std::function<void ()> onDone_):
	gModel(gModel_),
	onDone(onDone_),
	HasExited(false)
{
	view = modOption ?
		reinterpret_cast<OptionsView*>(new ModOptionsView()) :
		reinterpret_cast<OptionsView*>(new VanillaOptionsView());
	model = new OptionsModel(gModel);
	model->AddObserver(view);
	view->AttachController(this);
}

void OptionsController::SetHeatSimulation(bool state)
{
	model->SetHeatSimulation(state);
}

void OptionsController::SetAmbientHeatSimulation(bool state)
{
	model->SetAmbientHeatSimulation(state);
}

void OptionsController::SetNewtonianGravity(bool state)
{
	model->SetNewtonianGravity(state);
}

void OptionsController::SetWaterEqualisation(bool state)
{
	model->SetWaterEqualisation(state);
}

void OptionsController::SetGravityMode(int gravityMode)
{
	model->SetGravityMode(gravityMode);
}

void OptionsController::SetCustomGravityX(float x)
{
	model->SetCustomGravityX(x);
}

void OptionsController::SetCustomGravityY(float y)
{
	model->SetCustomGravityY(y);
}

void OptionsController::SetAirMode(int airMode)
{
	model->SetAirMode(airMode);
}

void OptionsController::SetAmbientAirTemperature(float ambientAirTemp)
{
	model->SetAmbientAirTemperature(ambientAirTemp);
}

void OptionsController::SetEdgeMode(int edgeMode)
{
	model->SetEdgeMode(edgeMode);
}

void OptionsController::SetTemperatureScale(int temperatureScale)
{
	model->SetTemperatureScale(temperatureScale);
}

void OptionsController::SetFullscreen(bool fullscreen)
{
	model->SetFullscreen(fullscreen);
}

void OptionsController::SetAltFullscreen(bool altFullscreen)
{
	model->SetAltFullscreen(altFullscreen);
}

void OptionsController::SetForceIntegerScaling(bool forceIntegerScaling)
{
	model->SetForceIntegerScaling(forceIntegerScaling);
}

void OptionsController::SetShowAvatars(bool showAvatars)
{
	model->SetShowAvatars(showAvatars);
}

void OptionsController::SetScale(int scale)
{
	model->SetScale(scale);
}

void OptionsController::SetResizable(bool resizable)
{
	model->SetResizable(resizable);
}

void OptionsController::SetFastQuit(bool fastquit)
{
	model->SetFastQuit(fastquit);
}

void OptionsController::SetDecoSpace(int decoSpace)
{
	model->SetDecoSpace(decoSpace);
}

OptionsView * OptionsController::GetView()
{
	return view;
}

void OptionsController::SetMouseClickrequired(bool mouseClickRequired)
{
	model->SetMouseClickRequired(mouseClickRequired);
}

void OptionsController::SetIncludePressure(bool includePressure)
{
	model->SetIncludePressure(includePressure);
}

void OptionsController::SetPerfectCircle(bool perfectCircle)
{
	model->SetPerfectCircle(perfectCircle);
}

void OptionsController::SetOppositeToolEnabled(bool oppositeTool) {
	model->SetAutoSelectOppositeTool(oppositeTool);
}
void OptionsController::SetSecretModShortcut(bool enabled) {
	model->SetSecretModShortcut(enabled);
}
void OptionsController::SetCrosshairInBrush(bool enabled) {
	model->SetCrosshairInBrush(enabled);
}
void OptionsController::SetHollowBrushes(bool enabled) {
	model->SetHollowBrushes(enabled);
}
void OptionsController::SetAutoHideHUD(bool enabled) {
	model->SetAutoHideHUD(enabled);
}
void OptionsController::SetDimGlowMode(bool enabled) {
	model->SetDimGlowMode(enabled);
}
void OptionsController::SetDrawingFrequencyLimit(int limit) {
	model->SetDrawingFrequencyLimit(limit);
}
void OptionsController::SetFasterRenderer(bool enabled) {
	model->SetFasterRenderer(enabled);
}
void OptionsController::SetSoundEnabled(bool enabled) {
	model->SetSoundEnabled(enabled);
}

void OptionsController::SetMomentumScroll(bool momentumScroll)
{
	model->SetMomentumScroll(momentumScroll);
}

void OptionsController::Exit()
{
	view->CloseActiveWindow();

	if (onDone)
		onDone();
	HasExited = true;
}


OptionsController::~OptionsController()
{
	view->CloseActiveWindow();
	delete model;
	delete view;
}

