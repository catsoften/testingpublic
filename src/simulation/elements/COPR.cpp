#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_COPR()
{
	Identifier = "DEFAULT_PT_COPR";
	Name = "COPR";
	Colour = 0xE39E59_rgb;
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
	PhotonReflectWavelengths = 0xFFA0000;

	Weight = 100;

	HeatConduct = 251;
	Description = "Copper. Great conductor, can be used as a cathode in a battery.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_HOT_GLOW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 1.0f;
	HighPressureTransition = ST;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1085.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * tmp, 0 = not green, anything > 0 = green speckles
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	constexpr int checkCoordsX[] = { -4, 4,  0, 0 };
	constexpr int checkCoordsY[] = {  0, 0, -4, 4 };

	// Normal corrosion
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

				// Corrode
				if (sim->rng.chance(1, 100))
				{
					if (elements[TYP(r)].Properties & PROP_WATER || TYP(r) == PT_ACID || TYP(r) == PT_AMNA || TYP(r) == PT_CAUS || TYP(r) == PT_O2 || TYP(r) == PT_WTRV)
					{
						if (!parts[i].tmp)
						{
							parts[i].tmp = sim->rng.between(1, 7);
						}
					}

					// Corrode randomly depending on self ID if touching corroded COPR / BRNZ
					if (
						!parts[i].tmp && (TYP(r) == PT_BRNZ || TYP(r) == PT_COPR) &&
						parts[ID(r)].tmp && ((i + ID(r) * (i - ID(r)))) % 5 == 0
					)
					{
						parts[i].tmp = sim->rng.between(1, 7);
					}
				}
			}
		}
	}

	// Fast SPRK code, stolen from GOLD
	if (!parts[i].life)
	{
		for (auto j = 0; j < 4; j++)
		{
			auto rx = checkCoordsX[j];
			auto ry = checkCoordsY[j];
			auto r = pmap[y + ry][x + rx];
			if (!r)
			{
				continue;
			}

			if (TYP(r) == PT_SPRK && parts[ID(r)].life && parts[ID(r)].life < 4)
			{
				sim->part_change_type(i, x, y, PT_SPRK);
				parts[i].life = 4;
				parts[i].ctype = PT_COPR;
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	// Green
	if (cpart->tmp)
	{
		int r = std::min(cpart->tmp, 10);
		*colr = 3 + 15 * r;
		*colg = 252 - 15 * r;
		*colb = 190 - 15 * r;
	}

	return 0;
}
