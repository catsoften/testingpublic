#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_STAR()
{
	Identifier = "DEFAULT_PT_STAR";
	Name = "STAR";
	Colour = 0xFFFF00_rgb;
	MenuVisible = 1;
	MenuSection = SC_RANDOM;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.00f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 100;

	Weight = 200;

	HeatConduct = 10;
	Description = "Star, undergoes stellar evolution and provides light for planets. Incomplete.";

	Properties = TYPE_SOLID | PROP_LIFE_DEC;

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

	DefaultProperties.life = 1150;
	DefaultProperties.temp = R_TEMP + 4.0f + 273.15f;
	DefaultProperties.tmp = 9;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * tmp: star type
	 * 1. brown dwarf
	 * 2. red dwarf
	 * 3. yellow sun
	 * 	> life = fuel remaining
	 * 4. red giant
	 * 5. red supergiant
	 * 6. blue supergiant
	 * 7. white dwarf
	 * 8. neutron star
	 * 9. black hole
	 * 	> life = rotational velocity
	 */

	switch (parts[i].tmp)
	{
		case 9: // Black hole
			{
				sim->gravIn.mass[Vec2 { x, y } / CELL] += 0.2f;

				// TODO check bounds
				// TODO the constants like 1150 move somewhere
				// Rotational blackholes have slightly stronger gravity at poles
				float rotationalGravityBonus = 0.2f * parts[i].life / 1150.0f;
				sim->gravIn.mass[Vec2{ x - 1, y } / CELL] += rotationalGravityBonus;
				sim->gravIn.mass[Vec2{ x + 1, y } / CELL] += rotationalGravityBonus;

				// Kill nearby particles
				for (auto rx = -1; rx <= 1; rx++)
				{
					for (auto ry = -1; ry <= 1; ry++)
					{
						auto r = pmap[y + ry][x + rx];
						if (!r)
						{
							r = sim->photons[y + ry][x + rx];
						}
						if (!r || TYP(r) == PT_STAR)
						{
							continue;
						}
						sim->kill_part(ID(r));
					}
				}

				break;
			}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	// Is edge check
	bool isEdge = false;
	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = gfctx.sim->pmap[(int)(cpart->y + 0.5f) + ry][(int)(cpart->x + 0.5f) + rx];
				if (!r || TYP(r) != PT_STAR || gfctx.sim->parts[ID(r)].tmp != cpart->tmp)
				{
					isEdge = true;
					goto end;
				}
			}
		}
	}
	end:

	switch (cpart->tmp)
	{
		case 9: // Black hole
			{
				*colr = *colg = *colb = 0;

				// Outer blackhole glows
				if (isEdge)
				{
					*firer = 255;
					*fireg = 150;
					*firea = 120 - (gfctx.sim->currentTick % 30); // Flicker
					*firea *= (float)cpart->life / 1150.0f;

					*pixel_mode |= FIRE_ADD;

					// Border flicker
					if (gfctx.sim->currentTick % 3 == 0 && cpart->life > 500)
					{
						*colr = 255; *colg = 150;
						*pixel_mode |= PMODE_FLARE;
					}
				}
				break;
			}
	}

	return 0;
}
