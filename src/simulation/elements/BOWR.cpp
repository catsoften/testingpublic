#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_BOWR()
{
	Identifier = "DEFAULT_PT_BOWR";
	Name = "BOWR";
	Colour = 0xF5B400_rgb;
	MenuVisible = 1;
	MenuSection = SC_RANDOM;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.1f;
	Gravity = 2.0f;
	Diffusion = 0.75f;
	HotAir = 0.001f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 500;

	HeatConduct = 100;
	Description = "Bowserinator, highly destructive singularity";

	Properties = TYPE_GAS | PROP_NEUTPASS | PROP_INDESTRUCTIBLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPL;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;

	DefaultProperties.temp = MAX_TEMP;
}

static int update(UPDATE_FUNC_ARGS)
{
	sim->gravIn.mass[Vec2{ x, y } / CELL] = 100.0f;
	sim->create_part(-1, parts[i].x - 1, parts[i].y - 1, PT_THDR);
	sim->create_part(-1, parts[i].x - 1, parts[i].y + 1, PT_PLSM);
	sim->create_part(-1, parts[i].x + 1, parts[i].y - 1, PT_CO2);
	sim->create_part(-1, parts[i].x + 1, parts[i].y + 1, PT_PLSM);
	if (sim->rng.chance(1, 200))
	{
		sim->create_part(-1, parts[i].x - 1, parts[i].y - 1, PT_LIGH);
	}
	parts[i].temp = MAX_TEMP;

	int c = 0;
	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];

				// Kill even layered particles
				while (r && TYP(r) != PT_BOWR && c < 10)
				{
					sim->kill_part(ID(r));
					r = pmap[y + ry][x + rx];
					c++;
				}
			}
		}
	}
	return 0;
}
