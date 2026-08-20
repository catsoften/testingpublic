#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_NPLM()
{
	Identifier = "DEFAULT_PT_NPLM";
	Name = "NPLM";
	Colour = 0xB00000_rgb; // Get it BOOOO // No I don't get it
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
	Enabled = 1;

	Advection = 0.5f;
	AirDrag = 0.02f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.97f;
	Collision = 0.0f;
	Gravity = 0.2f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 24;

	HeatConduct = 88;
	Description = "Napalm. Ultra-long burning explosive.";

	Properties = TYPE_LIQUID | PROP_LIFE_KILL;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 3726.85f + 273.15f;
	HighTemperatureTransition = PT_H2;

	Update = &update;

	DefaultProperties.life = 1200;
	DefaultProperties.temp = R_TEMP + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	if (parts[i].tmp > 0 && sim->frameCount % 10 == 0)
	{
		auto j = sim->create_part(-1, x - 1, y - 1, PT_FIRE);
		if (j >= 0)
		{
			parts[j].life = sim->rng.between(1, 200);
		}

		for (auto k = -1; k <= 1; k += 2)
		{
			j = sim->create_part(-1, x + k, y + k, PT_BCOL);
			if (j >= 0)
			{
				parts[j].life = sim->rng.between(0, 400);
				parts[j].vx = sim->rng.between(-15, 15);
				parts[j].vy = sim->rng.between(-15, 15);
			}
		}
	}

	// Fix burning forever
	if (sim->rng.chance(1, 100))
	{
		parts[i].life--;
	}

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r || TYP(r) == PT_NPLM)
				{
					continue;
				}
				auto rt = TYP(r);

				if (elements[rt].Properties & TYPE_PART || elements[rt].Properties & TYPE_SOLID)
				{
					parts[i].vx = parts[i].vy = 0.0f;
				}

				if (rt == PT_FIRE || rt == PT_PLSM || parts[ID(r)].type == PT_LAVA || parts[i].temp >= 1273.15f)
				{
					parts[ID(r)].life++;
					parts[ID(r)].vx *= 2;
					parts[ID(r)].vy *= 2;
					parts[i].life--;
					parts[i].temp += 50.0f;
					parts[i].tmp = 1;
				}
			}
		}
	}

	return 0;
}
