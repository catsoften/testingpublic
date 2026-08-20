#include "simulation/ElementCommon.h"

#include <queue>

constexpr int MAX_RCRT_CONNECT = 5;

static int update(UPDATE_FUNC_ARGS);

void Element::Element_RCRT()
{
	Identifier = "DEFAULT_PT_RCRT";
	Name = "RCRT";
	Colour = 0xA8A8A8_rgb;
	MenuVisible = 1;
	MenuSection = SC_POWDERS;
	Enabled = 1;

	Advection = 0.1f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.3f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 2;
	Hardness = 2;

	Weight = 90;

	HeatConduct = 100;
	Description = "Reinforced concrete. Remains in place near supporting solids.";

	Properties = TYPE_PART | PROP_HOT_GLOW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 949.85f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * tmp  - How is it supported
	 * 		  1 = touching solid
	 * 		  2 = touching RCRT touching solid
	 * 		  3 = touching RCRT touching RCRT touching solid
	 * 		  ...
	 * Max support length is 5
	 * tmp2  - Is connected to a solid rn? Randomly resets, if not connected
	 * 		   Countsdown, then collapses
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	int possibleConnectTmp = MAX_RCRT_CONNECT + 1; // For initial connecting
	bool foundSolid = false;

	if (parts[i].tmp2)
	{
		parts[i].tmp2--;
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

				if (elements[rt].Properties & TYPE_SOLID || (rt == PT_ROCK && !parts[ID(r)].tmp2) || (rt == PT_CRBN && parts[ID(r)].tmp2))
				{
					parts[i].tmp = 1;
					foundSolid = true;
				}
				else if (rt == PT_RCRT && parts[ID(r)].tmp && !parts[i].tmp)
				{
					possibleConnectTmp = std::min(possibleConnectTmp, parts[ID(r)].tmp + 1);
				}
			}
		}
	}

	if (possibleConnectTmp != MAX_RCRT_CONNECT + 1)
	{
		parts[i].tmp = possibleConnectTmp;
	}

	// No longer connected to a solid
	if (!foundSolid && parts[i].tmp == 1)
	{
		parts[i].tmp = MAX_RCRT_CONNECT + 1; // 0 means not yet calculated, 6 skips that
	}

	// Floodfill connected to solid rn property
	if (parts[i].tmp == 1)
	{
		// Floodfills connecting RCRT to tmp2 = 5, only does this for nearest 5
		std::queue<std::pair<int, int>> queue;
		queue.push(std::make_pair(x, y));
		int floodFilled = 0;

		while (queue.size() && floodFilled < MAX_RCRT_CONNECT)
		{
			auto p = queue.front();
			if (parts[ID(sim->pmap[p.second][p.first])].tmp2 == 5)
			{
				queue.pop();
				continue;
			}
			parts[ID(sim->pmap[p.second][p.first])].tmp2 = 5;

			// Floodfill
			for (auto rx = -1; rx <= 1; rx++)
			{
				for (auto ry = -1; ry <= 1; ry++)
				{
					if (rx || ry)
					{
						if (TYP(sim->pmap[p.second + ry][p.first + rx]) == PT_RCRT)
						{
							queue.push(std::make_pair(p.first + rx, p.second + ry));
						}
					}
				}
			}

			queue.pop();
		}
	}

	if (parts[i].tmp && parts[i].tmp <= MAX_RCRT_CONNECT && parts[i].tmp2 && std::hypot(parts[i].vx, parts[i].vy) < 2.0f)
	{
		parts[i].vx = parts[i].vy = 0.0f;
		return 1;
	}

	return 0;
}
