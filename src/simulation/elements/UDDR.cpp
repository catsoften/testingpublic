#include "simulation/ElementCommon.h"

#include "FLSH.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_UDDR()
{
	Identifier = "DEFAULT_PT_UDDR";
	Name = "UDDR";
	Colour = 0xFFADAD_rgb;
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
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

	Flammable = 10;
	Explosive = 0;
	Meltable = 0;
	Hardness = 15;

	Weight = 100;

	HeatConduct = 104;
	Description = "Udder. Makes milk when squeezed (pressure). Requires nutrients to make more milk.";

	Properties = TYPE_SOLID | PROP_NEUTPENETRATE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 200.0f + 273.15f;
	HighTemperatureTransition = PT_FIRE;

	Update = &update;

	DefaultProperties.tmp = 1000;
	DefaultProperties.tmp2 = 1500;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties
	 * life:  Graphics
	 * tmp:   Oxygen stored
	 * tmp2:  Nutrients stored
	 * tmp3: Highest temperature
	 * tmp4: Type 0 = inside, 1 = skin, 2 = dead
	 */

	Element_FLSH_update(UPDATE_FUNC_SUBCALL_ARGS);

	// Override skin formation
	if (parts[i].tmp4 == 1)
	{
		parts[i].tmp4 = 0;
	}

	if (sim->pv[y / CELL][x / CELL] > 1.0f && parts[i].tmp4 != 2)
	{
		for (auto rx = -1; rx <= 1; rx++)
		{
			for (auto ry = -1; ry <= 1; ry++)
			{
				if (rx || ry)
				{
					auto r = pmap[y + ry][x + rx];
					if (!r && parts[i].tmp2 && sim->rng.chance(1, 50))
					{
						sim->create_part(-1, x + rx, y + ry, PT_MILK);
						parts[i].tmp2 = std::max(0, parts[i].tmp2 - 50);
					}
				}
			}
		}
	}

	return 0;
}
