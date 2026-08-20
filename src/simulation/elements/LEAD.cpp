#include "simulation/ElementCommon.h"
#include "simulation/Air.h"

static int update(UPDATE_FUNC_ARGS);
static int molten_graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_LEAD()
{
	Identifier = "DEFAULT_PT_LEAD";
	Name = "LEAD";
	Colour = 0xD6D6FF_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
	Enabled = 1;

	Advection = 0.05f;
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
	Description = "Soft metal with a low melting point. Blocks pressure, absorbs energy particles.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_DEADLY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 327.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;

	MoltenGraphics = &molten_graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	// Block pressure and air
	sim->air->bmap_blockair[y / CELL][x / CELL] = 1;

	// Absorb energy
	for (int rx = -1; rx <= 1; rx++)
	{
		for (int ry = -1; ry <= 1; ry++)
		{
			auto r = sim->photons[y + ry][x + rx];
			if (!r || TYP(r) == PT_RSPK || TYP(r) == PT_BALI || TYP(r) == PT_JCB1 || TYP(r) == PT_NTRI)
			{
				continue;
			}

			parts[i].temp += parts[ID(r)].temp * 0.05f;
			sim->kill_part(ID(r));
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

	*colr = 0xD6 * 1.05f * (1.0f - blending) + blending * fr;
	*colg = 0xD6 * 1.05f * (1.0f - blending) + blending * fg;
	*colb = 0xFF         * (1.0f - blending) + blending * fb;

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
