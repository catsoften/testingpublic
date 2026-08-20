#include "simulation/ElementCommon.h"
#include "009.h"

static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_009()
{
	Identifier = "DEFAULT_PT_009";
	Name = "009";
	Colour = 0xF04848_rgb;
	MenuVisible = 1;
	MenuSection = SC_LIQUID;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 30;

	HeatConduct = 29;
	Description = "SCP-009. Water with reversed enthalpy changes.";

	Properties = TYPE_LIQUID | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_NEUTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = -100.0f + 273.15f;
	LowTemperatureTransition = PT_009G;
	HighTemperature = -0.15f + 273.15f;
	HighTemperatureTransition = PT_009S;

	Update = &Element_009_update;
	Graphics = &graphics;

	DefaultProperties.temp = -R_TEMP + 2.0f + 273.15f;
}

int Element_009_update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					continue;
				}
				auto rt = TYP(r);

				if (elements[rt].Properties & PROP_WATER ||
					rt == PT_MILK || rt == PT_PULP || rt == PT_GLUE || rt == PT_WTRV || rt == PT_MUD ||
					rt == PT_BCTR || rt == PT_BLOD || rt == PT_HONY || rt == PT_VNGR)
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_009);
					parts[ID(r)].tmp = 0;
					parts[ID(r)].tmp2 = 0;
				}
				else if (rt == PT_PLNT || rt == PT_BEE || rt == PT_FISH || rt == PT_SPDR || rt == PT_ANT ||
					rt == PT_BIRD || rt == PT_ICEI || rt == PT_SNOW || rt == PT_FLSH || rt == PT_POTO ||
					rt == PT_ALGE || rt == PT_UDDR || rt == PT_STMH)
				{
					if (sim->rng.chance(1, 200))
					{
						sim->part_change_type(ID(r), x + rx, y + ry, PT_009);
						parts[ID(r)].tmp = 0;
						parts[ID(r)].tmp2 = 0;
					}
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*pixel_mode |= PMODE_BLUR;
	return 1;
}
