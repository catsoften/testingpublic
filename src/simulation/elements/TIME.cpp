#include "simulation/ElementCommon.h"
#include "simulation/orbitalparts.h"
#include "CBMB.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_TIME()
{
	Identifier = "DEFAULT_PT_TIME";
	Name = "TIME";
	Colour = 0x3A695A_rgb;
	MenuVisible = 1;
	MenuSection = SC_POWERED;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.95f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 10;

	Weight = 100;

	HeatConduct = 0;
	Description = "Time Dilation Field Generator. HEAT = Faster, COOL = Slower";

	Properties = TYPE_SOLID | PROP_NO_TIME;

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

	DefaultProperties.temp = -16.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	// Let every 16 degrees = 1 level
	if (parts[i].temp < 273.15f + 16 * MIN_TIME_DILATION)
	{
		parts[i].temp = 273.15f + 16 * MIN_TIME_DILATION;
	}
	else if (parts[i].temp > 273.15f + 16 * MAX_TIME_DILATION)
	{
		parts[i].temp = 273.15f + 16 * MAX_TIME_DILATION;
	}

	// Generate the field
	if (parts[i].tmp2 > 0)
	{
		Element_CBMB_time_dilation(sim, x, y, 15, (parts[i].temp - 273.15f) / 16.0f);
	}

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			auto r = pmap[y + ry][x + rx];
			if (!r)
			{
				continue;
			}

			auto rt = TYP(r);

			// Turn on
			if (rt == PT_SPRK && parts[ID(r)].ctype == PT_PSCN)
			{
				sim->flood_prop(parts[i].x, parts[i].y, AccessProperty{ FIELD_TMP2, 10 });
			}

			// Turn off
			if (rt == PT_SPRK && parts[ID(r)].ctype == PT_NSCN)
			{
				sim->flood_prop(parts[i].x, parts[i].y, AccessProperty{ FIELD_TMP2, 0 });
			}
		}
	}

	// For some reason this is needed for graphics, this isn't calculated
	// Stolen from PRTI
	int orbd[4] = { 0, 0, 0, 0 }; // Orbital distances
	int orbl[4] = { 0, 0, 0, 0 }; // Orbital locations

	if (!sim->parts[i].life)
	{
		parts[i].life = sim->rng.gen();
	}

	if (!sim->parts[i].ctype)
	{
		parts[i].ctype = sim->rng.gen();
	}

	orbitalparts_get(parts[i].life, parts[i].ctype, orbd, orbl);

	for (int r = 0; r < 4; r++)
	{
		if (orbd[r] > 1)
		{
			orbd[r] -= 12;

			if (orbd[r] < 1)
			{
				orbd[r] = sim->rng.between(128, 255);
				orbl[r] = sim->rng.between(0, 254);
			}
			else
			{
				orbl[r] += 2;
				orbl[r] = orbl[r] % 255;
			}
		}
		else
		{
			orbd[r] = sim->rng.between(128, 255);
			orbl[r] = sim->rng.between(0, 254);
		}
	}

	orbitalparts_set(&parts[i].life, &parts[i].ctype, orbd, orbl);

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	float m = cpart->tmp2 / 10.0f + 1.0f;
	*colr *= m;
	*colg *= m;
	*colb *= m;

	if (cpart->tmp2 > 0)
	{
		if (cpart->temp > 273.15f)
		{
			*pixel_mode |= EFFECT_GRAVOUT;
		}
		else
		{
			*pixel_mode |= EFFECT_GRAVIN;
		}

		*pixel_mode &= ~PMODE;
		*pixel_mode |= PMODE_ADD;
	}

	return 0;
}
