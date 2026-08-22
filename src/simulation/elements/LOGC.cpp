#include "simulation/ElementCommon.h"

void Element::Element_LOGC()
{
	Identifier = "DEFAULT_PT_LOGC";
	Name = "LOGC";
	Colour = 0xFFFFFF_rgb;
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
	Description = "Multi-pixel Logic gate. Use PSCN / COPR for input, NSCN / ZINC for output. Tmp = type, pavg0 = output voltage.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 2300.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	//DefaultProperties.pavg[0] = 5.0f;
}
