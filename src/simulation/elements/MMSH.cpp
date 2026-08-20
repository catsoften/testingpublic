#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_MMSH()
{
	Identifier = "DEFAULT_PT_MMSH";
	Name = "MMSH";
	Colour = 0x91A8B5_rgb;
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
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Metallic mesh. Allows energy particles, blocks insects, birds and fish.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_HOT_GLOW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = ITH;
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

	// Custom melting code
	if (parts[i].temp > 1273.0f)
	{
		sim->part_change_type(i, x, y, PT_LAVA);
		parts[i].ctype = PT_BMTL;
		return 1;
	}

	// Sometimes block powders / liquids
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

				if (elements[TYP(r)].Properties & TYPE_PART || elements[TYP(r)].Properties & TYPE_LIQUID)
				{
					if (rx == -isign(parts[ID(r)].vx) && ry == -isign(parts[ID(r)].vy))
					{
						parts[ID(r)].vx = parts[ID(r)].vy = 0.0f;
					}
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if ((nx + ny) % 2)
	{
		*colr *= 0.6f;
		*colg *= 0.6f;
		*colb *= 0.6f;
	}

	return 0;
}
