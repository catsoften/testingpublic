#include "simulation/ElementCommon.h"

#include "FILT.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_PFLT()
{
	Identifier = "DEFAULT_PT_PFLT";
	Name = "PFLT";
	Colour = 0x000056_rgb;
	MenuVisible = 1;
	MenuSection = SC_POWERED;
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

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Powered filter. Toggle with PSCN / NSCN, use COPR / ZINC to change tmp.";

	Properties = TYPE_SOLID | PROP_NOAMBHEAT;

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

	Create = &Element_FILT_create;

	DefaultProperties.life = 10;
}

static int update(UPDATE_FUNC_ARGS)
{
	if (parts[i].tmp2)
	{
		parts[i].tmp2--;
	}

	for (auto rx = -2; rx <= 2; rx++)
	{
		for (auto ry = -2; ry <= 2; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					continue;
				}

				if (TYP(r) == PT_SPRK)
				{
					if (parts[ID(r)].ctype == PT_PSCN && !parts[i].life)
					{
						sim->flood_prop(x, y, AccessProperty{ FIELD_LIFE, 10 });
					}
					else if (parts[ID(r)].ctype == PT_NSCN && parts[i].life)
					{
						sim->flood_prop(x, y, AccessProperty{ FIELD_LIFE, 0 });
					}
					else if (!parts[i].tmp2)
					{
						if (parts[ID(r)].ctype == PT_COPR && parts[ID(r)].life == 3)
						{
							sim->flood_prop(x, y, AccessProperty{ FIELD_TMP, std::min(parts[i].tmp + 1, 100) });
							sim->flood_prop(x, y, AccessProperty{ FIELD_TMP2, 2 });
						}
						else if (parts[ID(r)].ctype == PT_ZINC && parts[ID(r)].life == 3)
						{
							sim->flood_prop(x, y, AccessProperty{ FIELD_TMP, std::max(0, parts[i].tmp - 1) });
							sim->flood_prop(x, y, AccessProperty{ FIELD_TMP2, 2 });
						}
					}
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	Element_FILT_graphics(GRAPHICS_FUNC_SUBCALL_ARGS);

	if (!cpart->life)
	{
		*colr *= 0.2f;
		*colg *= 0.2f;
		*colb *= 0.2f;
	}

	return 0;
}
