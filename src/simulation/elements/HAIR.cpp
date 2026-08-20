#include "SimulationConfig.h"
#include "simulation/ElementCommon.h"

constexpr static int HEAD_RADIUS = 12;

static int update(UPDATE_FUNC_ARGS);

void Element::Element_HAIR()
{
	Identifier = "DEFAULT_PT_HAIR";
	Name = "HAIR";
	Colour = 0x33302C_rgb;
	MenuVisible = 1;
	MenuSection = SC_RANDOM;
	Enabled = 1;

	Advection = 0.7f;
	AirDrag = 0.02f * CFDS;
	AirLoss = 0.96f;
	Loss = 0.80f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 10;
	Explosive = 0;
	Meltable = 0;
	Hardness = 30;

	Weight = 85;

	HeatConduct = 70;
	Description = "Hair. Sticks to STKM heads, gets dirty. Wash with SHPO.";

	Properties = TYPE_PART | PROP_NEUTPENETRATE | PROP_NOCTYPEDRAW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 232.778f + 273.15f;
	HighTemperatureTransition = PT_FIRE;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * tmp  - attached to STKM currently (-1 = GELed or frozen, 0 = not yet, 1 = STKM, 2 = STKM2, higher = FIGH)
	 * tmp2 - 1 = "base" strand, 2 = attached to a base strand
	 * life - ID of strand to attach to, if secondary
	 * tmp3/4 - Relative position to id
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	float xdiff = 0.0f, ydiff = 0.0f;

	auto playerLinkCheck = [&parts, i, &xdiff, &ydiff](playerst player, int newtmp) {
		if (player.spwn)
		{
			xdiff = parts[player.stkmID].x - parts[i].x,
			ydiff = parts[player.stkmID].y - parts[i].y;
			if (std::hypot(xdiff, ydiff) <= HEAD_RADIUS)
			{
				parts[i].tmp = newtmp;
				return true;
			}
		}
		return false;
	};

	// Invalid if >= max fighters
	if (parts[i].tmp >= 2 + MAX_FIGHTERS)
	{
		parts[i].tmp = parts[i].tmp2 = 0;
	}

	// Freeze if cold
	if (parts[i].temp < 0 && parts[i].tmp2 == 0)
	{
		parts[i].tmp = -1;
	}

	// Disconnect hair if STKM dies
	if ((parts[i].tmp == 1 && !sim->player.spwn) ||
		(parts[i].tmp == 2 && !sim->player2.spwn) ||
		(parts[i].tmp > 2 && !sim->fighters[parts[i].tmp - 2].spwn)
	)
	{
		parts[i].tmp = parts[i].tmp2 = 0;
	}

	// Attach hair if not already attached
	bool attach = false;
	if (parts[i].tmp == 0)
	{
		attach = playerLinkCheck(sim->player, 1);
		if (!attach)
		{
			attach = playerLinkCheck(sim->player2, 2);
			if (!attach)
			{
				for (auto j = 0; j < MAX_FIGHTERS; j++)
				{
					attach = playerLinkCheck(sim->fighters[j], j + 2);
					if (attach)
					{
						break;
					}
				}
			}
		}
	}

	// Attach to STKM
	if (attach)
	{
		parts[i].tmp2 = 1;
		parts[i].tmp3 = xdiff;
		parts[i].tmp4 = ydiff;
	}

	// Attach hair to other attached hair or get hair dirty
	for (auto rx = -2; rx <= 2; rx++)
	{
		for (auto ry = -2; ry <= 2; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					continue;
				}
				auto rt = TYP(r);

				if (rt == PT_HAIR && parts[i].tmp2 == 0 && parts[ID(r)].tmp == 1) // Attach to other hair
				{
					parts[i].life = ID(r);
					parts[i].tmp2 = 2;
					parts[i].tmp3 = rx;
					parts[i].tmp4 = ry;
				}
				else if (rt == PT_GEL && parts[i].tmp2 == 0) // Gel
				{
					parts[i].tmp = -1;
				}
				else if (rt != PT_HAIR && elements[rt].Properties & TYPE_PART && sim->rng.between(1, 100)) // "Stain" with dcolour of other elements
				{
					parts[i].dcolour = 0xFF000000 + elements[rt].Colour.Pack();
				}
			}
		}
	}

	// Move hair to target
	auto stkm = sim->player;
	if (parts[i].tmp == 2)
	{
		stkm = sim->player2;
	}
	else if (parts[i].tmp > 2)
	{
		stkm = sim->fighters[parts[i].tmp - 2];
	}

	// Check if hair is actually attached still (no extreme movements)
	if (parts[i].tmp2 > 0 && sim->frameCount % 10 == 0)
	{
		// Base strand: check STKM
		if (parts[i].tmp2 == 1)
		{
			int dx = parts[stkm.stkmID].x - x, dy = parts[stkm.stkmID].y - y;
			int dis = std::hypot(dx, dy);

			if (dis > 2 * HEAD_RADIUS)
			{
				parts[i].tmp = 0;
				parts[i].tmp2 = 0;
			}
		}
	}

	if (parts[i].tmp > 0) // Attached to STKM
	{
		parts[i].x = parts[stkm.stkmID].x - parts[i].tmp3;
		parts[i].y = parts[stkm.stkmID].y - parts[i].tmp4;
	}
	else if (parts[i].tmp2 == 2) // Attached to other hair
	{
		// Base strand no longer valid
		if (parts[i].life < 0 || parts[i].life >= NPART || parts[parts[i].life].type != PT_HAIR || parts[parts[i].life].tmp2 != 1)
		{
			parts[i].tmp = 0;
			parts[i].tmp2 = 0;
		}
		else
		{
			parts[i].x = parts[parts[i].life].x - parts[i].tmp3;
			parts[i].y = parts[parts[i].life].y - parts[i].tmp4;
		}
	}

	// Stop moving if hair is attached or GELed
	if (parts[i].tmp2 != 0 || parts[i].tmp == -1)
	{
		parts[i].vx = parts[i].vy = 0.0f;
	}

	return 0;
}
