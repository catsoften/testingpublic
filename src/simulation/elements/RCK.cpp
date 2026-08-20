#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_RCK()
{
	Identifier = "DEFAULT_PT_RCK";
	Name = "ROCK";
	Colour = 0x938F80_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
	Enabled = 1;

	Advection = 0.2f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.90f;
	Loss = 0.90f;
	Collision = 0.0f;
	Gravity = 0.5f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 121;
	Description = "Bowser's Rock. Tougher than brick, erodes slowly from moving water. Can be melted and refined into metals.";

	Properties = TYPE_PART | PROP_HOT_GLOW;

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

	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties
	 * - tmp   - Used for graphics
	 * - ctype - Type of ORE
	 * - tmp2  - Is powder form
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	bool validCtype = parts[i].ctype && parts[i].ctype < PT_NUM && elements[parts[i].ctype].Enabled;

	// High pressure, fragment and release gas
	if (sim->pv[y / CELL][x / CELL] > 12.0f)
	{
		int changeTo = 0;
		if (parts[i].ctype == PT_COAL)
		{
			changeTo = PT_BCOL;
		}
		else if (parts[i].ctype == PT_RDMD)
		{
			changeTo = PT_PQRT;
		}
		else if (parts[i].ctype == PT_TUNG)
		{
			changeTo = PT_BRMT;
		}
		else if (sim->rng.chance(1, 500))
		{
			changeTo = PT_OIL;
		}
		else if (sim->rng.chance(1, 500))
		{
			changeTo = PT_GAS;
		}
		else if (validCtype && (elements[parts[i].ctype].Properties & TYPE_PART || elements[parts[i].ctype].Properties & TYPE_LIQUID || elements[parts[i].ctype].Properties & TYPE_GAS))
		{
			changeTo = parts[i].ctype;
		}

		if (changeTo)
		{
			float temp = parts[i].temp;
			sim->create_part(i, x, y, changeTo);
			parts[i].temp = temp;
			return 1;
		}
		else
		{
			parts[i].tmp2 = 1;
		}
	}

	// Melt into the ore
	// If ore can't melt melt into regular lava
	if (validCtype && elements[parts[i].ctype].Meltable && parts[i].temp > elements[parts[i].ctype].HighTemperature)
	{
		sim->part_change_type(i, x, y, PT_LAVA);
		return 1;
	}
	else if (validCtype && !elements[parts[i].ctype].Meltable && parts[i].temp > 1000.0f + 273.15f)
	{
		parts[i].ctype = PT_ROCK;
		sim->part_change_type(i, x, y, PT_LAVA);
		return 1;
	}

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

				if (rt == PT_ICEI && sim->rng.chance(1, 1000)) // Expanding ICE may crack stone
				{
					parts[i].tmp2 = 1;
				}
				else if ((rt == PT_WATR || rt == PT_DSTW) && sim->rng.chance(1, 100)) // Convert WATR into SLTW
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_SLTW);
				}

				if (elements[rt].Properties & PROP_WATER && sim->rng.chance(1, 2000) && std::abs(parts[ID(r)].vy) + std::abs(parts[ID(r)].vx) > 0.1f) // Erode
				{
					sim->kill_part(i);
					return 1;
				}
				else if ((rt == PT_O2) && sim->rng.chance(1, 5000)) // Chance to rust randomly from O2
				{
					sim->part_change_type(i, x, y, PT_BMTL);
					return 1;
				}
			}
		}
	}

	if (!parts[i].tmp2)
	{
		parts[i].vx = parts[i].vy = 0.0f;
		return 1;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	int m = (cpart->tmp - 3) * 10;
	*colr += m;
	*colg += m;
	*colb += m;

	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	// Ores: COAL, IRON, TTAN, GOLD, RDMD, URAN, PLUT, TUNG, STNE
	int ore = sim->rng.between(0, 100);
	if (ore < 2) // Diamond ore
	{
		sim->parts[i].ctype = PT_RDMD;
	}
	else if (ore < 4)
	{
		sim->parts[i].ctype = PT_PLUT;
	}
	else if (ore < 6)
	{
		sim->parts[i].ctype = PT_GOLD;
	}
	else if (ore < 8)
	{
		sim->parts[i].ctype = PT_URAN;
	}
	else if (ore < 30)
	{
		sim->parts[i].ctype = PT_IRON;
	}
	else if (ore < 35)
	{
		sim->parts[i].ctype = PT_BMTL;
	}
	else if (ore < 40)
	{
		sim->parts[i].ctype = PT_COAL;
	}
	else if (ore < 42)
	{
		sim->parts[i].ctype = PT_TUNG;
	}
	else
	{
		sim->parts[i].ctype = PT_STNE;
	}

	sim->parts[i].tmp = sim->rng.between(0, 6);
}
