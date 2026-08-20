#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_PHSP()
{
	Identifier = "DEFAULT_PT_PHSP";
	Name = "PHSP";
	Colour = 0xEDD5D3_rgb;
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
	Enabled = 1;

	Advection = 0.3f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 2;
	PhotonReflectWavelengths = 0xFFFFFFFF;

	Weight = 90;

	HeatConduct = 150;
	Description = "Phosphorus. Highly flammable powder.";

	Properties = TYPE_PART | PROP_LIFE_KILL_DEC | PROP_SPARKSETTLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 280.5f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * life: Fire burn
	 * tmp:  Phot absorb timer
	 * tmp2: Is red variant
	 */
	if (parts[i].tmp > 0)
	{
		parts[i].tmp--;
	}

	// Change to red PHSP if hot
	if (parts[i].temp > 70.0f + 273.15f && sim->rng.chance(1, 2000))
	{
		parts[i].tmp2 = 1;
	}

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];

				// White PHSP random combustion
				if (!r && !parts[i].tmp2 && sim->rng.chance(1, 1000000))
				{
					sim->part_change_type(i, x, y, PT_FIRE);
					parts[i].life = 100;
					parts[i].temp += 200.0f;
					return 1;
				}

				if (TYP(r) == PT_FIRE || TYP(r) == PT_PLSM || TYP(r) == PT_LAVA) // Ignite
				{
					parts[i].life += sim->rng.between(0, 5);
				}
				else if (!r && parts[i].life) // Burn
				{
					auto j = sim->create_part(-1, x + rx, y + ry, sim->rng.chance(1, 3) ? PT_EMBR : PT_FIRE);
					if (j >= 0)
					{
						parts[j].life = sim->rng.between(50, 200);
						parts[j].vx = rx;
						parts[j].vy = ry;
						parts[j].temp = parts[i].temp + 1500.0f;
						parts[j].tmp2 = 1;
						parts[j].dcolour = 0xFFFFB37D;
					}
				}
				else if (TYP(r) == PT_PLNT) // Fertilize plnt
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_VINE);
					sim->kill_part(i);
					return 1;
				}
				else if (TYP(r) == PT_WTRV) // Glow when touching WTRV
				{
					parts[i].tmp++;
				}

				// Absorb PHOT
				r = sim->photons[y + ry][x + rx];
				if (TYP(r) == PT_PHOT && !parts[i].tmp2)
				{
					sim->kill_part(ID(r));
					parts[i].tmp += 30;
					if (sim->rng.chance(1, 10))
					{
						parts[i].tmp2 = 1;
					}
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->tmp && !cpart->tmp2)
	{
		*firea = 150;
		*firer = *fireg = *fireb = 255;
		*pixel_mode |= FIRE_ADD;
	}
	else if (cpart->tmp2)
	{
		// Red variant
		*colr = 194, *colg = 51, *colb = 19;
	}

	return 0;
}
