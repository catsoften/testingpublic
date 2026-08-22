#include "simulation/ElementCommon.h"

void Element::Element_IOSL()
{
	Identifier = "DEFAULT_PT_IOSL";
	Name = "IOSL";
	Colour = 0x4150E8_rgb;
	MenuVisible = 1;
	MenuSection = SC_ELECTROMAG;
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

	Weight = 30;

	HeatConduct = 29;
	Description = "Ionic solution. ";

	Properties = TYPE_LIQUID | PROP_CONDUCTS | PROP_PHOTPASS | PROP_LIFE_DEC | PROP_NEUTPASS | PROP_WATER;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = 273.15f;
	LowTemperatureTransition = PT_ICEI;
	HighTemperature = 99.85f + 273.15f;
	HighTemperatureTransition = PT_WTRV;

	DefaultProperties.temp = R_TEMP - 2.0f + 273.15f;
}
