#include "Tool.h"

#include "simulation/Simulation.h"

#include "gui/Style.h"
#include "gui/interface/Window.h"
#include "gui/interface/Button.h"
#include "gui/interface/Label.h"
#include "gui/interface/Textbox.h"
#include "gui/interface/DropDown.h"
#include "gui/interface/Checkbox.h"
#include "gui/game/GameModel.h"

#include "graphics/Graphics.h"
#include "gui/interface/Engine.h"
#include "gui/game/config_tool/util.h"
#include "simulation/transform/transform.h"

#include <cmath>
#include <unordered_map>

#define PI 3.141592f

class RotateWindow: public ui::Window {
public:
	RotateTool * tool;
	Simulation * sim;
    ui::Textbox * rotation_input;
    ui::Checkbox * layer, * quality_rotate;
    ui::Label * messageLabel;

    int point_count = 0, radius = 0;
    int dragging = 0; // 1 = rotate, 2 = window
    float rotation = 0.0f, prev_rotation = 0.0f; // In degrees for input

    pixel_vector pixels, rot_pixels;
    element_vector elements;

    ui::Point p1 = ui::Point(0, 0);
    ui::Point p2 = ui::Point(0, 0);

    const int rotate_thingy_radius = 7;
    int cx, cy, dragdx, dragdy;

