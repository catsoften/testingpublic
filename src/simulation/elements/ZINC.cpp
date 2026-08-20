#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int molten_graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_ZINC()
{
	Identifier = "DEFAULT_PT_ZINC";
	Name = "ZINC";
	Colour = 0xBADEE3_rgb;
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
	Description = "Zinc. Slightly brittle metal, has a low melting point. Can be used as an anode in a battery.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 1.0f;
	HighPressureTransition = ST;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 419.5f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;

	MoltenGraphics = &molten_graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	// 5 pressure, + 1 for every 20 degrees above 0 C
	if (sim->pv[y / CELL][x / CELL] > 5.0f + (parts[i].temp - 273.15f) / 20.0f && sim->rng.chance(1, 10))
	{
		sim->part_change_type(i, x, y, PT_BRMT);
		return 1;
	}

	return 0;
}

static int molten_graphics(GRAPHICS_FUNC_ARGS)
{
	*pixel_mode |= PMODE_BLUR;

	float blending = std::pow(std::min(cpart->temp / (1000.0f + 273.15f), 1.0f), 5.0f);

	int fr = std::min((cpart->life - 100) * 2 + 0xE0, 255);
	int fg = std::min((cpart->life - 100) * 1 + 0x50, 192);
	int fb = std::min((cpart->life - 100) / 2 + 0x10, 128);

	*colr = 0xBA * 1.05f * (1.0f - blending) + blending * fr;
	*colg = 0xDE * 1.05f * (1.0f - blending) + blending * fg;
	*colb = 0xE3         * (1.0f - blending) + blending * fb;

	if (blending > 0.9f)
	{
		*pixel_mode |= FIRE_ADD;

		*firea = cpart->temp > 3000.0f ? 40 : 20;
		*firer = *colr;
		*fireg = *colg;
		*fireb = *colb;
	}

	return 0;
}
