#include "Vehicle.h"

#include <cmath>

#include "ElementClasses.h"
#include "ElementGraphics.h"
#include "graphics/Renderer.h"
#include "Simulation.h"
#include "ultimata/ElementUtils.h"
#include "ultimata/Transform.h"

std::vector<int> Vehicle::vehicles = {};

// TODO ULTIMATA: More Vec2s

void Vehicle::InitialCollision(Simulation *sim, Particle *parts, int i, const Vehicle &v, bool &hasCollision)
{
	bool pbl = AttemptMove(sim, parts, i, v, -v.width / 2,  v.height / 2);
	bool pbr = AttemptMove(sim, parts, i, v,  v.width / 2,  v.height / 2);
	bool pbc = AttemptMove(sim, parts, i, v,  0,            v.height / 2);
	bool tbl = AttemptMove(sim, parts, i, v, -v.width / 2, -v.height / 2);
	bool tbr = AttemptMove(sim, parts, i, v,  v.width / 2, -v.height / 2);
	bool tbc = AttemptMove(sim, parts, i, v,  0,           -v.height / 2);
	hasCollision = pbl || pbr || pbc || tbl || tbr || tbc;

	float tmp3 = ToFloat(parts[i].tmp3);

	// Match terrain rotation
	if (pbl ^ pbr && !pbc)
	{
		tmp3 += pbl ? v.rotationSpeed : -v.rotationSpeed;
		parts[i].y -= 0.5f;
	}
	if (tbl ^ tbr && !tbc)
	{
		tmp3 += tbl ? v.rotationSpeed : -v.rotationSpeed;
		parts[i].y += 0.5f;
	}

	// If no collision rotate towards gravity every 0 frames
	if (!hasCollision && sim->frameCount % 10 == 0)
	{
		float targetAngle = 0.0f;
		bool haveTarget = false;
		switch (sim->gravityMode) // TODO ULTIMATA: GetGravityField
		{
			default:
			case 0: // Normal, vertical gravity
				targetAngle = 0.0f;
				haveTarget = true;
				break;

			case 1: // No gravity
				break;

			case 2: // Radial gravity
				targetAngle = std::atan2(parts[i].y - YRES / 2, parts[i].x - XRES / 2) + std::numbers::pi_v<float> / 2;
				haveTarget = true;
				break;

			case 3: // Custom gravity
				break;
		}

		if (haveTarget)
		{
			if (tmp3 > targetAngle)
			{
				tmp3 -= v.rotationSpeed;
			}
			else
			{
				tmp3 += v.rotationSpeed;
			}
		}
	}

	parts[i].tmp3 = FromFloat(tmp3);

	// If sim stkm or stkm2 exists then stop allowing control to this car
	if (parts[i].tmp2 == 1 && sim->player.spwn)
	{
		parts[i].tmp2 = 0;
	}
	else if (parts[i].tmp2 == 2 && sim->player2.spwn)
	{
		parts[i].tmp2 = 0;
	}

	// Prevent spawning of stkm and stkm2 if inside car
	if (parts[i].tmp2 == 1)
	{
		sim->vehicle_p1 = i;
	}
	if (parts[i].tmp2 == 2)
	{
		sim->vehicle_p2 = i;
	}
}

