#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_CLCM()
{
	Identifier = "DEFAULT_PT_CLCM";
	Name = "CLCM";
	Colour = 0xE7E7E7_rgb;
	MenuVisible = 1;
	MenuSection = SC_POWDERS;
	Enabled = 1;

	Advection = 0.4f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 5;
	Explosive = 0;
	Meltable = 5;
	Hardness = 1;

	Weight = 90;

	HeatConduct = 150;
	Description = "Calcium. Ignites when finely divided, makes hydrogen with water.";

	Properties = TYPE_PART | PROP_HOT_GLOW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 842.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
	Graphics = &graphics;

	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	int otherCLCM = 0;

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

				if (rt == PT_WATR || rt == PT_DSTW || rt == PT_SWTR || rt == PT_SLTW || rt == PT_CBNW || rt == PT_IOSL || rt == PT_WTRV)
				{
					sim->part_change_type(i, x, y, PT_H2);
					return 1;
				}
				else if (rt == PT_CLCM)
				{
					otherCLCM++;
				}
			}
		}
	}

	if (!otherCLCM && sim->rng.chance(1, 500))
	{
		sim->part_change_type(i, x, y, PT_FIRE);
		parts[i].life = sim->rng.between(50, 200);
		parts[i].temp += 500.0f;
		return 1;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	int z = (cpart->tmp - 5) * 8; // Speckles!
	*colr += z;
	*colg += z;
	*colb += z;

	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].tmp = sim->rng.between(0, 6);
}
