#include "simulation/ElementCommon.h"
#include "CESM.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_LCSM()
{
	Identifier = "DEFAULT_PT_LCSM";
	Name = "LCSM";
	Colour = 0xBED15E_rgb;
	MenuVisible = 1;
	MenuSection = SC_EXPLOSIVE;
	Enabled = 1;

	Advection = 0.3f;
	AirDrag = 0.02f * CFDS;
	AirLoss = 0.95f;
	Loss = 0.80f;
	Collision = 0.0f;
	Gravity = 0.15f;
	Diffusion = 0.00f;
	HotAir = 0.000001f * CFDS;
	Falldown = 2;

	Flammable = 0;
	Explosive = 0;
	Meltable = 50;
	Hardness = 1;
	PhotonReflectWavelengths = 0x0F30000;

	Weight = 50;

	HeatConduct = 200;
	Description = "Liquid cesium. Like rubidium but sexier.";

	Properties = TYPE_LIQUID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = 28.0f + 273.15f;
	LowTemperatureTransition = PT_CESM;
	HighTemperature = 1100.0f + 273.15f;
	HighTemperatureTransition = PT_FIRE;

	Update = &update;
	Graphics = &graphics;

	DefaultProperties.temp = 36.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	// "Shake" and stop conducting if above 671 C
	if (parts[i].temp > 671.0f + 273.15f)
	{
		parts[i].life = 4;
		parts[i].vx += sim->rng.uniform01() - 0.5f;
		parts[i].vy += sim->rng.uniform01() - 0.5f;
	}

	return Element_CESM_update(UPDATE_FUNC_SUBCALL_ARGS);
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	*pixel_mode |= PMODE_BLUR;
	Element_CESM_graphics(GRAPHICS_FUNC_SUBCALL_ARGS);

	// Glow when hot
	if (cpart->temp > 671.0f + 273.15f)
	{
		*pixel_mode |= FIRE_ADD;

		int strength = (cpart->temp - 671.0f - 273.15f) / 25.0f + 1;
		*firea = std::min(strength * 9, 255);
		*firer = *fireg = *fireb = 255;
	}

	return 0;
}
