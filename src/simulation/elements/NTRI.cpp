#include "simulation/ElementCommon.h"
#include "NEUT.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_NTRI()
{
	Identifier = "DEFAULT_PT_NTRI";
	Name = "NTRI";
	Colour = 0xEB34C3_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = -0.99f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = -1;

	HeatConduct = 0;
	Description = "Neutrinos. Goes through everything, interacts with neutrons and electrons.";

	Properties = TYPE_ENERGY | PROP_LIFE_DEC | PROP_LIFE_KILL_DEC;

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

	Create = &Element_NEUT_create;

	DefaultProperties.temp = R_TEMP + 900.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto deutExplosion = [sim, x, y](int n, float temp, int t) {
		int i;

		n = n / 50;
		if (n < 1)
		{
			n = 1;
		}
		else if (n > 340)
		{
			n = 340;
		}

		for (int c = 0; c < n; c++)
		{
			i = sim->create_part(-3, x, y, t);
			if (i >= 0)
			{
				sim->parts[i].temp = temp;
			}
			else if (sim->parts.MaxPartsReached())
			{
				break;
			}
		}

		sim->pv[y / CELL][x / CELL] += (6.0f * CFDS) * n;
		sim->gravIn.mass[Vec2{ x, y } / CELL] = 20.0f;
	};

	auto r = pmap[y][x];
	if (r)
	{
		auto rt = TYP(r);

		if (rt == PT_HEAC) // Send heat to HEAC
		{
			parts[ID(r)].temp = parts[i].temp;
		}
		else if (rt == PT_DEUT) // Detonate DEUT
		{
			int pressureFactor = 3 + sim->pv[y / CELL][x / CELL];
			if (sim->rng.chance(pressureFactor + 1 + (parts[ID(r)].life / 100), 1000))
			{
				deutExplosion(parts[ID(r)].life, restrict_flt(parts[ID(r)].temp + parts[ID(r)].life * 500.0f, MIN_TEMP, MAX_TEMP), PT_NTRI);
				sim->kill_part(ID(r));
			}
		}
		else // Copy heat from anything else
		{
			parts[i].temp = parts[ID(r)].temp;
		}
	}

	r = sim->photons[y][x];
	if (r)
	{
		if (TYP(r) == PT_NEUT) // NEUT -> ELEC + PHOT
		{
			sim->part_change_type(ID(r), x, y, PT_ELEC);
			auto j = sim->create_part(-3, x, y, PT_PHOT);
			if (j >= 0)
			{
				parts[j].temp = parts[i].temp;
			}
		}
		else if (TYP(r) == PT_ELEC) // ELEC -> PHOT
		{
			sim->part_change_type(ID(r), x, y, PT_PHOT);
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	// Glow if inside another element
	if (gfctx.sim->pmap[ny][nx])
	{
		*firer = *colr;
		*fireg = *colg;
		*fireb = *colb;
		*firea = 255;
		*pixel_mode |= FIRE_ADD;
	}
	return 0;
}
