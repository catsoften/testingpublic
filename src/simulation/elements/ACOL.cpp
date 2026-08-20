#include "simulation/ElementCommon.h"
#include "ACOL.h"

void Element::Element_ACOL()
{
	Identifier = "DEFAULT_PT_ACOL";
	Name = "ACOL";
	Colour = 0x222222_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
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
	Hardness = 20;
	PhotonReflectWavelengths = 0x00000000;

	Weight = 100;

	HeatConduct = 200;
	Description = "Anti-Coal, Burns very slowly with CFLM.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 6.0f;
	HighPressureTransition = PT_BACL;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_ACOL_update;
	Graphics = &Element_ACOL_graphics;

	DefaultProperties.life = 110;
	DefaultProperties.tmp = 50;
}

int Element_ACOL_update(UPDATE_FUNC_ARGS)
{
	if (parts[i].life == 0)
	{
		sim->create_part(i, x, y, PT_CFLM);
		return 1;
	}
	else if (parts[i].life < 100)
	{
		parts[i].life--;
		sim->create_part(-1, x + sim->rng.between(-1, 1), y + sim->rng.between(-1, 1), PT_CFLM);
		parts[i].temp = std::max(parts[i].temp - 5.0f, 0.0f);
	}
	else
	{
		for (auto rx = -1; rx <= 1; rx++)
		{
			for (auto ry = -1; ry <= 1; ry++)
			{
				if (rx || ry)
				{
					auto r = pmap[y + ry][x + rx];
					if (TYP(r) == PT_CFLM)
					{
						parts[i].life--;
					}
				}
			}
		}
	}
	return 0;
}

int Element_ACOL_graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->temp < 273.15f)
	{
		int add = 273.15f - cpart->temp;
		*colr = std::min(*colr + add, 255);
		*colg = std::min(*colg + add, 255);
		*colb = std::min(*colb + add, 255);
	}
	return 0;
}
