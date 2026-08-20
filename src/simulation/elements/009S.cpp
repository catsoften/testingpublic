#include "simulation/ElementCommon.h"
#include "009.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_009S()
{
	Identifier = "DEFAULT_PT_009S";
	Name = "009S";
	Colour = 0xFF3636_rgb;
	MenuVisible = 0;
	MenuSection = SC_SOLIDS;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = -0.0003f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 100;

	HeatConduct = 46;
	Description = "Solid SCP-009. Forms crystals.";

	Properties = TYPE_SOLID | PROP_LIFE_DEC | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = 273.15f;
	LowTemperatureTransition = PT_009;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;

	DefaultProperties.temp = -R_TEMP + 50.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * - tmp:  vx
	 * - tmp2: vy
	 * - tmp3: Max crystal size
	 * - tmp4: crystal size
	 */

	if (parts[i].tmp3 == 0)
	{
		parts[i].tmp = sim->rng.chance(1, 3) ? 0 : sim->rng.chance(1, 2) ? 0 : 2;
		parts[i].tmp2 = sim->rng.chance(1, 3) ? 0 : sim->rng.chance(1, 2) ? 0 : 2;
		parts[i].tmp3 = sim->rng.between(1, 30);
	}

	// Crystal growth
	if (parts[i].tmp4 < parts[i].tmp3)
	{
		auto nx = x + parts[i].tmp - 1;
		auto ny = y + parts[i].tmp2 - 1;
		if (nx >= 0 && nx < XRES && ny >= 0 && ny < YRES)
		{
			auto r = pmap[ny][nx];
			auto rt = TYP(r);
			int j = -1;

			if (rt == PT_PLNT || rt == PT_FLSH || rt == PT_UDDR || rt == PT_STMH)
			{
				sim->kill_part(ID(r));
				r = 0;
			}

			if (!r)
			{
				j = sim->create_part(-1, nx, ny, PT_009S);
				parts[j].tmp = parts[i].tmp;
				parts[j].tmp2 = parts[i].tmp2;
			}
			else if (rt == PT_WATR || rt == PT_DSTW || rt == PT_SLTW || rt == PT_CBNW || rt == PT_IOSL ||
				rt == PT_SWTR || rt == PT_MILK || rt == PT_PULP || rt == PT_GLUE || rt == PT_WTRV ||
				rt == PT_MUD || rt == PT_BCTR || rt == PT_BLOD)
			{
				j = ID(r);
			}

			if (j >= 0)
			{
				parts[j].tmp3 = parts[i].tmp3;
				parts[j].tmp4 = parts[i].tmp4 + 1;
			}
		}
	}

	return Element_009_update(UPDATE_FUNC_SUBCALL_ARGS);
}
