#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_SNOW()
{
	Identifier = "DEFAULT_PT_SNOW";
	Name = "SNOW";
	Colour = 0xC0E0FF_rgb;
	MenuVisible = 1;
	MenuSection = SC_POWDERS;
	Enabled = 1;

	Advection = 0.7f;
	AirDrag = 0.01f * CFDS;
	AirLoss = 0.96f;
	Loss = 0.90f;
	Collision = -0.1f;
	Gravity = 0.05f;
	Diffusion = 0.01f;
	HotAir = -0.00005f* CFDS;
	Falldown = 1;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 20;
	PhotonReflectWavelengths = 0x03FFFFFF;

	Weight = 50;

	DefaultProperties.temp = R_TEMP - 30.0f + 273.15f;
	HeatConduct = 46;
	Description = "Light particles. Created when ICE breaks under pressure.";

	Properties = TYPE_PART|PROP_NEUTPASS;
	CarriesTypeIn = 1U << FIELD_CTYPE;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 252.05f;
	HighTemperatureTransition = ST;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	// Fix for liquid melting, also update in ICEI
	if (
		parts[i].ctype >= 0 && parts[i].ctype < PT_NUM && elements[parts[i].ctype].Enabled &&
		elements[parts[i].ctype].LowTemperatureTransition == PT_LQUD && parts[i].temp > elements[parts[i].ctype].MeltingPoint
	)
	{
		sim->part_change_type(i, x, y, PT_LQUD);
		return 1;
	}

	if (parts[i].ctype==PT_FRZW)//get colder if it is from FRZW
	{
		parts[i].temp = restrict_flt(parts[i].temp-1.0f, MIN_TEMP, MAX_TEMP);
	}
	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y+ry][x+rx];
				if (!r)
					continue;
				//@ SNOW + SALT/SLTW -> 2xSLTW
				if ((TYP(r)==PT_SALT || TYP(r)==PT_SLTW) && sim->rng.chance(1, 333))
				{
					sim->part_change_type(i,x,y,PT_SLTW);
					sim->part_change_type(ID(r),x+rx,y+ry,PT_SLTW);
				}
			}
		}
	}
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	if (cpart->ctype >= 0 && cpart->ctype < PT_NUM && elements[cpart->ctype].Enabled && elements[cpart->ctype].FrozenGraphics)
	{
		auto color = elements[cpart->ctype].Colour;

		*colr = color.Red;
		*colg = color.Green;
		*colb = color.Blue;

		return elements[cpart->ctype].FrozenGraphics(GRAPHICS_FUNC_SUBCALL_ARGS);
	}

	return 0;
}
