#include "simulation/ElementCommon.h"
#include "CESM.h"

// See https://powdertoy.co.uk/Discussions/Thread/View.html?Thread=21682

void Element::Element_CESM()
{
	Identifier = "DEFAULT_PT_CESM";
	Name = "CESM";
	Colour = 0xD1E765_rgb;
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
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
	Meltable = 50;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 255;
	Description = "Cesium. Reactive alkali metal. Very low melting point.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 28.0f + 273.15f;
	HighTemperatureTransition = PT_LCSM;

	Update = &Element_CESM_update;
	Graphics = &Element_CESM_graphics;
}

int Element_CESM_update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * tmp - Type
	 * 	0 = Default
	 * 	1 = Cesium oxide
	 *  2 = CsAu
	 *  -1 = explosion
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	constexpr int checkCoordsX[] = { -4, 4,  0, 0 };
	constexpr int checkCoordsY[] = {  0, 0, -4, 4 };

	int explosionType = 0;

	// Gold like conduction
	if (!parts[i].life && parts[i].tmp == 2)
	{
		for(auto j = 0; j < 4; j++)
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
				parts[i].ctype = PT_CESM;
			}
		}
	}

	// Release O2
	if (parts[i].tmp == 1 && (parts[i].life || parts[i].tmp > 500.0f + 273.15f))
	{
		sim->part_change_type(i, x, y, PT_LCSM);
		sim->create_part(-1, x + sim->rng.between(-1, 2), y + sim->rng.between(-1, 2), PT_O2);
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
					r = sim->photons[y + ry][x + rx];
				}
				if (!r)
				{
					continue;
				}
				auto rt = TYP(r);

				if (parts[i].tmp != -1)
				{
					if (parts[i].temp > 1600.0f + 273.15f || rt == PT_DFLM || rt == PT_FIRE || rt == PT_PLSM || rt == PT_WTRV || elements[rt].Properties & PROP_WATER)
					{
						explosionType = rt;
						sim->flood_prop(x, y, AccessProperty{ FIELD_TMP, -1 });
					}
				}

				if (rt == PT_PHOT) // PHOT to ELEC
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_ELEC);
				}
				else if (rt == PT_ACID || rt == PT_CAUS) // Acid reactions
				{
					if (parts[i].tmp == 1)
					{
						sim->part_change_type(i, x, y, sim->rng.chance(1, 2) ? PT_DSTW : PT_SALT);
						return 1;
					}
					else
					{
						sim->part_change_type(i, x, y, sim->rng.chance(1, 2) ? PT_SALT : PT_H2);
						return 1;
					}
				}
				else if (rt == PT_O2) // Conversion to cesium oxide
				{
					parts[i].tmp = 1;
					sim->kill_part(ID(r));
				}
				else if (rt == PT_GOLD) // Coversion to CsAu
				{
					parts[i].tmp = 2;
				}
			}
		}
	}

	// Explosion code
	if (parts[i].tmp == -1)
	{
		sim->pv[y / CELL][x / CELL] += 2.0f;
		float otemp = parts[i].temp - 273.15f;

		// Stolen from TNT lol
		if (sim->rng.chance(1, 2))
		{
			if (sim->rng.chance(1, 2))
			{
				sim->create_part(-3, x, y, PT_FIRE);
			}
			else
			{
				sim->create_part(-3, x, y, PT_SMKE);
				parts[i].life = sim->rng.between(500, 549);
			}

			parts[i].temp = restrict_flt((MAX_TEMP / 4) + otemp, MIN_TEMP, MAX_TEMP);
		}
		else if (sim->rng.chance(1, 10))
		{
			sim->create_part(-3, x, y, PT_EMBR);
			parts[i].life = 50;
			parts[i].vx = sim->rng.between(-10, 10);
			parts[i].vy = sim->rng.between(-10, 10);
			parts[i].temp = restrict_flt((MAX_TEMP / 3) + otemp, MIN_TEMP, MAX_TEMP);
			parts[i].tmp = 0;
		}

		if (parts[i].tmp == 2 && (explosionType == PT_WATR || explosionType == PT_WTRV || explosionType == PT_DSTW))
		{
			sim->part_change_type(i, x, y, sim->rng.chance(1, 2) ? PT_GOLD : PT_SALT);
		}
		else
		{
			sim->kill_part(i);
		}

		return 1;
	}

	return 0;
}

int Element_CESM_graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->tmp == 1) // Cesium oxide
	{
		*colr *= 0.8f;
		*colg *= 0.8f;
		*colb *= 0.8f;
	}
	else if (cpart->tmp == 2) // CsAu
	{
		*colr *= 1.15f;
		*colg *= 1.05f;
	}

	return 0;
}
