#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_MILK()
{
	Identifier = "DEFAULT_PT_MILK";
	Name = "MILK";
	Colour = 0xFFFFFF_rgb;
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 30;

	HeatConduct = 29;
	Description = "Milk. Heals STKM, goes bad over time.";

	Properties = TYPE_LIQUID | PROP_CONDUCTS | PROP_NEUTPASS | PROP_EDIBLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = 273.15f;
	LowTemperatureTransition = PT_ICEI;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	FoodValue = 5;

	Update = &update;

	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties
	 * - tmp:  Has it been pasturized?
	 * - life: How long it's been around (increases)
	 */

	// Evaporate
	if (parts[i].temp > 100.0f + 273.15f)
	{
		sim->part_change_type(i, x, y, sim->rng.chance(1, 3) ? PT_CLCM : PT_WTRV);
		return 1;
	}

	// Pasturize
	if (parts[i].temp > 63.0f + 273.15f && sim->rng.chance(1, 2000))
	{
		parts[i].life = 0;
		parts[i].tmp = 1;
	}
	if (parts[i].temp > 72.0f + 273.15f && sim->rng.chance(1, 100))
	{
		parts[i].life = 0;
		parts[i].tmp = 1;
	}

	// Cold milk doesn't spoil as fast
	if (parts[i].temp < 5.0f + 273.15f)
	{
		parts[i].life++;
	}
	else
	{
		parts[i].life += 5;
	}

	// Spoil
	if (parts[i].life > 18000 && sim->rng.chance(1, 7000))
	{
		sim->kill_part(i);
		sim->create_part(-1, x, y, PT_BCTR);
		return 1;
	}

	// Touching dirty stuff spoils it
	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			auto r = pmap[y + ry][x + rx];
			if (!r)
			{
				continue;
			}

			if (TYP(r) == PT_BCOL || TYP(r) == PT_BCTR || TYP(r) == PT_COAL || TYP(r) == PT_MUD || TYP(r) == PT_SOIL)
			{
				parts[i].life += 10;
			}
		}
	}

	return 0;
}
