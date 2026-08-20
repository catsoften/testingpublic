#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void changeType(ELEMENT_CHANGETYPE_FUNC_ARGS);

void Element::Element_LQUD()
{
	Identifier = "DEFAULT_PT_LQUD";
	Name = "LQUD";
	Colour = 0xFFFFFF_rgb;
	MenuVisible = 0;
	MenuSection = SC_LIQUID;
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
	Hardness = 0;

	Weight = 10;

	HeatConduct = 34;
	Description = "Generic liquid state.";

	Properties = TYPE_LIQUID;

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

	ChangeType = &changeType;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	if (parts[i].ctype < 0 || parts[i].ctype >= PT_NUM || !elements[parts[i].ctype].Enabled || parts[i].ctype == PT_LQUD)
	{
		parts[i].ctype = 0;
	}

	static bool allowRecursion = true;

	if (parts[i].ctype)
	{
		if (
			(
				elements[parts[i].ctype].HighPressureTransition == PT_LQUD &&
				sim->pv[y / CELL][x / CELL] < elements[parts[i].ctype].HighPressure &&
				elements[parts[i].ctype].HighPressure != IPH
			) ||
			(
				elements[parts[i].ctype].LowPressureTransition == PT_LQUD &&
				sim->pv[y / CELL][x / CELL] > elements[parts[i].ctype].LowPressure &&
				elements[parts[i].ctype].LowPressure != IPL
			) ||
			(
				elements[parts[i].ctype].HighTemperatureTransition == PT_LQUD &&
				parts[i].temp < elements[parts[i].ctype].HighTemperature &&
				elements[parts[i].ctype].HighTemperature != ITH
			) ||
			(
				elements[parts[i].ctype].LowTemperatureTransition == PT_LQUD &&
				parts[i].temp > elements[parts[i].ctype].LowTemperature &&
				elements[parts[i].ctype].LowTemperature != ITL
			)
		)
		{
			sim->part_change_type(i, x, y, parts[i].ctype);
			parts[i].ctype = 0;
			return 1;
		}
		else if (elements[parts[i].ctype].BoilingPoint > 0.0f && parts[i].temp > elements[parts[i].ctype].BoilingPoint)
		{
			sim->part_change_type(i, x, y, parts[i].ctype);
			return 1;
		}
		else if (parts[i].temp < elements[parts[i].ctype].MeltingPoint)
		{
			sim->part_change_type(i, x, y, PT_ICEI);
			return 1;
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
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	*pixel_mode |= PMODE_BLUR;

	if (cpart->ctype >= 0 && cpart->ctype < PT_NUM && elements[cpart->ctype].Enabled)
	{
		auto color = elements[cpart->ctype].Colour;

		*colr = color.Red;
		*colg = color.Green;
		*colb = color.Blue;

		if (elements[cpart->ctype].LiquidGraphics)
		{
			return elements[cpart->ctype].LiquidGraphics(GRAPHICS_FUNC_SUBCALL_ARGS);
		}
	}

	return 0;
}

static void changeType(ELEMENT_CHANGETYPE_FUNC_ARGS)
{
	auto ct = sim->parts[i].ctype;
	if (to == PT_LQUD && (!ct || ct == PT_LQUD))
	{
		sim->parts[i].ctype = from;
	}
}