bool Vehicle::AttemptMove(Simulation *sim, Particle *parts, int i, const Vehicle &v, int x, int y)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	if (parts[i].vx == 0.0f && parts[i].vy == 0.0f)
	{
		return false;
	}

	// x, y are initially relative offsets when the vehicle is flat. Rotate and convert
	// to an absolute x y coordinate
	Vec2 pos{ x, y };
	Rotate(pos, ToFloat(parts[i].tmp3));
	x = parts[i].x + pos.X;
	y = parts[i].y + pos.Y;

	if (parts[i].type != PT_HRSE) // Horses don't do collision
	{
		// Search for nearby vehicles
		for (auto ov : vehicles)
		{
			if (ov == i) // Dont collide with self
			{
				continue;
			}

			float dx = std::abs(parts[i].x - parts[ov].x);
			float dy = std::abs(parts[i].y - parts[ov].y);

			if (dx < v.width && dy < v.height)
			{
				if (std::hypot(parts[i].vx, parts[i].vy) > v.collisionSpeed / 2.0f)
				{
					parts[ov].life -= sim->rng.between(10, 50);
					parts[i].life -= sim->rng.between(10, 50);
				}

				parts[i].vx = parts[i].vy = 0.0f;

				// Tank destroys other vehicles
				if (parts[i].type == PT_TANK && parts[ov].type != PT_TANK)
				{
					parts[ov].life = 0;
				}

				break;
			}
		}
	}

	// Test surrounding particles
	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (sim->IsWallBlocking(x + rx, y + ry, PT_CYTK))
			{
				parts[i].vx = parts[i].vy = 0.0f;
				return true;
			}

			auto r = sim->pmap[y + ry][x + rx];
			if (!r)
			{
				continue;
			}
			if (TYP(r) == PT_HRSE)
			{
				continue;
			}

			// Cybertrucks heal from electrons
			if (parts[i].type == PT_CYTK && TYP(r) == PT_ELEC)
			{
				parts[i].life = std::min(parts[i].life + 5, 120);
			}

			if (
				TYP(r) != PT_CYTK && TYP(r) != PT_PRTI && TYP(r) != PT_PRTO && TYP(r) != PT_TRUS &&
				(elements[TYP(r)].Properties & TYPE_PART || elements[TYP(r)].Properties & TYPE_SOLID)
			)
			{
				parts[i].vx = parts[i].vy = 0.0f;
				return true;
			}
		}
	}

	// Try moving in the direction of vx vy
	float largest = std::max(std::abs(parts[i].vx), std::abs(parts[i].vy));

	// Avoid "division by 0" effect where it ends up scanning the entire map
	if (largest <= 1.0f)
	{
		return false;
	}

	float dvx = parts[i].vx / largest, dvy = parts[i].vy / largest;
	float sx = x, sy = y;
	int count = 1;

	while (
		sx >= 0.0f && sy >= 0.0f && sx < XRES && sy < YRES &&
		std::abs(-dvx + dvx * count) <= std::abs(parts[i].vx) &&
		std::abs(-dvy + dvy * count) <= std::abs(parts[i].vy)
	)
	{
		auto r = sim->pmap[(int)(sy + 0.5f)][(int)(sx + 0.5f)];
		if (r)
		{
			if (
				TYP(r) != parts[i].type && TYP(r) != PT_PRTI && TYP(r) != PT_PRTO && TYP(r) != PT_TRUS &&
				(elements[TYP(r)].Properties & TYPE_PART || elements[TYP(r)].Properties & TYPE_SOLID)
			)
			{
				parts[i].vx = parts[i].vy = 0.0f;
				return true;
			}
		}

		sx += dvx;
		sy += dvy;
		count++;
	}

	return false;
}

void Vehicle::GetPlayerCommand(Simulation *sim, Particle *parts, int i, Command &cmd, Command &cmd2)
{
	int command = parts[i].tmp2 == 1 ? sim->player.comm : sim->player2.comm;

	if ((command & 0x3) == 0x03) // Left + Right
	{
		cmd = Command::LeftRight;
	}
	else if (command & 0x1) // Left
	{
		cmd = Command::Left;
	}
	else if (command & 0x2) // Right
	{
		cmd = Command::Right;
	}

	if (command & 0x4) // Up
	{
		cmd2 = Command::Up;
	}
	else if (command & 0x8) // Down
	{
		cmd2 = Command::Down;
	}
}

void Vehicle::GetTarget(Simulation *sim, Particle *parts, int &tarx, int &tary)
{
	if (sim->player.spwn)
	{
		tarx = sim->player.legs[0];
		tary = sim->player.legs[1];
	}
	else if (sim->player2.spwn)
	{
		tarx = sim->player2.legs[0];
		tary = sim->player2.legs[1];
	}
	else if (sim->vehicle_p1 >= 0)
	{
		tarx = parts[sim->vehicle_p1].x;
		tary = parts[sim->vehicle_p1].y;
	}
	else if (sim->vehicle_p2 >= 0)
	{
		tarx = parts[sim->vehicle_p2].x;
		tary = parts[sim->vehicle_p2].y;
	}
}

void Vehicle::ExitVehicle(Simulation *sim, Particle *parts, int i, int x, int y)
{
	// Flag that vehicle has no stkm
	// Make sure new spawn location isn't too close or STKM will instantly re-enter
	int exitX = x - MIN_STKM_DISTANCE_TO_ENTER - 5;
	if (exitX < 0)
	{
		exitX = x + MIN_STKM_DISTANCE_TO_ENTER + 5;
	}

	if (parts[i].tmp2 == 1)
	{
		sim->vehicle_p1 = -1;
		parts[i].tmp2 = 0;
		sim->player.stkmID = sim->create_part(-1, exitX, std::max(y - 5, 0), PT_STKM);
		sim->player.elem = parts[i].ctype;
		sim->player.rocketBoots = parts[i].tmp == 1;
	}
	else if (parts[i].tmp2 == 2)
	{
		sim->vehicle_p2 = -1;
		parts[i].tmp2 = 0;
		sim->player2.stkmID = sim->create_part(-1, exitX, std::max(y - 5, 0), PT_STKM2);
		sim->player2.elem = parts[i].ctype;
		sim->player2.rocketBoots = parts[i].tmp == 1;
	}
}

