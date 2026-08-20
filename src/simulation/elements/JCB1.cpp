#include "simulation/ElementCommon.h"

#include <array>
#include <vector>

#include "Format.h"
#include "common/Plane.h"
#include "graphics/Pixel.h"
#include "graphics/Renderer.h"
#include "ultimata/ElementUtils.h"

#include "JCB1_png.h"

std::unique_ptr<PlaneAdapter<std::vector<pixel_rgba>>> JCB1Image = format::PixelsFromPNG(JCB1_png.AsCharSpan());

std::array<std::pair<String, RGB>, 10> quotes({
	std::make_pair(String("No stealing saves"), 0xFFFFFF_rgb),
	std::make_pair(String("CGI Detected!"), 0xFFFFFF_rgb),
	std::make_pair(String("No vote farming!"), 0xFFFFFF_rgb),
	std::make_pair(String("Your salvation is at hand!"), 0xFFFFFF_rgb),
	std::make_pair(String("No spamming!"), 0xFFFFFF_rgb),
	std::make_pair(String("BANHAMMER!"), 0xFF0000_rgb),
	std::make_pair(String("Thread locked!"), 0xFF0000_rgb),
	std::make_pair(String("Download my mod!"), 0xFFFFFF_rgb),
	std::make_pair(String("I'm playing games, so I'm busy"), 0x7F7F7F_rgb),
	std::make_pair(String("Aboba"), 0x7F7F7F_rgb),
});

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);

void Element::Element_JCB1()
{
	Identifier = "DEFAULT_PT_JCB1";
	Name = "JCB1";
	Colour = 0x094867_rgb;
	MenuVisible = 1;
	MenuSection = SC_RANDOM;
	Enabled = 1;

	Advection = 0.0f;
	AirDrag = 0.00f * CFDS;
	AirLoss = 1.00f;
	Loss = 1.00f;
	Collision = -0.99f;
	Gravity = 0.0f;
	Diffusion = 0.00f;
	HotAir = 0.000f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = -1;

	HeatConduct = 61;
	Description = "Jacob1. Attacks FIGH.";

	Properties = TYPE_ENERGY | PROP_LIFE_DEC | PROP_NOCTYPEDRAW;

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

	Create = &create;

	DefaultProperties.tmp2 = 75;
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Properties
	 * life - Quote timer
	 * tmp  - Quote to display
	 * tmp2 - Health
	 * tmp3 - Laser 1 angle
	 * tmp4 - Laser 2 angle
	 */

	if (parts[i].life == 0) // Update quote
	{
		parts[i].life = 60;
		parts[i].tmp = sim->rng.between(0, quotes.size() - 1);
	}

	// Targeting
	int xdiff, ydiff;
	Vec2<int> target(0 ,0);
	int count = 0;
	bool aggressive = false;
	for (auto j = 0; j < MAX_FIGHTERS; j++)
	{
		if (!sim->fighters[j].spwn)
		{
			continue;
		}
		xdiff = parts[sim->fighters[j].stkmID].x - parts[i].x;
		ydiff = parts[sim->fighters[j].stkmID].y - parts[i].y;
		aggressive = true;

		float angle1 = ToFloat(parts[i].tmp3), angle2 = ToFloat(parts[i].tmp4);
		if (count == 0) // Laser 1
		{
			// Ease angle1 towards the FIGH
			angle1 += (std::atan2(ydiff, xdiff) - angle1) / 20.0f;
			target = IntersectLine(sim, x, y, std::cos(angle1), std::sin(angle1), 9);
			sim->CreateLine(x, y, target.X, target.Y, PT_LASR);
		}
		else if (count == 0 || count == 1) // Laser 2
		{
			// Ease angle2 towards the FIGH
			angle2 += (std::atan2(ydiff, xdiff) - angle2) / 20.0f;
			target = IntersectLine(sim, x, y, std::cos(angle2), std::sin(angle2), 9);
			sim->CreateLine(x, y, target.X, target.Y, PT_LASR);
		}
		parts[i].tmp3 = FromFloat(angle1), parts[i].tmp4 = FromFloat(angle2);

		if (sim->rng.chance(1, 50)) // Guided missile
		{
			auto k = sim->create_part(-1, x, y + (sim->rng.chance(1, 2) ? -2 : 2), PT_MSSL);
			if (k >= 0)
			{
				parts[k].tmp3 = parts[sim->fighters[j].stkmID].x;
				parts[k].tmp4 = parts[sim->fighters[j].stkmID].y;
				parts[k].life = 0;
			}
		}

		count++;
	}

	if (!aggressive)
	{
		// Project force field
		int px = -1, py = -1;
		for (float angle = 0; angle < 2 * std::numbers::pi_v<float>; angle += 0.01f)
		{
			auto x1 = x + 12 * std::cos(angle);
			auto y1 = y + 12 * std::sin(angle);
			if (x1 == px && y1 == py) // Avoid redrawing same spot
			{
				continue;
			}
			sim->create_part(-1, x1, y1, PT_FFLD);
			px = x1, py = y1;
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS) {
	Vec2 pos{ (int)cpart->x, (int)cpart->y };
	auto &quote = quotes[cpart->tmp % quotes.size()];
	gfctx.renderer->BlendText(pos + Vec2{ 12, -7 }, quote.first, quote.second.WithAlpha(255));
	gfctx.renderer->BlendRGBAImage(JCB1Image->data(), RectSized(pos + Vec2{ -2, -6 } , JCB1Image->Size()));

	*colr = *colg = *colb = *cola = 0;
	*pixel_mode = PMODE_NONE;
	return 0;
}

static void create(ELEMENT_CREATE_FUNC_ARGS)
{
	sim->parts[i].tmp = sim->rng.between(0, quotes.size() - 1);
}
