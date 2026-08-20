#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_BRKN()
{
	Identifier = "DEFAULT_PT_BRKN";
	Name = "BRKN";
	Colour = 0x705060_rgb;
	MenuVisible = 0;
	MenuSection = SC_SPECIAL;
	Enabled = 1;

	Advection = 0.2f;
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
	Hardness = 2;

	Weight = 90;

	HeatConduct = 211;
	Description = "A generic broken solid.";

	Properties = TYPE_PART | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	if (parts[i].ctype < 0 || parts[i].ctype >= PT_NUM || !elements[parts[i].ctype].Enabled || parts[i].ctype == PT_BRKN)
	{
		parts[i].ctype = 0;
	}

	static bool allowRecursion = true;

	parts[i].tmp = parts[i].ctype;

	bool flammable = false;

	if (parts[i].ctype)
	{
		if (!(elements[parts[i].ctype].Properties & PROP_CONDUCTS))
		{
			parts[i].life = 4; // Prevent spark conducting if not actually conductable
		}
		if (allowRecursion && elements[parts[i].ctype].Update)
		{
			allowRecursion = false;
			auto result = elements[parts[i].ctype].Update(UPDATE_FUNC_SUBCALL_ARGS);
			allowRecursion = true;
			if (result)
			{
				return result;
			}
		}
		if (elements[parts[i].ctype].HighTemperatureTransition && parts[i].temp > elements[parts[i].ctype].HighTemperature)
		{
			sim->part_change_type(i, x, y, elements[parts[i].ctype].HighTemperatureTransition);
			return 1;
		}
		flammable = elements[parts[i].ctype].Flammable;
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
				bool isWater = rt == PT_IOSL || rt == PT_WATR || rt == PT_DSTW || rt == PT_SLTW || rt == PT_CBNW || rt == PT_SWTR || rt == PT_WTRV;

				if (flammable && (rt == PT_FIRE || rt == PT_PLSM))
				{
					sim->part_change_type(i, x, y, PT_FIRE);
					parts[i].temp += 200.0f;
					return 1;
				}
				else if (parts[i].ctype == PT_SAWD && isWater)
				{
					sim->part_change_type(i, x, y, PT_PULP);
					parts[i].life += 1000;
					return 1;
				}
				else if (parts[i].ctype == PT_ALMN && rt == PT_BRMT && parts[i].temp > 240.0f + 273.15f && sim->rng.chance(1, 100))
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_THRM);
					sim->part_change_type(i, x, y, PT_THRM);
					return 1;
				}
			}
		}
	}

	if (parts[i].ctype == PT_PAPR)
	{
		parts[i].dcolour = 0;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	static bool allowRecursion = true;

	if (cpart->ctype > 0 && cpart->ctype < PT_NUM && elements[cpart->ctype].Enabled)
	{
		*colr = elements[cpart->ctype].Colour.Red;
		*colg = elements[cpart->ctype].Colour.Green;
		*colb = elements[cpart->ctype].Colour.Blue;

		if (allowRecursion && elements[cpart->ctype].Graphics)
		{
			allowRecursion = false;
			elements[cpart->ctype].Graphics(GRAPHICS_FUNC_SUBCALL_ARGS);
			allowRecursion = true;
		}
	}
	return 0;
}
