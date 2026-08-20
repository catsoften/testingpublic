#include "simulation/ElementCommon.h"

#include "common/Plane.h"
#include "Format.h"
#include "graphics/Pixel.h"
#include "simulation/Vehicle.h"
#include "ultimata/ElementUtils.h"
#include "ultimata/Transform.h"

#include "TANK_png.h"

std::unique_ptr<PlaneAdapter<std::vector<pixel_rgba>>> TANKImage = format::PixelsFromPNG(TANK_png.AsCharSpan());

constexpr Vehicle VEHICLE_TANK = Vehicle{
	35,   // width
	18,   // height
	1.5f, // acceleration
	0.5f, // flyAcceleration
	5.0f, // maxSpeed
	4.5f, // collisionSpeed
	1.1f, // runoverSpeed
	0.1f, // rotationSpeed
};

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void changeType(ELEMENT_CHANGETYPE_FUNC_ARGS);

void Element::Element_TANK()
{
	Identifier = "DEFAULT_PT_TANK";
	Name = "TANK";
	Colour = 0x52482E_rgb;
	MenuVisible = 1;
	MenuSection = SC_RANDOM;
	Enabled = 1;

	Advection = 0.01f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.15f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 20;
	Description = "Soviet Tank. STKM can ride, comes with several upgrades. Hold down to shoot.";

	Properties = TYPE_PART | PROP_NOCTYPEDRAW | PROP_VEHICLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;

	ChangeType = &changeType;

	DefaultProperties.life = 600;
}

using Cmd = Vehicle::Command;

