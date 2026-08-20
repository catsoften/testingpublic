#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_SHPO()
{
	Identifier = "DEFAULT_PT_SHPO";
	Name = "SHPO";
	Colour = 0xEAB5FF_rgb;
	MenuVisible = 1;
	MenuSection = SC_RANDOM;
	Enabled = 1;

	Advection = 0.6f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.98f;
	Loss = 0.95f;
	Collision = 0.0f;
	Gravity = 0.3f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 35;

	HeatConduct = 29;
	Description = "Shampoo. Creates foam, washes off deco color, and cures virus. Resets all properties, dissolves powders.";

	Properties = TYPE_LIQUID | PROP_NEUTPENETRATE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 100.0f + 273.15f;
	HighTemperatureTransition = PT_WTRV;

	Update = &update;
	Graphics = &graphics;

	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * flags: number of empty spaces
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	parts[i].flags = 0; // Track how many empty spaces
	for (auto rx = -2; rx <= 2; rx++)
	{
		for (auto ry = -2; ry <= 2; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					r = sim->photons[y + ry][x + rx];
				}
				if (!r)
				{
					parts[i].flags++;
					continue;
				}
				auto rt = TYP(r);

				if (rt == PT_SHPO) // Ignore shampoo
				{
					continue;
				}

				// Wash off deco color
				parts[ID(r)].dcolour = 0x0;

				// Reset properties to default
				float temp = parts[ID(r)].temp;
				sim->create_part(ID(r), x + rx, y + ry, rt);
				parts[ID(r)].temp = temp;

				// Dissolve powders
				if (elements[rt].Properties & TYPE_PART && sim->rng.chance(1, 2000))
				{
					sim->kill_part(ID(r));
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*pixel_mode |= PMODE_BLUR;

	// Bubbles
	if (cpart->flags > 2 && gfctx.rng.chance(1, 20))
	{
		gfctx.renderer->BlendEllipse({ (int)(cpart->x + 0.5f), (int)(cpart->y + 0.5f) }, { 2, 2 }, (0xBFE4FF_rgb).WithAlpha(255));
	}
	return 0;
}
