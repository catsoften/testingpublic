#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_TPRS()
{
	Identifier = "DEFAULT_PT_TPRS";
	Name = "TPRS";
	Colour = 0xBA002B_rgb;
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
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

	HeatConduct = 0;
	Description = "Tempreature Preserver. Keeps constant temp while transmitting heat";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPL;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
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
				auto r = pmap[y + ry][x + rx];
				if (!r || TYP(r) == PT_TPRS)
				{
					r = sim->photons[y + ry][x + rx];
				}
				if (!r)
				{
					continue;
				}

				float difference = (parts[i].temp - parts[ID(r)].temp) / 20.0f;
				parts[ID(r)].temp += difference;
				if (difference < 10.0f)
				{
					parts[ID(r)].temp = parts[i].temp;
				}
			}
		}
	}

	return 0;
}