void Vehicle::UpdateVehicle(Simulation *sim, Particle *parts, int i, const Vehicle &v, float ovx, float ovy)
{
	// Limit max speed
	float speed = std::hypot(ovx, ovy);
	if (speed > v.maxSpeed)
	{
		parts[i].vx = ovx * v.maxSpeed / speed;
		parts[i].vy = ovy * v.maxSpeed / speed;
	}

	// Run over STKM and FIGH if fast enough
	if (speed > v.runoverSpeed)
	{
		if (sim->player.spwn)
		{
			int xdiff = std::abs(sim->player.legs[0] - parts[i].x);
			int ydiff = std::abs(sim->player.legs[1] - parts[i].y);
			if (xdiff + ydiff < v.width / 2)
			{
				sim->kill_part(sim->player.stkmID);
			}
		}
		if (sim->player2.spwn)
		{
			int xdiff = std::abs(sim->player2.legs[0] - parts[i].x);
			int ydiff = std::abs(sim->player2.legs[1] - parts[i].y);
			if (xdiff + ydiff < v.width / 2)
			{
				sim->kill_part(sim->player2.stkmID);
			}
		}
		for (auto j = 0; j < MAX_FIGHTERS; j++)
		{
			if (sim->fighters[j].spwn)
			{
				int xdiff = std::abs(sim->fighters[j].legs[0] - parts[i].x);
				int ydiff = std::abs(sim->fighters[j].legs[1] - parts[i].y);
				if (xdiff + ydiff < v.width / 2)
				{
					sim->kill_part(sim->fighters[j].stkmID);
				}
			}
		}
	}

	// Check for nearby STKM and FIGH
	if (parts[i].tmp2 == 0 && sim->player.spwn && sim->vehicle_p1 < 0 && CheckSTKM(sim, parts, i, &sim->player))
	{
		parts[i].tmp2 = 1;
		sim->vehicle_p1 = i;
	}
	if (parts[i].tmp2 == 0 && sim->player2.spwn && sim->vehicle_p2 < 0 && CheckSTKM(sim, parts, i, &sim->player2))
	{
		parts[i].tmp2 = 2;
		sim->vehicle_p2 = i;
	}
	if (parts[i].tmp2 == 0)
	{
		for (auto j = 0; j < MAX_FIGHTERS; j++)
		{
			if (sim->fighters[j].spwn && CheckSTKM(sim, parts, i, &sim->fighters[j]))
			{
				parts[i].tmp2 = 3;
				break;
			}
		}
	}
}

bool Vehicle::CheckSTKM(Simulation *sim, Particle *parts, int i, playerst *data)
{
	int xdiff = std::abs(data->legs[0] - parts[i].x);
	int ydiff = std::abs(data->legs[1] - parts[i].y);
	if (xdiff + ydiff < MIN_STKM_DISTANCE_TO_ENTER)
	{
		if (parts[i].type != PT_HRSE)
		{
			if (data->rocketBoots) // Cyberthruster
			{
				parts[i].tmp = 1;
			}
			else if (data->elem == PT_FIRE) // Flamethrower
			{
				parts[i].tmp = 2;
			}
			else if (data->elem == PT_BOMB) // Bomb
			{
				parts[i].tmp = 3;
			}
		}
		else if (data->rocketBoots)
		{
			parts[i].tmp = 1;
		}
		parts[i].ctype = data->elem;
		sim->kill_part(data->stkmID);
		return true;
	}

	return false;
}

int Vehicle::CreatePart(Simulation *sim, Particle *parts, int i, Vec2<int> pos, int type, float theta)
{
	if (parts[i].tmp4)
	{
		pos.X = -pos.X; // Car going other way
	}
	Rotate(pos, theta);
	pos.X += parts[i].x, pos.Y += parts[i].y;
	return sim->create_part(-1, pos.X, pos.Y, type);
}

void Vehicle::DrawVehicle(pixel_rgba const *data, Rect<int> rect, Renderer *ren, float temp, int rider, bool flip, float rotation)
{
	RendererStats stats = ren->GetStats();
	bool heatMode = ren->GetColorMode() == COLOUR_HEAT;
	RGB heatColor = ren->heatTableAt(int((temp - stats.hdispLimitMin) / (stats.hdispLimitMax - stats.hdispLimitMin) * 1024));

	auto origin = rect.pos;
	for (auto pos : rect)
	{
		RGBA color = RGBA::Unpack(data[(pos.X - origin.X) + (pos.Y - origin.Y) * rect.size.X]);
		if (heatMode)
		{
			color = heatColor.WithAlpha(color.Alpha);
		}

		pos -= rect.size / 2;
		if (flip)
		{
			pos.X = -(pos.X - origin.X) + origin.X;
		}
		RotateAround(pos, origin, rotation);
		if (pos.X >= XRES || pos.Y >= YRES)
		{
			continue;
		}

		ren->BlendPixel(pos, color);

		// Blend if occupied
		if (rider == 1) // Player 1
		{
			ren->BlendPixel(pos, RGBA(255, 224, 160, 30));
		}
		else if (rider == 2) // Player 2
		{
			ren->BlendPixel(pos, RGBA(100, 100, 255, 30));
		}
		else if (rider > 2) // FIGH
		{
			ren->BlendPixel(pos, RGBA(255, 50, 0, 30));
		}
	}
}

void Vehicle::RecalcVehicles(Simulation *sim)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	vehicles.clear();

	for (auto i = 0; i <= sim->parts.active; i++)
	{
		if (elements[sim->parts[i].type].Properties & PROP_VEHICLE)
		{
			vehicles.push_back(i);
		}
	}
}
