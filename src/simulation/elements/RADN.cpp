#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static int liquid_graphics(GRAPHICS_FUNC_ARGS);
static int frozen_graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_RADN()
{
	Identifier = "DEFAULT_PT_RADN";
	Name = "RADN";
	Colour = 0xFEFE00_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 1.0f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.18f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 5;

	HeatConduct = 42;
	Description = "Radon, unstable gas. Decays with slow, hot neutrons.";

	Properties = TYPE_GAS | PROP_RADIOACTIVE | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = -61.7f + 273.15f;
	LowTemperatureTransition = PT_LQUD;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;

	LiquidGraphics = &liquid_graphics;
	FrozenGraphics = &frozen_graphics;

	Create = &create;

	MeltingPoint = -71.0f + 273.15f;
	BoilingPoint = -61.7f + 273.15f;

	DefaultProperties.temp = R_TEMP + 2.0f + 273.15f;
	DefaultProperties.tmp = 135;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * tmp  - neutrons left
	 * tmp2 - Glow?
	 */

	bool decayThisFrame = sim->rng.chance(1, 30000);

	if (!decayThisFrame)
	{
		for (auto rx = -1; rx <= 1; rx++)
		{
			for (auto ry = -1; ry <= 1; ry++)
			{
				if (rx || ry)
				{
					auto r = sim->photons[y + ry][x + rx];
					if (!r)
					{
						continue;
					}
					auto rt = TYP(r);

					if ((rt == PT_NEUT || rt == PT_PROT) && std::hypot(parts[ID(r)].vx, parts[ID(r)].vy) < 1.7f && parts[ID(r)].temp > 1000.0f)
					{
						decayThisFrame = true;
						goto end;
					}
				}
			}
		}
	}

end:

	if (decayThisFrame)
	{
		parts[i].temp += 2.0f;

		for (auto j = 0; j < 3; j++)
		{
			if (parts[i].tmp)
			{
				parts[i].tmp--;
			}

			sim->create_part(-3, x, y, PT_NEUT);
		}
	}

	if (!parts[i].tmp)
	{
		sim->part_change_type(i, x, y, PT_POLO);
		return 1;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*pixel_mode &= ~PMODE;

	*firer = *colr / 2;
	*fireg = *colg / 2;
	*fireb = *colb / 2;
	*firea = 80;

	*pixel_mode |= cpart->tmp2 ? FIRE_ADD : FIRE_BLEND;

	return 0;
}

static int liquid_graphics(GRAPHICS_FUNC_ARGS)
{
	*pixel_mode |= PMODE_BLUR;

	// Orange-yellow
	*colr = 0xFF;
	*colg = 0xB3;
	*colb = 0x0;

	return 1;
}

static int frozen_graphics(GRAPHICS_FUNC_ARGS)
{
	// Orange-red
	*colr = 0xFF;
	*colg = 0x55;
	*colb = 0x0;

	if (cpart->type == PT_ICEI)
	{
		*pixel_mode &= ~PMODE;
		*pixel_mode |= FIRE_ADD;

		*firer = *colr / 1.5f;
		*fireg = *colg / 1.5f;
		*fireb = *colb / 1.5f;
		*firea = 20;
	}

	return 1;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].tmp2 = sim->rng.between(0, 1);
}
