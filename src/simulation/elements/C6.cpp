#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_C6()
{
	Identifier = "DEFAULT_PT_C6";
	Name = "C-6";
	Colour = 0xFF40B6_rgb;
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
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
	Hardness = 1;

	Weight = 100;

	HeatConduct = 250;
	Description = "Experimental explosive, more powerful than C4.";

	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;

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
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	for (auto rx = -2; rx <= 2; rx++)
	{
		for (auto ry = -2; ry <= 2; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					continue;
				}

				if ((TYP(r) != PT_C6 && parts[ID(r)].temp > 373.15f && elements[TYP(r)].HeatConduct && (TYP(r) != PT_HSWC || parts[ID(r)].life == 10)) ||
					TYP(r) == PT_PLSM || TYP(r) == PT_FIRE || TYP(r) == PT_DFLM)
				{
					if (sim->rng.chance(1, 6))
					{
						parts[i].life = sim->rng.between(50, 199);
						parts[ID(r)].temp += 1000.0f;
						sim->pv[y / CELL][x / CELL] += 7.5;
					}
				}
			}
		}
	}

	// We exploded :D
	if (parts[i].life)
	{
		// Scatter EMBR and burning coal everywhere
		for (int rx = -1; rx <= 1; rx++)
		{
			for (int ry = -1; ry <= 1; ry++)
			{
				if (rx || ry)
				{
					auto r = pmap[y + ry][x + rx];
					if (!r)
					{
						auto j = sim->create_part(-1, x + rx, y + ry, sim->rng.chance(1, 2) ? PT_EMBR : PT_BCOL);
						if (j >= 0)
						{
							parts[j].life = sim->rng.between(10, 100);
							parts[j].vx = 2 * rx;
							parts[j].vy = 2 * ry;
							parts[j].temp = parts[i].temp + 2000.0f;
							parts[j].dcolour = 0xFFFF0000;
						}
					}
				}
			}
		}

		sim->part_change_type(i, x, y, PT_FIRE);
	};

	return 0;
}
