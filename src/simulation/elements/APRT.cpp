#include "simulation/ElementCommon.h"
#include "PROT.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_APRT()
{
	Identifier = "DEFAULT_PT_APRT";
	Name = "APRT";
	Colour = 0x7A14FF_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = -0.99f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = -1;

	HeatConduct = 61;
	Description = "Anti-Protons. Annihilates on contact with regular matter, sparks conductors.";

	Properties = TYPE_ENERGY;

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
	Create = &Element_PROT_create;

	DefaultProperties.life = 75;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	sim->pv[y / CELL][x / CELL] += 0.003f;

	for (auto rx = -2; rx <= 2; rx++)
	{
		for (auto ry = -2; ry <= 2; ry++)
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

			// Under check
			if (!rx && !ry)
			{
				switch (rt)
				{
					case PT_LCRY: //Powered LCRY reaction: APRT->PHOT
						if (parts[ID(r)].life > 5 && sim->rng.chance(1, 10))
						{
							sim->part_change_type(i, x, y, PT_PHOT);
							parts[i].life *= 2;
							parts[i].ctype = 0x3FFFFFFF;
							return 1;
						}
						break;

					case PT_INVIS:
						if (sim->rng.chance(1, 10))
						{
							sim->part_change_type(i, x, y, PT_NEUT);
							return 1;
						}
						break;
				}
			}

			switch (rt)
			{
				case PT_ELEC: // Repel
					parts[ID(r)].vx += rx;
					parts[ID(r)].vy += ry;
					parts[i].vx -= rx;
					parts[i].vy -= ry;
					break;

				case PT_PROT: // Turn into 2 PHOT based on kinetic energy
				{
					float totalE = std::hypot(parts[i].vx, parts[i].vy) + std::hypot(parts[ID(r)].vx, parts[ID(r)].vy);

					parts[ID(r)].temp += totalE / 2.0f * 700.0f;
					parts[i].temp += totalE / 2.0f * 700.0f;

					float angle = sim->rng.between(0, 359) / 180.0f * std::numbers::pi_v<float>;
					float vx = totalE / 2.0f * std::cos(angle);
					float vy = totalE / 2.0f * std::sin(angle);

					parts[ID(r)].vx = vx;
					parts[ID(r)].vy = vy;
					parts[i].vx = -vx;
					parts[i].vy = -vy;

					parts[ID(r)].life = parts[i].life = 1000;
					parts[ID(r)].ctype = parts[i].ctype = 0x3F000000 >> std::min(29, (int)totalE);

					sim->part_change_type(i, x, y, PT_PHOT);
					sim->part_change_type(ID(r), x + rx, y + ry, PT_PHOT);

					auto ni = sim->create_part(-3, x + rx / 2, y + ry / 2, PT_EMBR);
					if (ni >= 0)
					{
						parts[ni].life = 70;
						parts[ni].temp = MAX_TEMP;
						parts[ni].tmp = 3;
					}

					return 1;
					break;
				}

				case PT_PLUT:
					if (sim->rng.chance(1, 20))
					{
						sim->part_change_type(ID(r), x + rx, y + ry, PT_URAN);
						goto annihilate;
					}
					break;

				case PT_URAN:
					if (sim->rng.chance(1, 20))
					{
						sim->part_change_type(ID(r), x + rx, y + ry, PT_POLO);
						goto annihilate;
					}
					break;

				case PT_POLO:
					if (sim->rng.chance(1, 20))
					{
						sim->part_change_type(ID(r), x + rx, y + ry, PT_STNE);
						goto annihilate;
					}
					break;

				// Particle collisions
				case PT_APRT: // Should be mostly opposite directions
				{
					float difference = std::atan2(-parts[i].vy, parts[i].vx) - std::atan2(-parts[ID(r)].vy, parts[ID(r)].vx);
					if (difference < 0.0f)
					{
						difference += 6.28319f;
					}

					if (difference > 3.12659f && difference < 3.15659f)
					{
						float totalE = std::hypot(parts[i].vx, parts[i].vy) + std::hypot(parts[ID(r)].vx, parts[ID(r)].vy);

						if (totalE > 70)
						{
							sim->part_change_type(i, x, y, PT_SING);
						}
						else if (totalE > 20)
						{
							sim->part_change_type(i, x, y, PT_AMTR);
							parts[i].tmp2 = 1;
						}
						else if (totalE >= 10)
						{
							sim->part_change_type(i, x, y, PT_ANH2);
						}
						else // Not fast enough
						{
							break;
						}

						parts[i].temp += totalE * 700.0f;
						sim->kill_part(ID(r));

						return 1;
					}
					break;
				}

				case PT_NONE:
					break;

				default:
					if (elements[rt].Properties & PROP_WATER)
					{
						sim->create_part(ID(r), x + rx, y + ry, sim->rng.chance(1, 3) ? PT_O2 : PT_H2);
						goto annihilate;
					}

					if ((elements[rt].Properties & PROP_CONDUCTS) && rt != PT_MMSH && (rt != PT_NBLE || parts[i].temp < 2273.15))
					{
						sim->create_part(-1, x + rx, y + ry, PT_SPRK);
						goto annihilate;
					}

					if (
						!(elements[rt].Properties & PROP_INDESTRUCTIBLE) && !(elements[rt].Properties & TYPE_ENERGY) &&
						rt != PT_AMTR && rt != PT_ANH2 && rt != PT_BCLN && rt != PT_BCLN && rt != PT_BHOL && rt != PT_CLNE &&
						rt != PT_CLNE && rt != PT_EMBR && rt != PT_INVIS && rt != PT_LCRY && rt != PT_NBHL && rt != PT_NWHL &&
						rt != PT_PCLN && rt != PT_PVOD && rt != PT_SING && rt != PT_VOID && rt != PT_WARP && rt != PT_WHOL
					)
					{
						goto annihilate;
					}
					break;
			}
		}
	}

	return 0;

annihilate: // Annihilate with normal matter
	sim->kill_part(i);

	auto ni = sim->create_part(-3, x, y, PT_EMBR);
	if (ni >= 0)
	{
		parts[ni].life = 70;
		parts[ni].temp = MAX_TEMP;
		parts[ni].tmp = 3;
	}

	return 1;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*firea = 15;
	*firer = 58;
	*fireg = 150;
	*fireb = 220;

	*pixel_mode |= FIRE_BLEND;
	return 1;
}
