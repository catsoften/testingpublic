#include "simulation/ElementCommon.h"

void Element::Element_CAPR()
{
	Identifier = "DEFAULT_PT_CAPR";
	Name = "CPTR";
	Colour = 0x82A5CF_rgb;
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
	Description = "Polarized electrolytic capacitor. Stores and releases charge. pavg0 = capacitance.";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 999.85f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	//DefaultProperties.pavg[0] = 0.1f;
}
