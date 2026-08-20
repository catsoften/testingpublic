#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_PAPR()
{
	Identifier = "DEFAULT_PT_PAPR";
	Name = "PAPR";
	Colour = 0xFAFAFA_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
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

	Flammable = 50;
	Explosive = 0;
	Meltable = 0;
	Hardness = 50;

	Weight = 100;

	HeatConduct = 164;
	Description = "Paper. Can be stained, dissolves in water, burns quickly.";

	Properties = TYPE_SOLID | PROP_NEUTPENETRATE | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 232.8f + 273.15f; // 451 F
	HighTemperatureTransition = PT_FIRE;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * - life:    how much water it has
	 * - dcolour: current stained color
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	// Chance to dissolve if too wet
	if (parts[i].life > 4000 && sim->rng.chance(1, 900))
	{
		parts[i].dcolour = 0;
		sim->part_change_type(i, x, y, PT_PULP);
		return 1;
	}

	parts[i].life -= std::max(0, (int)((parts[i].temp - 273.15f) / 10.0f));
	if (parts[i].life <= 0)
	{
		parts[i].life = 0;
	}

	RGBA thisColor = RGBA::Unpack(parts[i].dcolour);

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

				bool isWater = elements[rt].Properties & PROP_WATER || rt == PT_WTRV;

				// Diffuse color to neighbours if self alpha > 100
				if (rt == PT_PAPR && parts[i].dcolour && parts[i].life > 800)
				{
					RGBA otherColor = RGBA::Unpack(parts[ID(r)].dcolour);
					if (otherColor.Alpha < thisColor.Alpha && thisColor.Alpha > 100)
					{
						float diffuseMulti = restrict_flt(parts[i].life / 6000.0f, 0.3f, 0.9f);
						parts[ID(r)].dcolour = otherColor.NoAlpha().Blend(thisColor).WithAlpha(std::max((uint8_t)(thisColor.Alpha * diffuseMulti), otherColor.Alpha)).Pack();
					}
				}

				// Stain self if not stained
				if (!parts[i].dcolour && rt != PT_PULP && (rx == 0 || ry == 0) && !isWater && (elements[rt].Properties & TYPE_LIQUID || elements[rt].Properties & TYPE_PART))
				{
					RGB otherColor = elements[rt].Colour;
					parts[i].dcolour = RGBA(
						std::min((int)(otherColor.Red * 1.5f), 255),
						std::min((int)(otherColor.Green * 1.5f), 255),
						std::min((int)(otherColor.Blue * 1.5f), 255),
						255
					).Pack();
				}

				// Get "wetter" with water
				if (isWater && parts[i].life < 40000)
				{
					parts[i].life += 800;
					sim->kill_part(ID(r));
					continue;
				}

				// Diffuse wetness
				if (rt == PT_PAPR && parts[ID(r)].life < parts[i].life && sim->rng.chance(1, 8))
				{
					int lose = std::min(parts[i].life, 1000);
					parts[i].life -= lose;
					parts[ID(r)].life += lose;
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	int darker = std::min(50, cpart->life / 200);
	*colr -= darker;
	*colg -= darker;
	*colb -= darker;

	return 0;
}
