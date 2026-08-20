#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_FIBR()
{
	Identifier = "DEFAULT_PT_FIBR";
	Name = "FIBR";
	Colour = 0xA9C8AB_rgb;
	MenuVisible = 1;
	MenuSection = SC_ELEC;
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
	Meltable = 0;
	Hardness = 0;

	Weight = 100;

	HeatConduct = 150;
	Description = "Fiber optic cable. Moves photons, set tmp for transfer speed.";

	Properties = TYPE_SOLID | PROP_PHOTPASS | PROP_HOT_GLOW | PROP_SPARKSETTLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 5.0f;
	HighPressureTransition = PT_BGLA;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;

	DefaultProperties.tmp = 30;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties
	 * - tmp: Speed
	 *
	 * Increases life of PHOT inside
	 */

	// Melt into glass
	if (parts[i].temp > 1973.15f)
	{
		sim->part_change_type(i, x, y, PT_LAVA);
		parts[i].ctype = PT_GLAS;
		return 1;
	}

	// Limit max / min speeds
	if (parts[i].tmp < 0)
	{
		parts[i].tmp = 0;
	}
	else if (parts[i].tmp > 50)
	{
		parts[i].tmp = 50;
	}

	// Keep photons alive
	auto r = sim->photons[y][x];
	if (TYP(r) == PT_PHOT)
	{
		parts[ID(r)].life++;
	}

	return 0;
}
