#include "simulation/ElementCommon.h"
#include "simulation/magnetics/magnetics.h"

//#TPT-Directive ElementClass Element_VOLT PT_VOLT 258
Element_VOLT::Element_VOLT()
{
	Identifier = "DEFAULT_PT_VOLT";
	Name = "VOLT";
	Colour = PIXPACK(0x826108);
	MenuVisible = 1;
	MenuSection = SC_ELECTROMAG;
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
	Meltable = 1;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Voltage battery. Set output voltage with tmp3.";

	Properties = TYPE_SOLID;
	DefaultProperties.tmp3 = 10.0f;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = ITH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &Element_VOLT::update;
	Graphics = &Element_VOLT::graphics;
}

//#TPT-Directive ElementHeader Element_VOLT static int update(UPDATE_FUNC_ARGS)
int Element_VOLT::update(UPDATE_FUNC_ARGS) {
	int r = sim->photons[y][x];

	if (!r || TYP(r) != PT_RSPK) {
		int ni = sim->create_part(-3, x, y, PT_RSPK);
		parts[ni].tmp = 1;
		parts[ni].life = 9;
		parts[ni].tmp3 = parts[i].tmp3;
	}
	else if (TYP(r) == PT_RSPK)
		parts[ID(r)].tmp3 = parts[i].tmp3;

	return 0;
}

//#TPT-Directive ElementHeader Element_VOLT static int graphics(GRAPHICS_FUNC_ARGS)
int Element_VOLT::graphics(GRAPHICS_FUNC_ARGS) {
	return 1;
}

Element_VOLT::~Element_VOLT() {}
