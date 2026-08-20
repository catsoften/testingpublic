#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);

void Element::Element_PDTC()
{
	Identifier = "DEFAULT_PT_PDTC";
	Name = "PDTC";
	Colour = 0xFDA118_rgb;
	MenuVisible = 1;
	MenuSection = SC_SENSOR;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 0.96f;
	Loss = 0.00f;
	Collision = 0.0f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 0;
	Description = "Photon Detector, outputs to WIFI channels depending on wavelength, floodfills nearby FILT";

	Properties = TYPE_SOLID;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;

	DefaultProperties.temp = 1.0f + 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Props:
	 * tmp:
	 * 0: Default behavior
	 * 1: Don't output to WIFI
	 * 2: Don't output to FILT
	 *
	 * tmp2:
	 * 0: Kill photon
	 * 1: Don't kill photon
	 *
	 * temp: delay before next measurement (in frames in C, < 0 = 0)
	 * life: delay counter
	 */

	if (parts[i].life)
	{
		parts[i].life--;
		return 0;
	}

	for (auto rx = -2; rx <= 2; rx++)
	{
		for (auto ry = -2; ry <= 2; ry++)
		{
			if (rx || ry)
			{
				auto r = sim->photons[y + ry][x + rx];
				if (!r)
				{
					continue;
				}
				auto rt = TYP(r);

				if (rt == PT_PHOT)
				{
					parts[i].life = parts[i].temp - 273.15f;

					if (parts[i].tmp != 1)
					{
						auto channel = CHANNELS * sim->faradayMap[y / CELL][x / CELL] + parts[ID(r)].ctype % 101 + 1;
						sim->wireless[channel][1] = 1;
						sim->ISWIRE = 2;
					}

					if (parts[i].tmp != 2)
					{
						for (auto rx = -1; rx <= 1; rx++)
						{
							for (auto ry = -1; ry <= 1; ry++)
							{
								if (rx || ry)
								{
									auto r2 = pmap[y + ry][x + rx];
									if (r2 && TYP(r2) == PT_FILT)
									{
										sim->flood_prop(x + ry, y + ry, AccessProperty{ FIELD_CTYPE, parts[ID(r)].ctype });
										goto foundFILT;
									}
								}
							}
						}
					}
foundFILT:

					// Kill photon if allowed
					if (!parts[i].tmp2)
					{
						sim->kill_part(ID(r));
					}

					return 0;
				}
			}
		}
	}

	return 0;
}
