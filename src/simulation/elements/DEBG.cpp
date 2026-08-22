#include "simulation/ElementCommon.h"

static void create(ELEMENT_CREATE_FUNC_ARGS);
static int update(UPDATE_FUNC_ARGS);

void Element::Element_DEBG()
{
	Identifier = "DEFAULT_PT_DEBG";
	Name = "DEBG";
	Colour = 0xFF0000_rgb;
	MenuVisible = 1;
	MenuSection = SC_RANDOM;
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
	Hardness = 0;

	Weight = 100;

	HeatConduct = 0;
	Description = "Debugger (for mod testing lol). Set tmp to change debugging mode.";

	Properties = TYPE_SOLID | PROP_INDESTRUCTIBLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;

	Create = &create;
}

static int update(UPDATE_FUNC_ARGS)
{
	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{

}
