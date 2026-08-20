#include "simulation/ElementCommon.h"

#include "ultimata/ElementUtils.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_TRBN()
{
	Identifier = "DEFAULT_PT_TRBN";
	Name = "TRBN";
	Colour = 0xDDDDDD_rgb;
	MenuVisible = 1;
	MenuSection = SC_SENSOR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 0;
	Description = "Turbine. Makes SPRK when in moving air or particles. tmp3 = air velocity, tmp4 = particle velocity.";

	Properties = TYPE_SOLID | PROP_NOCTYPEDRAW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 2000.0f + 273.15f;
	HighTemperatureTransition = PT_METL;

	Update = &update;
	Graphics = &graphics;

	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	// tmp = Graphics iteration

	float airSpeed = std::hypot(sim->vx[y / CELL][x / CELL], sim->vy[y / CELL][x / CELL]);
	bool fastPart = airSpeed > ToFloat(sim->parts[i].tmp3);
	bool alreadyAnimate = false;

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

				float partSpeed = std::hypot(parts[ID(r)].vx, parts[ID(r)].vy);
				if (partSpeed > ToFloat(sim->parts[i].tmp4))
				{
					fastPart = true;
				}

				if (fastPart && !alreadyAnimate)
				{
					parts[i].tmp++; // Graphics
					alreadyAnimate = true;
				}

				// Shred birds
				if (fastPart && TYP(r) == PT_BIRD)
				{
					sim->part_change_type(ID(r), x + rx, y + ry, PT_BLOD);
				}

				if (fastPart && elements[TYP(r)].Properties & PROP_CONDUCTS)
				{
					parts[ID(r)].life = 4;
					parts[ID(r)].ctype = TYP(r);
					sim->part_change_type(ID(r), x + rx, y + ry, PT_SPRK);
					return 0;
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	float m = 0.4f + ((cpart->tmp + nx + ny) % 6) / 5.0f * 0.6f;

	*colr *= m;
	*colg *= m;
	*colb *= m;

	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].tmp3 = FromFloat(0.3f);
	sim->parts[i].tmp4 = FromFloat(1.0f);
}
