#include "simulation/ElementCommon.h"

#include "ultimata/ElementUtils.h"

constexpr int SPOKES = 16;

static int update(UPDATE_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_SPDR()
{
	Identifier = "DEFAULT_PT_SPDR";
	Name = "SPDR";
	Colour = 0x444444_rgb;
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
	Enabled = 1;

	Advection = 0.7f;
	AirDrag = 0.02f * CFDS;
	AirLoss = 0.96f;
	Loss = 0.80f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 10;
	Explosive = 0;
	Meltable = 0;
	Hardness = 30;

	Weight = 32;

	HeatConduct = 70;
	Description = "Spider. Spins webs and eats captured creatures.";

	Properties = TYPE_PART;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 5.0f;
	HighPressureTransition = PT_BCTR;
	LowTemperature = -5.0f + 273.15f;
	LowTemperatureTransition = PT_SNOW;
	HighTemperature = 60.0f + 273.15f;
	HighTemperatureTransition = PT_DUST;

	Update = &update;

	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * tmp: dir (0 = left / up, 1 = right / down)
	 * tmp2: web stage
	 * 	0 = looking to construct
	 *  1 = first line and second line
	 *  2 = spokes and rings
	 * 	3 = done
	 * tmp3, tmp4: Target location to move
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	auto notEmpty = [&pmap](int x, int y)
	{
		return
			pmap[y - 1][x - 1] || pmap[y - 1][x] ||
			pmap[y - 1][x + 1] || pmap[y + 1][x] ||
			pmap[y + 1][x - 1] || pmap[y][x - 1] ||
			pmap[y + 1][x + 1] || pmap[y][x + 1];
	};

	bool touching = false;

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					continue;
				}
				auto rt = TYP(r);

				if (elements[rt].Properties & TYPE_SOLID)
				{
					touching = true;
				}

				if (rt == PT_WEB && parts[ID(r)].ctype) // Eat food on web
				{
					sim->kill_part(ID(r));
				}
				else if (rt == PT_WEB && parts[i].tmp2 == 3 && sim->rng.chance(1, 8)) // Randomly move along webs if its tmp2 is 3
				{
					parts[i].vx = rx;
					parts[i].vy += ry;
					return 0;
				}
			}
		}
	}

	if (touching) // Dont move if touching a solid
	{
		parts[i].vx = parts[i].vy = 0.0f;
	}
	else // Reset target location if "falls off" web
	{
		parts[i].tmp2 = 0;
		parts[i].tmp3 = parts[i].tmp4 = 0;
	}

	// At target, reset
	if (std::abs(x - parts[i].tmp3) < 2 && std::abs(y - parts[i].tmp4) < 2)
	{
		parts[i].tmp3 = parts[i].tmp4 = 0;
	}

	if (parts[i].tmp3 && parts[i].tmp4) // Move to target
	{
		parts[i].vx += (parts[i].tmp3 - x) / 200.0f;
		parts[i].vy += (parts[i].tmp4 - y) / 200.0f;
	}
	else if (parts[i].tmp2 == 0) // Move along solids
	{
		if (parts[i].tmp) // Move down and right, randomize which one to avoid loops
		{
			if (sim->rng.chance(1, 2) && !pmap[y][x + 1] && notEmpty(x + 1, y))
			{
				parts[i].x++;
			}
			else if (!pmap[y + 1][x] && notEmpty(x, y + 1))
			{
				parts[i].y++;
			}
			else
			{
				parts[i].tmp = 0; // Swap move dir
			}
		}
		else // Move left and up, randomize which one to avoid loops
		{
			if (sim->rng.chance(1, 2) && !pmap[y][x - 1] && notEmpty(x - 1, y))
			{
				parts[i].x--;
			}
			else if (!pmap[y - 1][x] && notEmpty(x, y - 1))
			{
				parts[i].y--;
			}
			else
			{
				parts[i].tmp = 1; // Swap move dir
			}
		}
	}

	// ------ WEB SPINING -------- //

	// Randomly spin a web in a direction
	// tmp2 = 0 means not started yet, randomly decide to start
	// tmp2 = 1 means started, make the 2nd line
	if ((parts[i].tmp2 == 1 && !parts[i].tmp3) || (parts[i].tmp2 == 0 && !parts[i].tmp3 && sim->rng.chance(1, 2000) && touching))
	{
		// Attempt to web 5 times
		for (auto k = 0; k < 5; k++)
		{
			float angle = sim->rng.uniform01() * 2.0f * std::numbers::pi_v<float>;
			auto pos = IntersectLine(sim, x, y, std::cos(angle), std::sin(angle), 5);

			if (pos.X == -1 && pos.Y == -1)
			{
				pos = IntersectLine(sim, x, y, -std::cos(angle), -std::sin(angle), 5);
			}
			if (pos.X != -1 && pos.Y != -1)
			{
				// Web must be a certain distance, 30 - 350px
				int dis = std::hypot(pos.X - x, pos.Y - y);
				if (dis > 30 && dis < 350)
				{
					sim->CreateLine(x, y, pos.X, pos.Y, PT_WEB);
					parts[i].tmp2++;
					parts[i].tmp3 = (x + pos.X) / 2;
					parts[i].tmp4 = (y + pos.Y) / 2;
					return 0;
				}
			}
		}
	}
	else if (parts[i].tmp2 == 2 && !parts[i].tmp3) // Make the spokes and rings
	{
		// Attempt spokes from a random starting point
		float start = sim->rng.uniform01() * 2.0f * std::numbers::pi_v<float>;
		bool web = false;

		for (auto k = 0; k < SPOKES; k++)
		{
			float angle = start + (float)k / SPOKES * 2.0f * std::numbers::pi_v<float>;
			auto pos1 = IntersectLine(sim, x, y, std::cos(angle), std::sin(angle), 6, PT_WEB);

			if (pos1.X != -1 && pos1.Y != -1)
			{
				// Web must be a certain distance. Spoke webs can be shorter between 3 - 350px long
				int dis = std::hypot(pos1.X - x, pos1.Y - y);
				if (dis > 3 && dis < 350)
				{
					sim->CreateLine(x, y, pos1.X, pos1.Y, PT_WEB);
					web = true;

					// Cheat on the rings since its very hard to implement
					for (auto ringDis = 0.4f; ringDis < 1.0f; ringDis += 0.2f)
					{
						auto pos3 = Vec2{ x, y } + (pos1 - Vec2{ x, y }) * ringDis;
						auto pos2 = IntersectLine(sim, pos3.X, pos3.Y, std::cos(angle - std::numbers::pi_v<float> / 2.0f), std::sin(angle - std::numbers::pi_v<float> / 2.0f), 6, PT_WEB);

						if (pos2.X != -1 && pos2.Y != -1)
						{
							int dis = std::hypot(pos2.X - pos3.X, pos2.Y - pos3.Y);
							if (dis > 3 && dis < 100)
							{
								sim->CreateLine(pos2.X, pos2.Y, pos3.X, pos3.Y, PT_WEB);
							}
						}
					}
				}
			}
		}

		// Advance to next stage
		if (web)
		{
			parts[i].tmp2++;
		}
	}

	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].tmp = sim->rng.chance(1, 2);
}
