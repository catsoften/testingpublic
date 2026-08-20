#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_TMRM()
{
	Identifier = "DEFAULT_PT_TMRM";
	Name = "TMRM";
	Colour = 0xB35539_rgb;
	MenuVisible = 1;
	MenuSection = SC_ELECTROMAG;
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
	Hardness = 10;

	Weight = 100;

	HeatConduct = 150;
	Description = "Thermium. Conductor, only melts under exposure to lightning.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 4000.0f + 273.15f;
	HighTemperatureTransition = NT;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
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

				if (TYP(r) == PT_LIGH && parts[ID(r)].temp > 5000.0 + 273.15f)
				{
					sim->part_change_type(i, x, y, PT_LAVA);
					parts[i].temp += 1000.0f;
					parts[i].ctype = PT_TMRM;
					return 1;
				}
			}
		}
	}

	return 0;
}
