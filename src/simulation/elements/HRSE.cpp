#include "simulation/ElementCommon.h"

#include "simulation/Vehicle.h"
#include "ultimata/ElementUtils.h"

constexpr static Vehicle VEHICLE_HORSE = Vehicle{
	25,   // width
	15,   // height
	1.5f, // acceleration
	1.5f, // flyAcceleration
	2.5f, // maxSpeed
	2.0f, // collisionSpeed
	1.5f, // runoverSpeed
	0.1f, // rotationSpeed
};

void Element::Element_HRSE()
{
	Identifier = "DEFAULT_PT_HRSE";
	Name = "HRSE";
	Colour = 0xC27536_rgb;
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
	Enabled = 1;

	Advection = 0.01f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.9999f;
	Collision = -0.1f;
	Gravity = 0.1f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 1;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;

	Weight = 50;

	HeatConduct = 60;
	Description = "Horse. Can be processed into GLUE, STKM can ride it, press down to dismount.";

	Properties = TYPE_PART | PROP_NOCTYPEDRAW | PROP_VEHICLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 346.85f + 273.15f;
	HighTemperatureTransition = PT_FIRE;

	DefaultProperties.life = 100;
	DefaultProperties.temp = R_TEMP + 14.6f + 273.15f;
	DefaultProperties.tmp4 = FromFloat(std::numbers::pi_v<float> / 4); // About 45 deg upwards default neck rotation
}
