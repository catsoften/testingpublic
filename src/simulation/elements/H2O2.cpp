#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_H2O2()
{
	Identifier = "DEFAULT_PT_H2O2";
	Name = "H2O2";
	Colour = 0x2738E6_rgb;
	MenuVisible = 1;
	MenuSection = SC_LIQUID;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.12f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 2;

	Flammable = 1000;
	Explosive = 100;
	Meltable = 0;
	Hardness = 20;

	Weight = 35;

	HeatConduct = 29;
	Description = "Hydrogen Peroxide. Powerful oxidizer, slowly decays into water and hydrogen.";

	Properties = TYPE_LIQUID | PROP_CONDUCTS | PROP_PHOTPASS | PROP_LIFE_DEC | PROP_NEUTPASS | PROP_DEADLY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = -0.4f + 273.15f;
	LowTemperatureTransition = PT_ICEI;
	HighTemperature = 150.05f + 273.15f;
	HighTemperatureTransition = PT_WTRV;

	Update = &update;

	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	if (sim->rng.chance(1, std::max(1, 30000 - 10 * (int)parts[i].temp)))
	{
		sim->part_change_type(i, x, y, sim->rng.chance(1, 2) ? PT_DSTW : PT_H2);
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

				if (rt == PT_VIRS || rt == PT_VRSG || rt == PT_VRSS || rt == PT_BCTR) // Break down VIRS and BCTR
				{
					sim->kill_part(ID(r));
					continue;
				}
				else if (rt == PT_SUFR)
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_ACID);
					sim->part_change_type(i, x, y, PT_ACID);
					return 1;
				}
				else if (elements[rt].Hardness && sim->rng.between(1, 100) > elements[rt].Hardness &&
					sim->rng.chance(1, 500) && !(elements[rt].Properties & TYPE_LIQUID))
				{
					sim->kill_part(ID(r));
					sim->kill_part(i);
					return 1;
				}
			}
		}
	}

	return 0;
}
