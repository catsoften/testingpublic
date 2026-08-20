#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_BAKS()
{
	Identifier = "DEFAULT_PT_BAKS";
	Name = "BAKS";
	Colour = 0xF0F0F0_rgb;
	MenuVisible = 1;
	MenuSection = SC_POWDERS;
	Enabled = 1;

	Advection = 0.3f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 60;

	HeatConduct = 150;
	Description = "Baking Soda.";

	Properties = TYPE_PART;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	// Decompose
	if (parts[i].temp > 80.0f + 273.15f)
	{
		sim->part_change_type(i, x, y, sim->rng.chance(1, 2) ? PT_CO2 : PT_WATR);
		return 1;
	}

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

				if (rt == PT_FIRE || rt == PT_PLSM)
				{
					sim->kill_part(ID(r));
				}
				else if (rt == PT_ACID || rt == PT_CAUS)
				{
					if (sim->rng.chance(1, 100))
					{
						sim->part_change_type(i, x, y, PT_CO2);
						sim->part_change_type(ID(r), x + rx, y + ry, PT_CO2);
						return 1;
					}
				}
				else if (rt == PT_BCTR)
				{
					parts[ID(r)].tmp2 = 1;
				}
				else
				{
					// Clear dcolor
					if (sim->rng.chance(1, 5000))
					{
						parts[ID(r)].dcolour = 0;
					}
				}
			}
		}
	}

	return 0;
}
