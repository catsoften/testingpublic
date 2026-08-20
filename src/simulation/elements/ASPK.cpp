#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_ASPK()
{
	Identifier = "DEFAULT_PT_ASPK";
	Name = "ASPK";
	Colour = 0x45A9C4_rgb;
	MenuVisible = 1;
	MenuSection = SC_ELEC;
	Enabled = 1;

	Advection = 0.4f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.2f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 5;
	Hardness = 1;

	Weight = 30;

	HeatConduct = 50;
	Description = "Anti-static powder. Prevents most conductors from conducting spark, blocks lightning. Sticky.";

	Properties = TYPE_PART;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1309.85f + 273.15f;
	HighTemperatureTransition = PT_FIRE;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	float vx = 0.0f, vy = 0.0f;

	for (auto rx = -2; rx <= 2; rx++)
	{
		for (auto ry = -2; ry <= 2; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					r= sim->photons[y + ry][x + rx];
				}
				if (!r)
				{
					continue;
				}

				if (elements[TYP(r)].Properties & PROP_CONDUCTS)
				{
					parts[ID(r)].life = 4; // Prevent conduction
					vx += rx;
					vy += ry;
				}
				else if (TYP(r) == PT_ELEC || TYP(r) == PT_APRT || TYP(r) == PT_THDR)
				{
					sim->kill_part(ID(r));
				}
			}
		}
	}

	parts[i].vx += isign(vx);
	parts[i].vy += isign(vy);

	return 0;
}
