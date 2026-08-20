#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_PINV()
{
	Identifier = "DEFAULT_PT_PINV";
	Name = "PINV";
	Colour = 0x00CCCC_rgb;
	MenuVisible = 1;
	MenuSection = SC_POWERED;
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
	Hardness = 15;

	Weight = 100;

	HeatConduct = 164;
	Description = "Invisible to particles when powered.";

	Properties = TYPE_SOLID | PROP_NEUTPASS;

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

				if (TYP(r) == PT_SPRK)
				{
					if (parts[ID(r)].ctype == PT_PSCN)
					{
						sim->flood_prop(x, y, AccessProperty{ FIELD_LIFE, 10 });
					}
					else if (parts[ID(r)].ctype == PT_NSCN)
					{
						sim->flood_prop(x, y, AccessProperty{ FIELD_LIFE, 0 });
					}
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->life == 0)
	{
		*colr *= 0.3f;
		*colg *= 0.3f;
		*colb *= 0.3f;
	}

	return 0;
}