	RotateWindow(RotateTool * tool_, Simulation * sim_, ui::Point pos);
	void OnDraw() override;
	void DoDraw() override;
	void DoMouseMove(int x, int y, int dx, int dy) override;
	void DoMouseUp(int x, int y, unsigned button) override {
        ui::Window::DoMouseUp(x, y, button);
        dragging = 0;
	}
	void DoMouseWheel(int x, int y, int d) override {
        ui::Window::DoMouseWheel(x, y, d);
	}
	void DoKeyPress(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override {
		ui::Window::DoKeyPress(key, scan, repeat, shift, ctrl, alt);
	}
	void DoKeyRelease(int key, int scan, bool repeat, bool shift, bool ctrl, bool alt) override {
		ui::Window::DoKeyRelease(key, scan, repeat, shift, ctrl, alt);
	}
    void DoMouseDown(int x, int y, unsigned button) override;
	virtual ~RotateWindow() {}
	void OnTryExit(ui::Window::ExitMethod method) override;
};

RotateWindow::RotateWindow(RotateTool * tool_, Simulation * sim_, ui::Point pos):
	ui::Window(ui::Point(XRES - 200, YRES - 120), ui::Point(200, 100)),
	tool(tool_),
	sim(sim_),
    p1(pos), p2(pos)
{
    align_window_far(Position, pos, Size);

	messageLabel = make_left_label(ui::Point(4, 5), ui::Point(Size.X-8, 15), "Rotate Selection");
	messageLabel->SetTextColour(style::Colour::InformationTitle);
	AddComponent(messageLabel);

    // Rotation textbox
    ui::Label * tempLabel = make_left_label(ui::Point(4, 22), ui::Point(Size.X - 8, 15), "Rotation (deg)");
    AddComponent(tempLabel);
    rotation_input = make_left_textbox(ui::Point(Size.X / 2 - 4, 22), ui::Point(Size.X / 2 - 4, 15), "0.0", "0.0");
    FLOAT_INPUT(rotation_input, rotation);
    AddComponent(rotation_input);

    // Checkboxes
    layer = new ui::Checkbox(ui::Point(6, 42), ui::Point(Size.X, 15), "Delete stacked particles (If any)", "idk");
    layer->SetChecked(true);
    AddComponent(layer);

    quality_rotate = new ui::Checkbox(ui::Point(6, 60), ui::Point(Size.X, 15), "Auto-fill gaps", "idk");
    AddComponent(quality_rotate);

	ui::Button * okayButton = make_center_button(ui::Point(Size.X / 2, Size.Y-16), ui::Point(Size.X / 2, 16), "OK");
	okayButton->SetActionCallback({ [this] {
        // Apply rotation
        element_vector final;
        fastrot_vector<element_vector>(elements, rotation / 180 * PI, final, cx, cy);
        std::unordered_map<int, int> stacking_check;
    
        for (auto &point : final) {
            // Skip if already have new element at this location
            // Key is a fancy somewhat unique function for a pair of coordinates
            // (Cantor pairing function)
            int key = map_key(point.first.X, point.first.Y);
            if (layer->GetChecked() && stacking_check.count(key)) {
                sim->kill_part(point.second);
                continue;
            }

            // Setting .x and .y doesn't update pmap, so we're creating an exact copy
            int ni = sim->create_part(-3, point.first.X, point.first.Y, sim->parts[point.second].type);
            sim->parts[ni].temp = sim->parts[point.second].temp;
            sim->parts[ni].vx = sim->parts[point.second].vx;
            sim->parts[ni].vy = sim->parts[point.second].vy;
            sim->parts[ni].tmp = sim->parts[point.second].tmp;
            sim->parts[ni].tmp2 = sim->parts[point.second].tmp2;
            sim->parts[ni].tmp3 = sim->parts[point.second].tmp3;
            sim->parts[ni].tmp4 = sim->parts[point.second].tmp4;
            sim->parts[ni].flags = sim->parts[point.second].flags;
            sim->parts[ni].ctype = sim->parts[point.second].ctype;
            sim->parts[ni].life = sim->parts[point.second].life;
            sim->parts[ni].dcolour = sim->parts[point.second].dcolour;

            stacking_check[key] = sim->parts[ni].type;
            sim->kill_part(point.second);
        }

        // Auto detect gaps
        if (quality_rotate->GetChecked()) {
            for (auto &point : final) {
                for (int rx1 = -1; rx1 <= 1; ++rx1)
                for (int ry1 = -1; ry1 <= 1; ++ry1) {
                    if (!stacking_check.count(map_key(point.first.X + rx1, point.first.Y + ry1))) { // Potentional gap?
                        int element = 0, key;
                        int copyx, copyy;
                        for (int rx2 = -1; rx2 <= 1; ++rx2)
                        for (int ry2 = -1; ry2 <= 1; ++ry2) {
                            if (!((rx2 == 0 || ry2 == 0) && (rx2 || ry2)))
                                continue;
                            key = map_key(point.first.X + rx1 + rx2, point.first.Y + ry1 + ry2);
                            if (!stacking_check.count(key))
                                goto failed; // A gap must have at least 4 surrounding elements not diagonal
                            else {
                                element = stacking_check[key];
                                copyx = point.first.X + rx1 + rx2;
                                copyy = point.first.Y + ry1 + ry2;
                            }
                        }
                        int ni = sim->create_part(-1, point.first.X + rx1, point.first.Y + ry1, element);
                        int r = sim->pmap[copyy][copyx];
                        sim->parts[ni].ctype = sim->parts[ID(r)].ctype;
                        sim->parts[ni].life = sim->parts[ID(r)].life;
                        sim->parts[ni].tmp = sim->parts[ID(r)].tmp;
                        sim->parts[ni].tmp2 = sim->parts[ID(r)].tmp2;
                        sim->parts[ni].tmp3 = sim->parts[ID(r)].tmp3;
                        sim->parts[ni].tmp4 = sim->parts[ID(r)].tmp4;
                    }
                    failed:
                    continue;
                }
            }
        }

		CloseActiveWindow();
		SelfDestruct();
	}});
	AddComponent(okayButton);
	SetOkayButton(okayButton);

    ui::Button * cancelButton = make_center_button(ui::Point(0, Size.Y-16), ui::Point(Size.X / 2, 16), "Cancel");
    CANCEL_BUTTON(cancelButton);
    AddComponent(cancelButton);
	MakeActiveWindow();
}

void RotateWindow::OnTryExit(ui::Window::ExitMethod method) {
    if (method == ui::Window::MouseOutside) { // Clicking outside shouldn't exit since we're clicking
        return;
    }
	CloseActiveWindow();
	SelfDestruct();
}

void RotateWindow::DoMouseDown(int x, int y, unsigned button) {
    ui::Window::DoMouseDown(x, y, button);

    // Clicked on header
	if (x >= Position.X && x <= Position.X + Size.X && y >= Position.Y && y <= Position.Y + 16) {
		dragging = 2;
		dragdx = x - Position.X;
		dragdy = y - Position.Y;
        return;
	}

    if (point_count <= 2) {
        ++point_count;
        if (point_count == 3) { // Create a graphic to rotate
            ++point_count;
            
            // Determine top left corner
            int topx = std::min(p1.X, p2.X);
            int topy = std::min(p1.Y, p2.Y);
            int botx = std::max(p1.X, p2.X);
            int boty = std::max(p1.Y, p2.Y);

            // Iterate
            for (int sx = topx; sx <= botx; ++sx)
                for (int sy = topy; sy <= boty; ++sy) {
                    int r = sim->pmap[sy][sx];
                    if (!r) r = sim->photons[sy][sx];
                    if (!r) continue;

                    int color = sim->elements[TYP(r)].Colour;
                    elements.push_back(std::make_pair(ui::Point(sx, sy), ID(r)));
                    pixels.push_back(std::make_pair(ui::Point(sx, sy),
                        ui::Colour(PIXR(color), PIXG(color), PIXB(color), 150)));
                }
        }
    }

    // We're still placing down points
    if (point_count == 2)
        p2 = ui::Point(x, y);

    // Drag the rotation thing
    if (point_count > 2) {
        int dx = x - (cx + radius * cos(rotation / 180 * PI));
        int dy = y - (cy + radius * sin(rotation / 180 * PI));
        if (sqrtf(dx * dx + dy * dy) <= rotate_thingy_radius)
            dragging = 1;
    }
}

void RotateWindow::DoMouseMove(int x, int y, int dx, int dy) {
	ui::Window::DoMouseMove(x, y, dx, dy);

    if (point_count == 1) // Seting 2nd point
        p2 = ui::Point(x, y);
    else if (point_count > 2 && dragging == 1) { // Set rotation
        prev_rotation = rotation;
        rotation = atan2(y - cy, x - cx) / PI * 180;
        if (rotation < 0) rotation = 360 + rotation;

        // Round rotation to nearest 1
        rotation = round(rotation); 
        rotation_input->SetText(String::Build(rotation));
    }

    if (dragging == 2) {
        Position.X = x - dragdx;
		Position.Y = y - dragdy;

		if (Position.X < 0) Position.X = 0;
		if (Position.Y < 0) Position.Y = 0;
		if (Position.X + Size.X > XRES) Position.X = XRES - Size.X;
		if (Position.Y + Size.Y > YRES) Position.Y = YRES - Size.Y;
		messageLabel->ClearSelection(); // Avoid selecting while dragging
    }
}

void RotateWindow::DoDraw() {
    Graphics * g = GetGraphics();

    // Draw bounding box
    if (point_count >= 1) {
        // Create corners
        int c1x = p1.X, c1y = p1.Y, c2x = p1.X, c2y = p2.Y,
            c3x = p2.X, c3y = p1.Y, c4x = p2.X, c4y = p2.Y;
        cx = (p1.X + p2.X) / 2, cy = (p1.Y + p2.Y) / 2;
        rotate_around(c1x, c1y, cx, cy, rotation / 180 * PI);
        rotate_around(c2x, c2y, cx, cy, rotation / 180 * PI);
        rotate_around(c3x, c3y, cx, cy, rotation / 180 * PI);
        rotate_around(c4x, c4y, cx, cy, rotation / 180 * PI);

        radius = std::max(abs(p1.X - p2.X), abs(p1.Y - p2.Y)) / 2;

        g->draw_line(c1x, c1y, c2x, c2y, 255, 255, 255, 150);
        g->draw_line(c1x, c1y, c3x, c3y, 255, 255, 255, 150);
        g->draw_line(c4x, c4y, c2x, c2y, 255, 255, 255, 150);
        g->draw_line(c4x, c4y, c3x, c3y, 255, 255, 255, 150);
    }
    // Draw rotate circle option and preview
    if (point_count >= 2) {
        if (prev_rotation != rotation)
            fastrot_vector<pixel_vector>(pixels, rotation / 180 * PI, rot_pixels, (float)cx, (float)cy);
        for (auto &t : rot_pixels)
            g->drawrect(t.first.X, t.first.Y, 1, 1, t.second.Red, t.second.Green, t.second.Blue, t.second.Alpha);

        g->drawcircle(cx, cy, radius, radius, 255, 255, 255, 155);
        g->fillcircle(cx + radius * cos(rotation / 180 * PI), cy + radius * sin(rotation / 180 * PI),
            rotate_thingy_radius, rotate_thingy_radius, 255, 255, 255, 255);
    }

    g->clearrect(XRES, 0, WINDOWW - XRES, WINDOWH);
	g->clearrect(0, YRES, WINDOWW, WINDOWH - YRES);

    ui::Window::DoDraw();
}

void RotateWindow::OnDraw() {
	Graphics * g = GetGraphics();
	g->clearrect(Position.X-2, Position.Y-2, Size.X+3, Size.Y+3);
	g->drawrect(Position.X, Position.Y, Size.X, Size.Y, 200, 200, 200, 255);
}

void RotateTool::Click(Simulation * sim, Brush * brush, ui::Point position) {
    RotateWindow * t = new RotateWindow(this, sim, position);
    t->p1 = position;
    ++t->point_count;
}

#undef PI
