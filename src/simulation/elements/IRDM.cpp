#include "simulation/ElementCommon.h"
#include "BALI.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_IRDM()
{
	Identifier = "DEFAULT_PT_IRDM";
	Name = "IRDM";
	Colour = 0xB1C7C1_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 0;

	Weight = 100;

	HeatConduct = 251;
	Description = "Iridium. Produces fire when sparked.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_HOT_GLOW | PROP_SPARKSETTLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 2466.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * life: Used by SPRK
	 * tmp:  oxygen timer
	 * tmp2: color cycle
	 */

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = sim->pmap[y + ry][x + rx];
				if (!r)
				{
					continue;
				}
				auto rt = TYP(r);

				if (parts[ID(r)].temp > 2273.15f && rt == PT_LAVA && parts[ID(r)].ctype == PT_SALT && sim->rng.chance(1, 500)) // Corroded by hot molten SALT
				{
					sim->kill_part(i);
					return 1;
				}
				else if (rt == PT_O2 && parts[i].temp > 1273.15f) // Oxygen makes it glow > 1000C
				{
					parts[i].tmp++;
					if (parts[i].tmp > 300)
					{
						parts[i].tmp = 300;
					}
				}

				// SPRK logic is in SPRK.cpp
			}
		}
	}

	if (parts[i].tmp)
	{
		if (parts[i].tmp2 < 250)
		{
			parts[i].tmp2 = 250;
		}
		parts[i].tmp2 += 2;
		if (parts[i].tmp2 > 700)
		{
			parts[i].tmp2 = 250;
		}

		parts[i].tmp--;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->tmp)
	{
		*pixel_mode |= FIRE_ADD;

		Element_BALI_findBoundary(cpart->tmp2, *firer, *fireg, *fireb, *cola);

		*cola = 255;
		*firea = 255;
	}

	return 0;
}
