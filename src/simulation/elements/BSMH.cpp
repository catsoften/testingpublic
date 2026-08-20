#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

static const std::vector<pixel> BSMH_COLORS({
	0xd3cc71,
	0xd96280,
	0x39ec9b,
	0xe77da4,
	0x3dd9ef,
	0xe0f2bb
});

void Element::Element_BSMH()
{
	Identifier = "DEFAULT_PT_BSMH";
	Name = "BSMH";
	Colour = 0xD0DBDB_rgb;
	MenuVisible = 1;
	MenuSection = SC_SOLIDS;
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

	HeatConduct = 25;
	Description = "Bismuth. Forms square crystals when melted then cooled.";

	Properties = TYPE_SOLID | PROP_CONDUCTS | PROP_LIFE_DEC;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 5.0f;
	HighPressureTransition = PT_BRMT;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = 271.55f + 273.15f;
	HighTemperatureTransition = PT_LAVA;

	Update = &update;
	Graphics = &graphics;

	DefaultProperties.ctype = 1;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties:
	 * ctype = 1 is pure non-crystallized bismuth, otherwise crystal
	 * tmp
	 * 	 0 - Not set yet
	 * 	 1 - Inert crystal
	 * 	 >1 - Growing crystal, max crystal size
	 * tmp2
	 * 	 Crystal growing size, decrements as size grows
	 * tmp3
	 * 	 Used for setting "dark" color bands
	 * tmp4
	 * 	 Used to store color varient
	 */

	auto createCrystalAtPoint = [&sim, &parts, pmap](int x, int y, int tmp2, pixel color)
	{
		if (y < 0 || x < 0 || y >= YRES || x >= XRES)
		{
			return;
		}

		auto r = pmap[y][x];
		if (r && (TYP(r) != PT_BSMH || (TYP(r) == PT_BSMH && parts[ID(r)].tmp > 1)))
		{
			return;
		}

		auto j = (TYP(r) == PT_BSMH && parts[ID(r)].tmp <= 1) ? ID(r) : sim->create_part(-1, x, y, PT_BSMH);
		if (j >= 0)
		{
			parts[j].ctype = 0;
			parts[j].tmp = 1;
			parts[j].tmp3 = tmp2 % 2;
			parts[j].tmp4 = color;
		}
	};

	// Just cooled, set crystal growth state
	if (!parts[i].ctype && !parts[i].tmp)
	{
		parts[i].tmp = sim->rng.chance(1, 30) ? sim->rng.between(2, 14) : 1; // 1 / 30 chance to be a grow start location
		parts[i].tmp4 = BSMH_COLORS[sim->rng.between(0, BSMH_COLORS.size() - 1)];
	}

	// Grow crystal if hot enough and less than max size (20)
	if (parts[i].temp > 126.85f + 273.15f && parts[i].tmp > 1 && parts[i].tmp2 < parts[i].tmp)
	{
		// Top line
		if (y - parts[i].tmp2 >= 0)
		{
			for (auto x2 = x -parts[i].tmp2; x2 <= x + parts[i].tmp2; x2++)
			{
				createCrystalAtPoint(x2, y - parts[i].tmp2, parts[i].tmp2, parts[i].tmp4);
			}
		}

		// Bottom line
		if (y + parts[i].tmp2 < YRES)
		{
			for (auto x2 = x -parts[i].tmp2; x2 <= x + parts[i].tmp2; x2++)
			{
				createCrystalAtPoint(x2, y + parts[i].tmp2, parts[i].tmp2, parts[i].tmp4);
			}
		}

		// Left line
		if (x - parts[i].tmp2 >= 0)
		{
			for (auto y2 = y -parts[i].tmp2; y2 <= y + parts[i].tmp2; y2++)
			{
				createCrystalAtPoint(x - parts[i].tmp2, y2, parts[i].tmp2, parts[i].tmp4);
			}
		}

		// Right line
		if (x + parts[i].tmp2 < XRES)
		{
			for (auto y2 = y -parts[i].tmp2; y2 <= y + parts[i].tmp2; y2++)
			{
				createCrystalAtPoint(x + parts[i].tmp2, y2, parts[i].tmp2, parts[i].tmp4);
			}
		}

		parts[i].tmp2++;

		// Randomly change color again
		if (sim->rng.chance(1, 3))
		{
			parts[i].tmp4 = BSMH_COLORS[sim->rng.between(0, BSMH_COLORS.size() - 1)];
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	if (cpart->tmp4)
	{
		*colr = RGB::Unpack(cpart->tmp4).Red;
		*colg = RGB::Unpack(cpart->tmp4).Blue;
		*colb = RGB::Unpack(cpart->tmp4).Green;
	}

	if (cpart->tmp3)
	{
		*colr *= 0.5f;
		*colg *= 0.5f;
		*colb *= 0.5f;
	}

	return 0;
}
