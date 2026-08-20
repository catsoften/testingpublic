#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_N2()
{
	Identifier = "DEFAULT_PT_N2";
	Name = "N2";
	Colour = 0x343685_rgb;
	MenuVisible = 1;
	MenuSection = SC_GAS;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.1f;
	Gravity = 0.0f;
	Diffusion = 1.50f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;
	PhotonReflectWavelengths = 0xFFF;

	Weight = 1;

	HeatConduct = 60;
	Description = "Nitrogen gas. Non-flammable. Puts out fires.";

	Properties = TYPE_GAS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = -210.0f + 273.15f;
	LowTemperatureTransition = PT_LNTG;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	int hygnCount = 0;
	int hygn[3];
	bool catalyst = false;

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

				if (rt == PT_FIRE) // Put out fire
				{
					sim->kill_part(ID(r));
				}
				else if (rt == PT_ACID || rt == PT_CAUS) // ACID + N2 = Aqua regia
				{
					sim->part_change_type(i, x, y, PT_AQRG);
					sim->part_change_type(ID(r), x + rx, y + ry, PT_AQRG);
					return 1;
				}
				else if (rt == PT_NITR) // N2 + NITR -> GEL
				{
					sim->part_change_type(i, x, y, PT_GEL);
					sim->part_change_type(ID(r), x + rx, y + ry, PT_GEL);
					return 1;
				}
				else if (rt == PT_H2) // 3 H2 + N2 + heat + pressure -> AMNA
				{
					if (hygnCount < 3)
					{
						hygn[hygnCount] = ID(r);
					}
					hygnCount++;
				}
				else if (rt == PT_IRON || rt == PT_PTNM) // Catalyst for haber process
				{
					catalyst = true;
				}
			}
		}
	}

	if (hygnCount == 3 && sim->pv[y / CELL][x / CELL] > 20.0f && parts[i].temp > 100.0f + 273.15f && sim->rng.chance(1, catalyst ? 200 : 20000))
	{
		sim->part_change_type(i, x, y, PT_AMNA);
		parts[i].temp += 5.0f;
		for (auto i = 0; i < 3; i++)
		{
			parts[hygn[i]].temp += 5.0f;
			sim->part_change_type(hygn[i], (int)(parts[hygn[i]].x + 0.5f), (int)(parts[hygn[i]].y + 0.5f), PT_AMNA);
		}
		return 1;
	}

	return 0;
}
