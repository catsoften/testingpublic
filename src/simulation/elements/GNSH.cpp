#include "simulation/ElementCommon.h"

#include "simulation/Vehicle.h"

constexpr static Vehicle VEHICLE_GUNSHIP = Vehicle{
	70,    // width
	30,    // height
	0.05f, // acceleration
	0.4f,  // flyAcceleration
	2.5f,  // maxSpeed
	55.0f, // collisionSpeed
	1.1f,  // runoverSpeed
	0.1f,  // rotationSpeed
};

void Element::Element_GNSH()
{
	Identifier = "DEFAULT_PT_GNSH";
	Name = "GNSH";
	Colour = 0x8FA7B3_rgb;
	MenuVisible = 1;
	MenuSection = SC_RANDOM;
	Enabled = 1;

	Advection = 0.01f;
	AirDrag = 0.04f * CFDS;
	AirLoss = 0.94f;
	Loss = 0.95f;
	Collision = -0.1f;
	Gravity = 0.07f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 10;
	Description = "UEF T3 Heavy Gunship. Now rideable! UP = Shoot, DOWN = Fly, L + R + D = exit";

	Properties = TYPE_PART | PROP_NOCTYPEDRAW | PROP_VEHICLE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	DefaultProperties.life = 5000;
}
