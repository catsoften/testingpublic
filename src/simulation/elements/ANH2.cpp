#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_ANH2()
{
	Identifier = "DEFAULT_PT_ANH2";
	Name = "ANH2";
	Colour = 0xBD3335_rgb;
	MenuVisible = 1;
	MenuSection = SC_NUCLEAR;
	Enabled = 1;

	Advection = 2.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.99f;
	Loss = 0.30f;
	Collision = -0.1f;
	Gravity = 0.0f;
	Diffusion = 3.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 1;

	HeatConduct = 251;
	Description = "Anti-Hydrogen. Annihilates ordinary matter, undergoes fusion at high temperature and pressure.";

	Properties = TYPE_GAS | PROP_PHOTPASS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	for (auto rx = -2; rx <= -2; rx++)
	{
		for (auto ry = -2; ry <= -2; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					continue;
				}
				auto rt = TYP(r);

				// Stolen from HYGN
				if (sim->pv[y / CELL][x / CELL] > 45.0f)
				{
					if (parts[ID(r)].temp > 2273.15f)
					{
						continue;
					}
				}
				else
				{
					if (rt == PT_FIRE)
					{
						if (parts[ID(r)].tmp & 0x02)
						{
							parts[ID(r)].temp = 3473.0f;
						}
						else
						{
							parts[ID(r)].temp = 2473.15f;
						}
						parts[ID(r)].tmp |= 1;

						auto j = sim->create_part(i, x, y,PT_FIRE);
						if (j >= 0)
						{
							parts[j].life = sim->rng.between(10, 120);
						}

						parts[i].temp += sim->rng.between(0, 99);
						parts[i].tmp |= 1;

						return 1;
					}
					else if ((rt == PT_PLSM && !(parts[ID(r)].tmp & 4)) || (rt == PT_LAVA && parts[ID(r)].ctype != PT_BMTL))
					{
						auto j = sim->create_part(i, x, y,PT_FIRE);
						if (j >= 0)
						{
							parts[j].life = sim->rng.between(10, 120);
						}

						parts[i].temp += sim->rng.between(0, 99);
						parts[i].tmp |= 1;

						return 1;
					}

					// Destroy on contact with ordinary matter
					if (
						!(elements[rt].Properties & PROP_INDESTRUCTIBLE) && rt != PT_AMTR && rt != PT_ANH2 && rt != PT_BCLN && rt != PT_BHOL &&
						rt != PT_FIRE && rt != PT_NBHL && rt != PT_PBCN && rt != PT_PCLN && rt != PT_PLSM && rt != PT_PRTI && rt != PT_PRTO && rt != PT_VOID
					)
					{
						sim->pv[y / CELL][x / CELL] += 10.0f;
						parts[ID(r)].temp += 9000.0f;

						int count = sim->rng.between(4, 50);
						for (auto j = 0; j < count; j++)
						{
							auto k = sim->create_part(-3, x, y, PT_PHOT);
							if (k >= 0)
							{
								parts[k].temp = MAX_TEMP;
								parts[k].life = sim->rng.between(0, 299);

								float angle = sim->rng.uniform01() * 2.0f * std::numbers::pi_v<float>;
								float v = sim->rng.uniform01() * 5.0f;
								parts[k].vx = v * std::cos(angle);
								parts[k].vy = v * std::sin(angle);
							}
						}

						sim->kill_part(ID(r));
						sim->kill_part(i);

						return 1;
					}
				}
			}
		}
	}

	// Stolen from hydrogen
	if (parts[i].temp > 2273.15f && sim->pv[y / CELL][x / CELL] > 50.0f)
	{
		if (sim->rng.chance(1, 5))
		{
			float temp = parts[i].temp;
			sim->create_part(i, x, y, PT_AMTR);
			parts[i].tmp2 = 1;

			auto j = sim->create_part(-3, x, y, PT_NEUT);
			if (j >= 0)
			{
				parts[j].temp = temp;
			}

			if (sim->rng.chance(1, 10))
			{
				j = sim->create_part(-3, x, y, PT_POSI);
				if (j >= 0)
				{
					parts[j].temp = temp;
				}
			}

			j = sim->create_part(-3, x, y, PT_PHOT);
			if (j >= 0)
			{
				parts[j].ctype = 0x7C0000;
				parts[j].temp = temp;
				parts[j].tmp = 0x1;
			}

			int rx = x + sim->rng.between(-1, 1);
			int ry = y + sim->rng.between(-1, 1);
			auto rt = TYP(pmap[ry][rx]);
			if (sd.can_move[PT_PLSM][rt] || rt == PT_ANH2)
			{
				j = sim->create_part(-3, rx, ry, PT_PLSM);
				if (j >= 0)
				{
					parts[j].temp = temp;
					parts[j].tmp |= 4;
				}
			}

			parts[i].temp = temp + sim->rng.between(750, 1249);
			sim->pv[y / CELL][x / CELL] += 30.0f;

			return 1;
		}
	}

	return 0;
}
