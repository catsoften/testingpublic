#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_SICN()
{
	Identifier = "DEFAULT_PT_SICN";
	Name = "SLCN";
	Colour = 0x628099_rgb;
	MenuVisible = 1;
	MenuSection = SC_ELEC;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Silicon. Temporarily turns on powered elements when sparked, cannot conduct spark on diagonals.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1414.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

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

				// Doping
				if (
					rt == PT_BSMH ||
					(rt == PT_LAVA && parts[ID(r)].ctype == PT_BSMH) ||
					rt == PT_PHSP ||
					(rt == PT_LAVA && parts[ID(r)].ctype == PT_PHSP)
				) // N-type
				{
					if (parts[i].temp > 400.0f + 273.15f && sim->rng.chance(1, 1000))
					{
						sim->part_change_type(i, x, y, PT_NSCN);
						return 1;
					}
				}
				else if (rt == PT_ALMN || (rt == PT_LAVA && parts[ID(r)].ctype == PT_ALMN) || rt == PT_ION) // P-type
				{
					if (parts[i].temp > 400.0f + 273.15f && sim->rng.chance(1, 1000))
					{
						sim->part_change_type(i, x, y, PT_PSCN);
						return 1;
					}
				}

				// Deactivate powered materials (SPRK(SICN) activates powered)
				if ((rt == PT_SWCH || elements[rt].MenuSection == SC_POWERED) && parts[i].life && parts[ID(r)].life)
				{
					sim->flood_prop(x + rx, y + ry, AccessProperty{ FIELD_LIFE, 0 });

					// LCRY needs tmp & tmp2 for some reason
					if (TYP(r) == PT_LCRY)
					{
						sim->flood_prop(x + rx, y + ry, AccessProperty{ FIELD_TMP, 0 });
						sim->flood_prop(x + rx, y + ry, AccessProperty{ FIELD_TMP2, 0 });
					}
				}
			}
		}
	}

	return 0;
}
