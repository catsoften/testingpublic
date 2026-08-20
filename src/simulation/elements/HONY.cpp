#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_HONY()
{
	Identifier = "DEFAULT_PT_HONY";
	Name = "HONY";
	Colour = 0xFF9900_rgb;
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
	Enabled = 1;

	Advection = 0.6f;
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

	Weight = 35;

	HeatConduct = 29;
	Description = "Honey. Sweet liquid produced by BEEs.";

	Properties = TYPE_LIQUID | PROP_EDIBLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 71.0f + 273.15f;
	HighTemperatureTransition = PT_SWTR;

	FoodValue = 5;

	Update = &update;
	Graphics = &graphics;

	DefaultProperties.temp = 30.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	float stickx = 0.0f, sticky = 0.0f;

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

				if (rt == PT_WAX) // Dont move if near wax to avoid leaking from honeycombs
				{
					parts[i].vx = parts[i].vy = 0.0f;
				}
				else if (rt == PT_BCTR && sim->rng.chance(1, 1000)) // Slowly kill BCTR
				{
					sim->kill_part(ID(r));
				}
				else if (rt == PT_HONY || elements[rt].Properties & TYPE_SOLID) // "Stick" to solids and itself
				{
					stickx += rx;
					sticky += ry;
				}
			}
		}
	}

	// Move torwards stick direction
	parts[i].vx += stickx / 5.0f;
	parts[i].vy += sticky / 5.0f;

	// Freeze if cold
	if (parts[i].temp < -20.0f + 273.15f)
	{
		parts[i].vx = parts[i].vy = 0.0f;
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->temp >= -20.0f + 273.15f) // Looks like liquid if not cold
	{
		*pixel_mode |= PMODE_BLUR;
	}
	return 0;
}
