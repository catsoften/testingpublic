#include "simulation/ElementCommon.h"

#include <set>

constexpr int FFGN_MAX_LINE_DIS = 100 * 100; // 100 px max FFGN connection, 100^2 = 100 * 100

// Keep track when to clear
static unsigned int previousFrameCount = 0;
static std::set<int> lineGenerators;

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_FFGN()
{
	Identifier = "DEFAULT_PT_FFGN";
	Name = "FFGN";
	Colour = 0x7F6600_rgb;
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

	HeatConduct = 0;
	Description = "Force field generator. HEAT / COOL = size, tmp = shape, tmp2 = type. Use sparingly.";

	Properties = TYPE_SOLID | PROP_NOAMBHEAT | PROP_NOCTYPEDRAW;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 100.8f;
	HighPressureTransition = PT_BREC;
	LowTemperature = ITL;
	LowTemperatureTransition = NT;
	HighTemperature = ITH;
	HighTemperatureTransition = NT;

	Update = &update;
	Graphics = &graphics;

	CtypeDraw = &Element::ctypeDrawVInTmp;

	DefaultProperties.temp = 100.0f;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Props:
	 * temp: radius / size
	 * ctype: type to affect
	 * life: on / off state
	 * tmp: shape
	 *   0: circle
	 *   1: square
	 *   2: connect to nearest FFGN
	 * tmp2: type
	 *   See FFLD.cpp
	 * tmp3: ring distance
	 * flags:
	 * 	  How many FFGN surrounds it, used for ring graphics
	 */

	auto floodFillFFLD = [&sim, &parts, i](int x, int y) {
		if (x < 0 || y < 0 || x >= XRES || y >= YRES)
		{
			return;
		}

		sim->flood_prop(x, y, AccessProperty{ FIELD_TMP, parts[i].tmp });
		sim->flood_prop(x, y, AccessProperty{ FIELD_CTYPE, parts[i].ctype });
		sim->flood_prop(x, y, AccessProperty{ FIELD_DCOLOUR, parts[i].dcolour });
	};

	// Every 10 frames recalculate
	if (sim->currentTick % 100 == 0 && previousFrameCount != (unsigned int)sim->currentTick)
	{
		previousFrameCount = sim->currentTick;
		lineGenerators.clear();
	}

	// Add self to set
	if (parts[i].tmp == 2)
	{
		lineGenerators.insert(i);
	}

	if (++parts[i].tmp3 >= 100)
	{
		parts[i].tmp3 = 0;
	}

	parts[i].flags = 0; // Reset every frame

	if (parts[i].life)
	{
		if (parts[i].tmp == 0) // Circle
		{
			int prevx = -1, prevy = -1;
			for (auto angle = 0.0f; angle < 2.0f * std::numbers::pi_v<float>; angle += 0.001f) // 0.001 might be a little small
			{
				int x1 = std::sin(angle) * parts[i].temp + parts[i].x;
				int y1 = std::cos(angle) * parts[i].temp + parts[i].y;

				if (x1 == prevx && y1 == prevy)
				{
					continue;
				}

				auto j = sim->create_part(-1, x1, y1, PT_FFLD);
				if (j >= 0)
				{
					parts[j].ctype = parts[i].ctype;
					parts[j].tmp2 = parts[i].tmp2;
					parts[j].dcolour = parts[i].dcolour;
				}
				prevx = x1, prevy = y1;
			}
		}
		else if (parts[i].tmp == 1) // Square
		{
			sim->CreateBox(-1, x - parts[i].temp, y - parts[i].temp, x + parts[i].temp, y - parts[i].temp, PT_FFLD, 0);
			sim->CreateBox(-1, x - parts[i].temp, y + parts[i].temp, x + parts[i].temp, y + parts[i].temp, PT_FFLD, 0);
			sim->CreateBox(-1, x - parts[i].temp, y - parts[i].temp, x - parts[i].temp, y + parts[i].temp, PT_FFLD, 0);
			sim->CreateBox(-1, x + parts[i].temp, y - parts[i].temp, x + parts[i].temp, y + parts[i].temp, PT_FFLD, 0);

			// Find a middle of a side to floodfill from
			floodFillFFLD(x, y + parts[i].temp);
			floodFillFFLD(x, y - parts[i].temp);
			floodFillFFLD(x + parts[i].temp, y);
			floodFillFFLD(x - parts[i].temp, y);
		}
		else if (parts[i].tmp == 2 && lineGenerators.size() >= 2) // Electrode like behavior!
		{
			// Find nearest 2 neighbours and draw lines to them
			int dis1 = 999999999, dis2 = 999999999;
			int id1 = -1, id2 = -1;
			for (auto j : lineGenerators)
			{
				if (j >= i)
				{
					continue;
				}

				int dis = std::hypot(parts[j].x - parts[i].x, parts[j].y - parts[i].y);
				if (dis < dis1)
				{
					id1 = j;
					dis1 = dis;
				}
				else if (dis < dis2)
				{
					id2 = j;
					dis2 = dis;
				}
			}

			// Only draw if within 100 px (100^2 = 10000)
			if (id1 != -1 && i > id1 && dis1 < FFGN_MAX_LINE_DIS)
			{
				sim->CreateLine(x, y, parts[id1].x, parts[id1].y, PT_FFLD);
			}
			if (id2 != -1 && i > id2 && dis2 < FFGN_MAX_LINE_DIS)
			{
				sim->CreateLine(x, y, parts[id2].x, parts[id2].y, PT_FFLD);
			}

			floodFillFFLD(x + 1, y);
			floodFillFFLD(x - 1, y);
			floodFillFFLD(x, y - 1);
			floodFillFFLD(x, y + 1);
		}
	}

	// Check for ON / OFF
	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			auto r = pmap[y + ry][x + rx];
			if (!r)
			{
				continue;
			}
			auto rt = TYP(r);

			if (rt == PT_FFGN)
			{
				parts[i].flags++;
			}
			else if (rt == PT_SPRK && (parts[ID(r)].ctype == PT_PSCN || parts[ID(r)].ctype == PT_NSCN))
			{
				sim->flood_prop(parts[i].x, parts[i].y, AccessProperty{ FIELD_LIFE, parts[ID(r)].ctype == PT_PSCN ? 1 : 0 });
			}
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	// Lighter if activated
	if (cpart->life > 0)
	{
		*colr *= 2;
		*colg *= 2;
		*colb *= 2;

		// Fancy rings emitting from the generator
		// Ring lasts 100 frames
		// Only draw if 3 or less surround generators
		if (cpart->flags <= 3)
		{
			float multi = cpart->tmp3 / 100.0f;
			if (cpart->tmp == 0) // Circle
			{
				for (float angle = 0.0f; angle < 2.0f * std::numbers::pi_v<float>; angle += 0.06f)
				{
					int x1 = multi * std::sin(angle) * cpart->temp + cpart->x;
					int y1 = multi * std::cos(angle) * cpart->temp + cpart->y;
					int x2 = multi * std::sin(angle + 0.06f) * cpart->temp + cpart->x;
					int y2 = multi * std::cos(angle + 0.06f) * cpart->temp + cpart->y;
					gfctx.renderer->DrawLine({ x1, y1 }, { x2, y2 }, RGB(80, 80, 100));
				}
			}
			else if (cpart->tmp == 1) // Square
			{
				int top = cpart->y + cpart->temp * multi;
				int bottom = cpart->y - cpart->temp * multi;
				int left = cpart->x - cpart->temp * multi;
				int right = cpart->x + cpart->temp * multi;

				gfctx.renderer->DrawLine({ left, bottom }, { left, top }, RGB(80, 80, 100));
				gfctx.renderer->DrawLine({ right, bottom }, { right, top }, RGB(80, 80, 100));
				gfctx.renderer->DrawLine({ right, bottom }, { left, bottom }, RGB(80, 80, 100));
				gfctx.renderer->DrawLine({ right, top }, { left, top }, RGB(80, 80, 100));
			}
		}
	}

	return 0;
}
