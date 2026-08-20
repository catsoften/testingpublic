#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_POTO()
{
	Identifier = "DEFAULT_PT_POTO";
	Name = "POTO";
	Colour = 0xB08464_rgb;
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
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
	Meltable = 0;
	Hardness = 20;

	Weight = 50;

	HeatConduct = 150;
	Description = "Potatoes.";

	Properties = TYPE_PART | PROP_EDIBLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 600.0f + 273.15f;
	HighTemperatureTransition = PT_FIRE;

	FoodValue = 10;

	Update = &update;
	Graphics = &graphics;

	Create = &create;

	DefaultProperties.life = 4;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * tmp3 - max temp
	 * life  - water content
	 * tmp   - graphics
	 */

	if (parts[i].temp > parts[i].tmp3)
	{
		parts[i].tmp3 = parts[i].temp;
	}

	if (parts[i].temp > 100.0f + 273.15f && parts[i].life > 0)
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
						parts[i].life--;
						sim->create_part(-1, x + rx, y + ry, PT_WTRV);
						return 0;
					}
				}
			}
		}
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
