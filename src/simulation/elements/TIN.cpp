#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int molten_graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_TIN()
{
	Identifier = "DEFAULT_PT_TIN";
	Name = "TIN";
	Colour = 0xEFEADB_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
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
	Hardness = 0;
	PhotonReflectWavelengths = 0xFFFFFFF;

	Weight = 100;

	HeatConduct = 180;
	Description = "Tin. Poorer heat conductor than most metals, has a low melting point.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 1.0f;
	HighPressureTransition = ST;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 232.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;

	MoltenGraphics = &molten_graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * - tmp: is it an oxide layer
	 */

	// Break (> 5 pressure)
	if (sim->pv[y / CELL][x / CELL] > 5.0f)
	{
		parts[i].ctype = PT_TIN;
		sim->part_change_type(i, x, y, PT_BRKN);
		return 1;
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
					if (sim->rng.chance(1, 6))
					{
						parts[i].tmp = 1;
					}

					continue;
				}

				if (TYP(r) == PT_BMTL && parts[ID(r)].tmp) // Undo corrosion of IRON (BMTL w/ tmp + 1)
				{
					parts[ID(r)].tmp = 0;
					sim->part_change_type(ID(r), x + rx, y + ry, PT_IRON);
				}
				else if (!parts[i].tmp && (TYP(r) == PT_ACID || TYP(r) == PT_CAUS) && sim->rng.chance(1, 100)) // Only be dissolved if not an oxide layer
				{
					sim->kill_part(i);
					sim->kill_part(ID(r));
				}
				else if (parts[i].temp < 3.72f && TYP(r) == PT_SPRK) // Superconduction
				{
					sim->FloodINST(x, y, PT_TIN);
				}
			}
		}
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

	*colr = 0xFF         * (1.0f - blending) + blending * fr;
	*colg = 0xFA * 1.05f * (1.0f - blending) + blending * fg;
	*colb = 0xEB * 1.05f * (1.0f - blending) + blending * fb;

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
