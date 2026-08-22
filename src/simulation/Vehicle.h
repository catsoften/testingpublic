#pragma once

#include <vector>

#include "common/Vec2.h"
#include "graphics/Pixel.h"

struct Particle;
struct playerst;
class Renderer;
class Simulation;

/**
 * Note that vehicles update TWICE per frame
 *
 * Explainations for vehicle properties:
 * - width:           width in px
 * - height:          height in px
 * - acceleration:    acceleration when moving on ground
 * - flyAcceleration: acceleration when flying
 * - maxSpeed:        vx and vy must be smaller in magnitude than this
 * - collisionSpeed:  min speed for collision damage to vehicle
 * - runoverSpeed:    min speed to runover STKM
 * - rotationSpeed:   radians to rotate per update
 */

struct Vehicle
{
	// Config for all vehicles
	constexpr static int MAX_VEHICLES = 30;
	constexpr static int MIN_STKM_DISTANCE_TO_ENTER = 10; // STKM <= to center pixel will "enter" a vehicle

	enum class Command
	{
		None,
		Up,
		Down,
		Left,
		Right,
		LeftRight,
	};

	static std::vector<int> vehicles;

	static void InitialCollision(Simulation *sim, Particle *parts, int i, const Vehicle &v, bool &hasCollision);
	static bool AttemptMove(Simulation *sim, Particle *parts, int i, const Vehicle &v, int x, int y);
	static void GetPlayerCommand(Simulation *sim, Particle *parts, int i, Command &cmd, Command &cmd2);
	static void GetTarget(Simulation *sim, Particle *parts, int &tarx, int &tary);
	static void ExitVehicle(Simulation *sim, Particle *parts, int i, int x, int y);
	static void UpdateVehicle(Simulation *sim, Particle *parts, int i, const Vehicle &v, float ovx, float ovy);
	static bool CheckSTKM(Simulation *sim, Particle *parts, int i, playerst *data);
	static int CreatePart(Simulation *sim, Particle *parts, int i, Vec2<int> pos, int type, float theta);
	static void DrawVehicle(pixel_rgba const *data, Rect<int> rect, Renderer *ren, float temp, int rider, bool flip, float rotation);
	static void RecalcVehicles(Simulation *sim);

	const int width, height;
	const float acceleration, flyAcceleration, maxSpeed, collisionSpeed, runoverSpeed, rotationSpeed;
};
