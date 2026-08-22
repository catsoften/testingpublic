#include "simulation/ElementCommon.h"

void Element::Element_RSTR()
{
	Identifier = "DEFAULT_PT_RSTR";
	Name = "RSTR";
	Colour = 0xEACE9E_rgb;
	MenuVisible = 1;
	MenuSection = SC_ELECTROMAG;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
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

	HeatConduct = 2;
	Description = "Resistor. Set resistivity / px as pavg0.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 1400.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	//DefaultProperties.pavg[0] = 1000;
}
