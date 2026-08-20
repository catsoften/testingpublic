#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_CMNT()
{
	Identifier = "DEFAULT_PT_CMNT";
	Name = "CMNT";
	Colour = 0xB8B8B8_rgb;
	MenuVisible = 1;
	MenuSection = SC_LIQUID;
	Enabled = 1;

	Advection = 0.2f;
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
	Hardness = 20;

	Weight = 70;

	HeatConduct = 50;
	Description = "Liquid concrete. Slowly dries out into reinforced concrete. Makes weaker concrete with contamination.";

	Properties = TYPE_LIQUID | PROP_LIFE_DEC | PROP_NEUTPENETRATE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 709.85f + 273.15f;
	HighTemperatureTransition = PT_STNE;

	Update = &update;
	Graphics = &graphics;

	Create = &create;

	DefaultProperties.life = 2000;
	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * life - water absorbed
	 * tmp  - Graphical speckles
	 * tmp2 - Purity, if contaminated may only produce regular CNCT
	 * 	0 = pure, 100 = total contamination
	 * 	> 50 = makes CNCT
	 * 	> 120 = makes STNE, not even CNCT lol
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	float nvx = 0.0f, nvy = 0.0f;

	// Decrement life based on temp
	if (parts[i].temp > 273.15f)
	{
		parts[i].life -= (parts[i].temp - 273.15f) / 50.0f;
	}

	// Change to concrete
	if (parts[i].life <= 0)
	{
		int type = PT_RCRT;
		if (parts[i].tmp2 > 120)
		{
			type = PT_STNE;
		}
		else if (parts[i].tmp2 > 50)
		{
			type = PT_CNCT;
		}
		parts[i].life = 0;
		parts[i].tmp = 0;
		parts[i].tmp2 = 0;
		sim->part_change_type(i, x, y, type);
		return 1;
	}

	// Heat up slightly when hardening
	parts[i].temp += 0.01f;

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

				// Stick to solids, powders, and concrete
				if (elements[rt].Properties & TYPE_SOLID || elements[rt].Properties & TYPE_PART)
				{
					nvx += rx;
					nvy += ry;
				}
				else if (rt == PT_CMNT && sim->rng.chance(1, 10))
				{
					nvx += rx * 0.2f;
					nvy += ry * 0.2f;
				}

				if (elements[rt].Properties & PROP_WATER) // Touching water
				{
					// Sugar water and salt water are impure and will make low quality concrete
					if (rt == PT_SWTR || rt == PT_SLTW || rt == PT_CBNW || rt == PT_IOSL)
					{
						parts[i].tmp2 += 5;
					}

					// Stores at most 3000 life per px
					if (parts[i].life < 3000)
					{
						parts[i].life = std::min(parts[i].life + 200, 3000);
						sim->kill_part(ID(r));
					}
				}
				else if ((rt == PT_CRBN || rt == PT_DUST || rt == PT_SALT) && sim->rng.chance(1, 100)) // Touching "impure" elements
				{
					parts[i].tmp2 += 2;
				}
				else if (rt == PT_CMNT && parts[ID(r)].life > parts[i].life) // Average life between particles
				{
					int avg = (parts[ID(r)].life + parts[i].life) / 2 + 1;
					parts[ID(r)].life = avg;
					parts[i].life = avg;
				}
			}
		}
	}

	// Stick
	if (nvx != 0.0f || nvy != 0.0f)
	{
		parts[i].vx = nvx;
		parts[i].vy = nvy;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	// 184 -> 168 depending on life, 3000 -> 0
	*colr = 184 - (184 - 168) * std::min(cpart->life / 3000.0f, 1.0f);
	*colg = 184 - (184 - 168) * std::min(cpart->life / 3000.0f, 1.0f);
	*colb = 184 - (184 - 168) * std::min(cpart->life / 3000.0f, 1.0f);

	int z = (cpart->tmp - 5) * 4; // Speckles!
	*colr += z;
	*colg += z;
	*colb += z;

	*pixel_mode |= PMODE_BLUR | PMODE_BLOB;

	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].tmp = sim->rng.between(0, 6);
}
