#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_MUD()
{
	Identifier = "DEFAULT_PT_MUD";
	Name = "MUD";
	Colour = 0x493804_rgb;
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
	Description = "Mud. Sticky liquid, can be baked into BRCK.";

	Properties = TYPE_LIQUID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 500.0f + 273.15f;
	HighTemperatureTransition = PT_BRCK;

	Update = &update;
	Graphics = &graphics;

	DefaultProperties.temp = 30.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	// "Freeze" if cold or too hot
	if (parts[i].temp < 273.15f || parts[i].temp > 100.0f + 273.15f)
	{
		parts[i].vx = parts[i].vy = 0;
	}

	sim->pv[y / CELL][x / CELL] -= 0.001f;

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->temp > 273.15f && cpart->temp < 100.0f + 273.15f)
	{
		*pixel_mode |= PMODE_BLUR;
	}
	else if (cpart->temp > 100.0f + 273.15f) // Ligthen if hot
	{
		float m = 1 + (cpart->temp - (100.0f + 273.15f)) / 200.0f;
		*colr *= m;
		*colg *= m;
		*colb *= m;
	}

	return 0;
}
