#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static bool ctypeDraw(CTYPEDRAW_FUNC_ARGS);

void Element::Element_CLUD()
{
	Identifier = "DEFAULT_PT_CLUD";
	Name = "CLUD";
	Colour = 0xCCCCCC_rgb;
	MenuVisible = 1;
	MenuSection = SC_GAS;
	Enabled = 1;

	Advection = 0.09f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.10f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 10;

	HeatConduct = 0;
	Description = "Cloud. Weather depends on ctype. (SNOW / WATR / LIGH / LAVA / LN2)";

	Properties = TYPE_GAS;

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

	CtypeDraw = &ctypeDraw;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * - ctype
	 * 	- WATR - Rain, gray cloud
	 *  - LIGH - Thunderstorm, LIGH randomly. Lights up randomly
	 * 			 may make BALI
	 *  - SNOW - Snow storm
	 *  - GLAS - GLAS (Molten if cloud is hot enough)
	 *  - LN2  - Liquid nitrogen
	 *  - LAVA - Molten LAVA
	 */

	// Rain
	if (sim->rng.chance(1, 1500))
	{
		int j = -1;
		if (parts[i].ctype == PT_LIGH || parts[i].ctype == PT_THDR || parts[i].ctype == PT_WATR || parts[i].ctype == PT_DSTW)
		{
			j = sim->create_part(-3, x, y, PT_WATR);
		}
		else if (parts[i].ctype == PT_GLAS || parts[i].ctype == PT_BGLA)
		{
			j = sim->create_part(-3, x, y, PT_BGLA);
		}
		else if (parts[i].ctype == PT_LAVA)
		{
			j = sim->create_part(-3, x, y, PT_LAVA);
		}
		else if (parts[i].ctype == PT_LNTG)
		{
			j = sim->create_part(-3, x, y, PT_LNTG);
		}
		else if (parts[i].ctype == PT_SNOW || parts[i].ctype == PT_ICEI)
		{
			j = sim->create_part(-3, x, y, PT_SNOW);
		}

		// Rain = cloud temp
		if (j >= 0)
		{
			parts[j].temp = parts[i].temp;
		}
	}

	// Lightning
	if (sim->rng.chance(1, 500000) && (parts[i].ctype == PT_LIGH || parts[i].ctype == PT_THDR))
	{
		sim->create_part(-3, x, y, PT_LIGH);
	}

	// BALI (super rare!)
	if (sim->rng.chance(1, 500000000) && (parts[i].ctype == PT_LIGH || parts[i].ctype == PT_THDR))
	{
		sim->create_part(-3, x, y, PT_BALI);
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	// Vary color depending on type of cloud
	float m = 1.0f;

	if (cpart->ctype == PT_LIGH || cpart->ctype == PT_THDR) // Dark storm cloud
	{
		m = 0.2f;
	}
	else if (cpart->ctype == PT_WATR || cpart->ctype == PT_DSTW) // Rain
	{
		m = 0.5f;
	}
	else if (cpart->ctype == PT_LNTG || cpart->ctype == PT_ICEI || cpart->ctype == PT_SNOW) // Snow or LN2
	{
		m = 0.8f;
	}
	else if (cpart->ctype == PT_GLAS || cpart->ctype == PT_LAVA) // Molten
	{
		*colr *= 0.5f;
		*colg *= 0.2f;
		*colb *= 0.1f;
	}

	if (m != 1.0f)
	{
		*colr *= m;
		*colg *= m;
		*colb *= m;
	}

	// Gas effect
	*pixel_mode &= ~PMODE;
	*pixel_mode |= FIRE_BLEND;
	*pixel_mode |= DECO_FIRE;

	*firer = *colr * 0.7f;
	*fireg = *colg * 0.7f;
	*fireb = *colb * 0.7f;
	*firea = 125;

	return 0;
}

static bool ctypeDraw(CTYPEDRAW_FUNC_ARGS)
{
	if (t != PT_BGLA && t != PT_DSTW && t != PT_GLAS && t != PT_ICEI && t != PT_LAVA && t != PT_LIGH && t != PT_LNTG && t != PT_SNOW && t != PT_THDR && t != PT_WATR)
	{
		return false;
	}

	return Element::basicCtypeDraw(CTYPEDRAW_FUNC_SUBCALL_ARGS);
}
