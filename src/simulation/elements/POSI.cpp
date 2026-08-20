#include "simulation/ElementCommon.h"
#include "ELEC.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_POSI()
{
	Identifier = "DEFAULT_PT_POSI";
	Name = "POSI";
	Colour = 0xFFEFDF_rgb;
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

	HeatConduct = 251;
	Description = "Positrons. Annihilates on contact with electrons, removes spark.";

	Properties = TYPE_ENERGY | PROP_LIFE_DEC | PROP_LIFE_KILL_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &Element_ELEC_graphics;

	Create = &Element_ELEC_create;

	DefaultProperties.temp = R_TEMP + 200.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
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

			switch(rt)
			{
				case PT_GLAS: // Stolen from ELEC
					for (auto rrx = -1; rrx <= 1; rrx++)
					{
						for (auto rry = -1; rry <= 1; rry++)
						{
							if (x + rx + rrx >= 0 && y + ry + rry >= 0 && x + rx + rrx < XRES && y + ry + rry < YRES)
							{
								auto j = sim->create_part(-1, x + rx + rrx, y + ry + rry, PT_EMBR);
								if (j >= 0)
								{
									parts[j].life = 50;
									parts[j].vx = sim->rng.between(-10, 10);
									parts[j].vy = sim->rng.between(-10, 10);
									parts[j].temp = parts[i].temp*0.8f;
									parts[j].tmp = 0;
								}
							}
						}
					}
					sim->kill_part(i);
					return 1;

				case PT_LCRY: // Reset LCRY
					parts[ID(r)].tmp2 = parts[ID(r)].tmp > 1 ? 10 : 0;
					break;

				case PT_PROT: // Repel
					parts[i].vx -= rx;
					parts[i].vy -= ry;
					parts[ID(r)].vx += rx;
					parts[ID(r)].vy += ry;
					break;

				case PT_ELEC: // Turn into 2 PHOT based on kinetic energy
					{
						float totalE = std::hypot(parts[i].vx, parts[i].vy) + std::hypot(parts[ID(r)].vx, parts[ID(r)].vy);
						parts[i].temp += totalE / 2.0f * 700.0f;
						parts[ID(r)].temp += totalE / 2.0f * 700.0f;

						float angle = sim->rng.between(0, 359) / 180.0f * std::numbers::pi_v<float>;
						float vx = totalE / 2.0f * std::cos(angle);
						float vy = totalE / 2.0f * std::sin(angle);

						parts[i].vx = -vx;
						parts[i].vy = -vy;
						parts[ID(r)].vx = vx;
						parts[ID(r)].vy = vy;

						parts[ID(r)].life = parts[i].life = 1000;
						parts[ID(r)].ctype = parts[i].ctype = 0x3F000000 >> std::min(29, (int)totalE);

						sim->part_change_type(i, x, y, PT_PHOT);
						sim->part_change_type(ID(r), x + rx, y + ry, PT_PHOT);

						auto j = sim->create_part(-3, x + rx / 2, y + ry / 2, PT_EMBR);
						if (j >= 0)
						{
							parts[j].life = 70;
							parts[j].tmp = 3;
							parts[j].temp = MAX_TEMP;
						}
						return 1;
					}

				case PT_BTRY:
					if (sim->rng.chance(1, 20))
					{
						sim->part_change_type(ID(r), x + rx, y + ry, PT_BRMT);
					}
					break;

				case PT_NEUT: // Form anti hydrogen
				case PT_APRT:
					sim->part_change_type(ID(r), x + rx, y + ry, PT_ANH2);
					parts[ID(r)].life = 0;
					parts[ID(r)].ctype = 0;
					sim->kill_part(i);
					return 1;

				case PT_EXOT: // Negate EXOT
					parts[ID(r)].life = 1000;
					parts[ID(r)].tmp2 -= 5;
					break;

				case PT_SPRK: // Clear spark
					sim->flood_prop(x + rx, y + ry, AccessProperty( FIELD_LIFE, 0));
					break;

				default:
					break;
			}
		}
	}

	return 0;
}
