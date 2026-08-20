#include "simulation/ElementCommon.h"
#include "THOR.h"

static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_THOR()
{
	Identifier = "DEFAULT_PT_THOR";
	Name = "THOR";
	Colour = 0x8F8276_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
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

	HeatConduct = 150;
	Description = "Thorium. Creates heat with neutrons, still radioactive when molten.";

	Properties = TYPE_SOLID | PROP_RADIOACTIVE | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1200.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &Element_THOR_update;
	Graphics = &graphics;

	DefaultProperties.tmp = 100;
}


int Element_THOR_update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties
	 * life - SPRK
	 * tmp  - Fuel left, 0 = depelted
	 * tmp2 - NEUT to emit
	 */

	// Warm to 37 C
	if (parts[i].temp < 37.0f + 273.15f && parts[i].tmp)
	{
		parts[i].temp += 0.002f;
	}

	// Depelted, slowly turn into RADN
	// Type check == PT_THOR since update is called in PT_FIRE for LAVA
	if (!parts[i].tmp && parts[i].type == PT_THOR && sim->rng.chance(1, 200))
	{
		sim->part_change_type(i, x, y, PT_RADN);
		return 1;
	}

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				if (parts[i].tmp2 > 0 && sim->rng.chance(1, 8))
				{
					parts[i].tmp2--;
					auto j = sim->create_part(-3, x + rx, y + ry, PT_NEUT);
					if (j >= 0)
					{
						parts[j].temp = parts[i].temp;
					}
				}

				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					r = sim->photons[y + ry][x + rx];
				}
				if (!r)
				{
					continue;
				}
				auto rt = TYP(r);

				if (rt == PT_PROT) // High velocity PROT -> PLUT
				{
					if (std::hypot(parts[ID(r)].vx, parts[ID(r)].vy) > 5.0f)
					{
						sim->part_change_type(i, x, y, PT_PLUT);
						return 1;
					}
				}
				else if (rt == PT_NEUT && parts[i].tmp && sim->rng.chance(1, 15)) // NEUT: 1/15 chance to create more NEUT
				{
					parts[i].temp += 130.0f;
					parts[i].tmp--;
					parts[i].tmp2 += 2;
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*firer = 200;
	*fireg = *fireb = 255;
	*firea = 20 * std::min(100, cpart->tmp) / 100;
	*pixel_mode |= FIRE_ADD;

	return 0;
}
