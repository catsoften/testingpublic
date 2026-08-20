#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_BRNZ()
{
	Identifier = "DEFAULT_PT_BRNZ";
	Name = "BRNZ";
	Colour = 0xFFD187_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 1;
	Hardness = 20;
	PhotonReflectWavelengths = 0xFFA0000;

	Weight = 100;

	HeatConduct = 251;
	Description = "Bronze. Kills bacteria, made from mixing copper and tin. Decelerates air.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC | PROP_HOT_GLOW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = ITH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 950.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * tmp, 0 = not green, anything > 0 = green speckles
	 * tmp2 Non-green black coating? (tmp overrides)
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	sim->vx[y / CELL][x / CELL] /= 1.02f;
	sim->vy[y / CELL][x / CELL] /= 1.02f;

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					if (sim->rng.chance(1, 2000))
					{
						parts[i].tmp2 = 1;
					}
					continue;
				}

				// Uncorrode black corrosion
				if (TYP(r) == PT_SOAP || TYP(r) == PT_VNGR || TYP(r) == PT_BAKS)
				{
					parts[i].tmp2 = 0;
				}

				// Corrode
				if (sim->rng.chance(1, 100))
				{
					if (parts[i].tmp == 0 && (TYP(r) == PT_ACID || TYP(r) == PT_CAUS || elements[TYP(r)].Properties & PROP_WATER || TYP(r) == PT_O2 || TYP(r) == PT_WTRV || TYP(r) == PT_AMNA))
					{
						parts[i].tmp = sim->rng.between(1, 7);
					}

					// Corrode randomly depending on self ID if touching corroded BRNZ or COPR
					if (parts[i].tmp == 0 && (TYP(r) == PT_BRNZ || TYP(r) == PT_COPR) &&
						parts[ID(r)].tmp > 0 && ((i + ID(r)*(i- ID(r)))) % 5 == 0)
					{
						parts[i].tmp = sim->rng.between(1, 7);
					}
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->tmp > 0) // Green
	{
		int r = std::min(cpart->tmp, 10);
		*colr = 3 + 15 * r;
		*colg = 252 - 15 * r;
		*colb = 190 - 15 * r;
	}
	else if (cpart->tmp2 > 0) // Black
	{
		*colr *= 0.5;
		*colg *= 0.5;
		*colb *= 0.5;
	}

	return 0;
}
