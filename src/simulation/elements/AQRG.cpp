#include "simulation/ElementCommon.h"

constexpr static int COLOR_FRAMES = 300;

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_AQRG()
{
	Identifier = "DEFAULT_PT_AQRG";
	Name = "AQRG";
	Colour = 0xF05000_rgb;
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
	Hardness = 0;

	Weight = 31;

	HeatConduct = 34;
	Description = "Aqua regia. Can dissolve certain metals.";

	Properties = TYPE_LIQUID | PROP_DEADLY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = -42.0f + 273.15f;
	LowTemperatureTransition = NT;
	HighTemperature = 108.0f + 273.15f;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;

	DefaultProperties.life = 75;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * life:  Dissolving power left
	 * ctype: Metal dissolved
	 * tmp2:  Color timer
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	if (parts[i].temp < elements[PT_AQRG].LowTemperature)
	{
		parts[i].tmp = 1;
		parts[i].tmp2 = 0;
		sim->part_change_type(i, x, y, parts[i].ctype ? PT_BRKN : PT_ICEI);
		if (parts[i].type == PT_ICEI)
		{
			parts[i].ctype = PT_AQRG;
		}
		return 1;
	}
	else if (parts[i].temp > elements[PT_AQRG].HighTemperature)
	{
		parts[i].tmp = 1;
		parts[i].tmp2 = 0;
		sim->part_change_type(i, x, y, parts[i].ctype ? PT_BRKN : PT_WTRV);
		return 1;
	}

	for (int rx = -1; rx <= 1; rx++)
	{
		for (int ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					continue;
				}
				auto rt = TYP(r);

				auto rt2 = rt;
				if (rt2 == PT_BRKN)
				{
					rt2 = parts[ID(r)].ctype;
				}
				else if (rt2 == PT_ROCK)
				{
					rt2 = parts[ID(r)].ctype;
				}
				if (rt2 < 0 || rt2 > PT_NUM)
				{
					rt2 = 0;
				}
				bool rtIsNobleMetal = rt2 == PT_TIN || rt2 == PT_COPR || rt2 == PT_BRNZ || rt2 == PT_BRAS || rt2 == PT_PTNM || rt2 == PT_GOLD;

				if (rt == PT_BAKS || rt == PT_SOAP || rt == PT_WATR || rt == PT_DSTW) // Base / acid neutralizes
				{
					parts[i].life--;
					parts[i].temp += 0.5f;
					if (sim->rng.chance(1, 75))
					{
						sim->kill_part(ID(r));
					}
				}
				else if (rt == PT_AQRG && sim->rng.chance(1, 10)) // Random diffusion
				{
					if (parts[i].life > parts[ID(r)].life && parts[i].life > 10)
					{
						parts[i].life--;
						parts[ID(r)].life++;
					}
					else if (parts[i].ctype && !parts[ID(r)].ctype)
					{
						std::swap(parts[i].ctype, parts[ID(r)].ctype);
					}
				}
				else if (
					rt != PT_CLNE && rt != PT_PCLN && rt != PT_ACID && rt != PT_AQRG && rt != PT_CAUS &&
					parts[i].life >= 20 && !parts[i].ctype && (sim->rng.chance(elements[rt2].Hardness, 1000) || rtIsNobleMetal)
				) // Dissolve
				{
					// GLAS protects stuff from acid
					if (sim->parts_avg(i, ID(r), PT_GLAS) != PT_GLAS)
					{
						if (rtIsNobleMetal)
						{
							parts[i].ctype = rt2;
						}

						parts[i].temp += 2.0f;
						parts[i].life--;
						sim->kill_part(ID(r));
					}
				}
			}
		}
	}

	if (sim->rng.chance(1, 30))
	{
		parts[i].life--;
	}
	if (parts[i].life < 0)
	{
		parts[i].life = 0;
	}
	if (!parts[i].life && sim->rng.chance(1, 150))
	{
		parts[i].tmp = 1;
		parts[i].tmp2 = 0;
		sim->part_change_type(i, x, y, parts[i].ctype ? PT_BRKN : PT_CAUS);
		if (parts[i].type == PT_CAUS)
		{
			if (sim->rng.chance(1, 50))
			{
				parts[i].life = 75; // keep CAUS alive
			}
			else
			{
				sim->part_change_type(i, x, y, parts[i].ctype ? PT_BRKN : PT_WATR);
			}
		}
		return 1;
	}

	if (parts[i].tmp2 < COLOR_FRAMES)
	{
		parts[i].tmp2++;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*pixel_mode |= PMODE_BLUR;
	if (cpart->tmp2 < COLOR_FRAMES)
	{
		// Ease colors from water (2030D0) to F05000 (red)
		float ease = cpart->tmp2 * 1.0f / COLOR_FRAMES;
		*colr = 0x20 + (0xF0 - 0x20) * ease;
		*colg = 0x30 + (0x50 - 0x30) * ease;
		*colb = 0xD0 + (0x00 - 0xD0) * ease;
	}

	return 0;
}
