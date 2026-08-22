#include "simulation/ElementCommon.h"

void Element::Element_TRST()
{
	Identifier = "DEFAULT_PT_TRST";
	Name = "TRST";
	Colour = 0xE09972_rgb;
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
	Description = "Thermoresistor, resistance is pavg0 + pavg1 * temp.";

	Properties = TYPE_SOLID | PROP_CONDUCTS;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 3000.0f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	//DefaultProperties.pavg[0] = 1000.0f;
	//DefaultProperties.pavg[1] = 10.0f;
}
