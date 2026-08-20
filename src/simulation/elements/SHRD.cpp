#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_SHRD()
{
	Identifier = "DEFAULT_PT_SHRD";
	Name = "SHRD";
	Colour = 0x4A443D_rgb;
	MenuVisible = 1;
	MenuSection = SC_POWERED;
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
	Meltable = 0;
	Hardness = 1;

	Weight = 100;

	HeatConduct = 251;
	Description = "Shredder. Breaks solids and certain powders when they enter.";

	Properties = TYPE_SOLID | PROP_DEADLY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = IPH;
	HighPressureTransition = NT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * life - is powered?
	 * tmp  - Used for animation
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	if (parts[i].life)
	{
		parts[i].tmp = (parts[i].tmp + 1) % 5;
	}

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			auto r = pmap[y + ry][x + rx];
			if (!r || TYP(r) == PT_SHRD)
			{
				continue;
			}
			auto rt = TYP(r);

			// Toggle on off
			if (rt == PT_SPRK)
			{
				if (parts[ID(r)].ctype == PT_PSCN)
				{
					sim->flood_prop(x, y, AccessProperty{ FIELD_LIFE, 10 });
				}
				else if (parts[ID(r)].ctype == PT_NSCN)
				{
					sim->flood_prop(x, y, AccessProperty{ FIELD_LIFE, 0 });
				}
			}

			// Shred stuff!
			// These elements have existing breaks and dont need to be turned into BRKN
			if (parts[i].life)
			{
				int changeType = 0;
				switch (rt)
				{

					case PT_AERO:
						changeType = PT_GEL;
						break;

					case PT_ANT:
					case PT_BEE:
					case PT_SPDR:
						changeType = PT_BCTR;
						break;

					case PT_ARAY:
					case PT_CRAY:
					case PT_DRAY:
					case PT_EMP:
					case PT_SWCH:
					case PT_WIFI:
						changeType = PT_BREC;
						break;

					case PT_BIRD:
					case PT_FISH:
						changeType = PT_BLOD;
						parts[ID(r)].life = 100;
						break;

					case PT_BMTL:
					case PT_IRON:
					case PT_METL:
					case PT_MMSH:
						changeType = PT_BRMT;
						break;

					case PT_CNCT:
						changeType = PT_STNE;
						break;

					case PT_COAL:
						changeType = PT_BCOL;
						break;

					case PT_CRMC:
						changeType = PT_CLST;
						break;

					case PT_FIBR:
					case PT_GLAS:
						changeType = PT_BGLA;
						break;

					case PT_FUSE:
						changeType = PT_FSEP;
						break;

					case PT_PLNT:
					case PT_VINE:
						changeType = PT_SEED;
						break;

					case PT_RCRT:
						changeType = PT_CNCT;
						break;

					case PT_VIBR:
						changeType = PT_BVBR;
						break;

					case PT_WOOD:
						changeType = PT_SAWD;
						break;

					// Special elements that pretend to be solid
					case PT_CRBN:
						parts[ID(r)].tmp2 = 0;
						break;

					case PT_RCK:
						parts[ID(r)].tmp2 = 1;
						break;
				}

				// Can break under high pressure
				if (!changeType && elements[rt].HighPressureTransition > 0)
				{
					changeType = elements[rt].HighPressureTransition;
				}

				if (changeType)
				{
					sim->part_change_type(ID(r), x + rx, y + ry, changeType);
				}

				// We wish to shred some solids without a broken state
				// so we change it to BRKN (ctype the solid)
				if (!changeType)
				{
					if (elements[rt].Properties & PROP_INDESTRUCTIBLE)
					{
						continue;
					}

					switch (rt)
					{
						case PT_SAWD:
							sim->part_change_type(ID(r), x + rx, y + ry, PT_BRKN);
							parts[ID(r)].ctype = PT_SAWD;
							break;

						case PT_MONY:
						case PT_PAPR:
							sim->part_change_type(ID(r), x + rx, y + ry, PT_BRKN);
							parts[ID(r)].ctype = PT_PAPR;
							break;
					}

					// Shred conductable solids
					if (TYP(r) != PT_BRKN && elements[rt].Properties & PROP_CONDUCTS && elements[rt].Properties & TYPE_SOLID && rt != PT_NSCN && rt != PT_PSCN)
					{
						sim->part_change_type(ID(r), x + rx, y + ry, PT_BRKN);
						parts[ID(r)].ctype = rt;
					}
				}
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	int r = (nx / 2 + ny / 2 + cpart->tmp) % 5;
	*colr -= r * 10;
	*colg -= r * 10;
	*colb -= r * 10;

	return 0;
}
