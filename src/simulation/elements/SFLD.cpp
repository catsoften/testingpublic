#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_SFLD()
{
	Identifier = "DEFAULT_PT_SFLD";
	Name = "SFLD";
	Colour = 0x8EC6ED_rgb;
	MenuVisible = 1;
	MenuSection = SC_LIQUID;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = 1.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 30;

	HeatConduct = 255;
	Description = "Superfluid neon. Likes to flow along surfaces.";

	Properties = TYPE_LIQUID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = -246.048f + 273.15f;
	HighTemperatureTransition = PT_NEON;

	Update = &update;

	DefaultProperties.temp = 20.0f;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	std::pair<int, int> newv;
	int maxSolids = 0;

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					// Check if we can move to empty spot
					// Pick a spot to maximize number of nearby solids
					int solidCount = 0;
					for (auto rx2 = -1; rx2 <= 1; rx2++)
					{
						for (auto ry2 = -1; ry2 <= 1; ry2++)
						{
							if (rx2 || ry2)
							{
								auto r2 = pmap[y + ry + ry2][x + rx + rx2];
								if (r2 && (elements[TYP(r2)].Properties & TYPE_SOLID || elements[TYP(r2)].Properties & TYPE_PART))
								{
									solidCount++;
								}
							}
						}
					}

					if (solidCount > maxSolids)
					{
						maxSolids = solidCount;
						newv = std::make_pair(rx, ry);
					}
				}
			}
		}
	}

	if (maxSolids)
	{
		parts[i].vx = newv.first;
		parts[i].vy = newv.second;
	}

	return 0;
}