static int update(UPDATE_FUNC_ARGS)
{
	// NOTE: TANK UPDATES TWICE PER FRAME
	/**
	 * Properties:
	 * vx, vy (velocity)
	 * ctype = element of STKM when it entered
	 * tmp2 = which STKM controls it (1 = STKM, 2 = STK2, 3 = AI car)
	 * tmp = rocket or flamethrower (0 none, 1 plasma, 2 flamethrower, 3 bomb)
	 * life = HP
	 * tmp3 = rotation
	 * tmp4 = direction of travel (left or right)
	 *
	 * If touched by a FIGH the FIGH will attempt to use the tank to run down
	 * the STKM and STK2
	 */

	float ovx = parts[i].vx, ovy = parts[i].vy;
	bool hasCollision;
	Vehicle::InitialCollision(sim, parts, i, VEHICLE_TANK, hasCollision);

	// Collision damage
	if (hasCollision && std::hypot(ovx, ovy) > VEHICLE_TANK.collisionSpeed)
	{
		parts[i].life -= sim->rng.between(0, 25);
	}

	// Heat damage
	if (parts[i].temp > 273.15f + 200.0f)
	{
		parts[i].life--;
	}
	if (parts[i].temp > 3000.0f)
	{
		parts[i].life = 0;
	}

	// Self destruction, leave a randomized KV2 shaped pile of powder
	if (parts[i].life <= 0)
	{
		sim->kill_part(i);
		return 0;
	}

	// If life <= 150 spawn sparks (EMBR)
	if (parts[i].life <= 150)
	{
		if (sim->rng.chance(1, 50))
		{
			Vehicle::CreatePart(sim, parts, i, { (int)(VEHICLE_TANK.width * 0.4f), (int)(-VEHICLE_TANK.height * 0.5f) }, PT_EMBR, ToFloat(parts[i].tmp3));
		}
		if (sim->rng.chance(1, 50))
		{
			Vehicle::CreatePart(sim, parts, i, { (int)(-VEHICLE_TANK.width * 0.4f), (int)(-VEHICLE_TANK.height * 0.5f) }, PT_EMBR, ToFloat(parts[i].tmp3));
		}
		if (sim->rng.chance(1, 50))
		{
			Vehicle::CreatePart(sim, parts, i, { 0, (int)(-VEHICLE_TANK.height * 0.5f) }, PT_EMBR, ToFloat(parts[i].tmp3));
		}
	}

	// If life <= 300 spawn fire damage
	if (parts[i].life <= 300 && sim->rng.chance(1, 30))
	{
		Vehicle::CreatePart(sim, parts, i, { (int)(-VEHICLE_TANK.width * 0.4f), (int)(-VEHICLE_TANK.height * 0.5f) }, PT_FIRE, ToFloat(parts[i].tmp3));
	}

	// Player controls
	Cmd cmd = Cmd::None, cmd2 = Cmd::None;
	if (parts[i].tmp2 == 1 || parts[i].tmp2 == 2)
	{
		Vehicle::GetPlayerCommand(sim, parts, i, cmd, cmd2);
	}
	else if (parts[i].tmp2 >= 3) // Fighter AI
	{
		// Get target
		int tarx = -1, tary = -1;
		Vehicle::GetTarget(sim, parts, tarx, tary);
		if (tarx > -0)
		{
			if (parts[i].tmp == 2 || parts[i].tmp == 3) // Flamethrower / bomb weapon
			{
				cmd2 = Cmd::Down;
				parts[i].tmp4 = tarx > parts[i].x;
			}
			else if (parts[i].tmp == 1) // Rocket
			{
				cmd = tarx > parts[i].x ? Cmd::Right : Cmd::Left;
				cmd2 = tary < parts[i].y ? Cmd::Down : Cmd::None;
			}
			else if (hasCollision) // Run 'em over
			{
				cmd = tarx > parts[i].x ? Cmd::Right : Cmd::Left;
			}
		}
	}

	// Do controls
	if (cmd != Cmd::None || cmd2 != Cmd::None)
	{
		Vec2<float> vel{ parts[i].vx, parts[i].vy };
		if (hasCollision || parts[i].tmp == 1) // Accelerating only can be done on ground or if rocket
		{
			Vec2<float> acc{ hasCollision ? -VEHICLE_TANK.acceleration : -VEHICLE_TANK.flyAcceleration / 8.0f, 0.0f };
			if (cmd == Cmd::Left) // Left
			{
				Rotate(acc, ToFloat(parts[i].tmp3));
				vel += acc;
				parts[i].tmp4 = 0; // Set face direction
				parts[i].y -= 0.5f;
			}
			else if (cmd == Cmd::Right) // Right
			{
				acc.X *= -1.0f;
				Rotate(acc, ToFloat(parts[i].tmp3));
				vel += acc;
				parts[i].tmp4 = 1; // Set face direction
				parts[i].y -= 0.5f;
			}
		}

		if (cmd2 == Cmd::Up) // Exit (up)
		{
			parts[i].vx = vel.X, parts[i].vy = vel.Y;
			Vehicle::ExitVehicle(sim, parts, i, x, y);
			return 0;
		}
		else if (cmd2 == Cmd::Down) // Fly or shoot (down)
		{
			if (parts[i].tmp == 1 || parts[i].tmp == 2) // Flamethrower
			{
				auto j = Vehicle::CreatePart(sim, parts, i, { (int)(-VEHICLE_TANK.width * 0.4f), (int)(-VEHICLE_TANK.height * 0.5f) }, PT_BCOL, ToFloat(parts[i].tmp3));
				if (j >= 0)
				{
					parts[j].life = sim->rng.between(0, 100) + 50;
					if (parts[i].tmp == 1) // Plasma
					{
						parts[j].temp = 9000.0f;
					}
					Vec2<float> velj{ parts[i].tmp4 ? 15.0f : -15.0f, -(float)(sim->rng.between(0, 3) + 3) };
					Rotate(velj, ToFloat(parts[i].tmp3));
					parts[j].vx = velj.X, parts[j].vy = velj.Y;
				}
			}
			else if (parts[i].tmp == 0 || parts[i].tmp == 3) // BOMB
			{
				if (sim->frameCount % 50 == 0)
				{
					auto j1 = Vehicle::CreatePart(sim, parts, i, { (int)(-VEHICLE_TANK.width * 0.4f), (int)(-VEHICLE_TANK.height * 0.5f) }, parts[i].tmp == 3 ? PT_BOMB : PT_FSEP, ToFloat(parts[i].tmp3));
					if (j1 >= 0)
					{
						parts[j1].life = 20;
						parts[j1].temp = 9999.0f;
						Vec2<float> velj(parts[i].tmp4 ? 550.0f : -550.0f, 0.0f);
						Rotate(velj, ToFloat(parts[i].tmp3));
						parts[j1].vx = velj.X, parts[j1].vy = velj.Y;
					}
				}

				// Turret flame when firing (Not with BOMB)
				if (parts[i].tmp != 3)
				{
					auto j2 = Vehicle::CreatePart(sim, parts, i, { (int)(-VEHICLE_TANK.width * 0.4f + 1.0f), (int)(-VEHICLE_TANK.height * 0.5f) }, PT_BANG, ToFloat(parts[i].tmp3));
					parts[j2].temp = 1500.0f;
				}
			}
		}
		parts[i].vx = vel.X, parts[i].vy = vel.Y;
	}

	Vehicle::UpdateVehicle(sim, parts, i, VEHICLE_TANK, ovx, ovy);
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	Vehicle::DrawVehicle(TANKImage->data(), RectSized(Vec2{ (int)cpart->x, (int)cpart->y }, TANKImage->Size()), gfctx.renderer, cpart->temp, cpart->tmp2, cpart->tmp4, ToFloat(cpart->tmp3));

	*colr = *colg = *colb = *cola = 0;
	*pixel_mode = PMODE_NONE;

	return 0;
}

static void changeType(ELEMENT_CHANGETYPE_FUNC_ARGS)
{
	auto &parts = sim->parts;

	Vehicle::ExitVehicle(sim, parts, i, x, y);

	if (to == PT_NONE && parts[i].life <= 0)
	{
		// Die into a tank shaped pile of BRMT and BREC
		pixel_rgba const *data = TANKImage->data();
		auto rect = RectSized(Vec2{ 0, 0 }, TANKImage->Size());
		for (auto pos : rect)
		{
			RGBA color = RGBA::Unpack(data[pos.X + pos.Y * rect.size.X]);
			pos -= rect.size / 2;
			if (color.Alpha)
			{
				int j, t;
				t = sim->rng.between(0, 100);
				if (t < 70)
				{
					j = Vehicle::CreatePart(sim, parts, i, pos, PT_BRMT, ToFloat(parts[i].tmp3));
				}
				else
				{
					j = Vehicle::CreatePart(sim, parts, i, pos, PT_BREC, ToFloat(parts[i].tmp3));
				}

				if (j >= 0)
				{
					parts[j].dcolour = 0xCF000000 | color.Pack();
					parts[j].vx = parts[i].vx;
					parts[j].vy = parts[i].vy;
					parts[j].temp = parts[i].temp;
				}
			}
		}
	}
}
