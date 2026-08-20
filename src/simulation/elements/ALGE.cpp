#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_ALGE()
{
	Identifier = "DEFAULT_PT_ALGE";
	Name = "ALGE";
	Colour = 0x70E680_rgb;
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
	Enabled = 1;

	Advection = 0.4f;
	AirDrag = 0.02f * CFDS;
	AirLoss = 0.96f;
	Loss = 0.80f;
	Collision = 0.0f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 20;
	Explosive = 0;
	Meltable = 0;
	Hardness = 30;
	PhotonReflectWavelengths = 0x0007C000;

	Weight = 20;

	HeatConduct = 70;
	Description = "Algae. Grows on the surface of water.";

	Properties = TYPE_PART | PROP_EDIBLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 300.0f + 273.15f;
	HighTemperatureTransition = PT_CRBN;

	FoodValue = 1;

	Update = &update;
	Graphics = &graphics;

	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				// Look for nearby water
				auto r = pmap[y + ry][x + rx];
				auto rt = TYP(r);
				if (sim->rng.chance(1, 100))
				{
					if (elements[rt].Properties & PROP_WATER)
					{
						// Look for empty spot near the water
						for (auto rx2 = -1; rx2 <= 1; rx2++)
						{
							for (auto ry2 = -1; ry2 <= 1; ry2++)
							{
								r = pmap[y + ry + ry2][x + rx + rx2];
								if (!r)
								{
									sim->create_part(-1, x + rx + rx2, y + ry + ry2, PT_ALGE);
									return 0;
								}
							}
						}
					}
				}
				else if (rt == PT_SMKE || rt == PT_CO2)
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_O2);
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	int z = (cpart->tmp - 5) * 16; // Speckles!
	*colr += z;
	*colg += z;
	*colb += z;
	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].tmp = sim->rng.between(0, 6);
}
