#include "simulation/ElementCommon.h"

#include "ultimata/ElementUtils.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

constexpr int EXFN_LEFT = 0;
constexpr int EXFN_TOP = 1;
constexpr int EXFN_RIGHT = 2;
constexpr int EXFN_BOTTOM = 3;

bool isUp(int dir)
{
	return dir == EXFN_TOP || dir == EXFN_BOTTOM;
}

bool isPartOfWave(int tmp3, int tmp4, int period, int delta, int directionMultiplier, int timer, int offset)
{
	float result = tmp4 / 2;
	result = (int)(result + result * std::sin(std::numbers::pi_v<float> * 2.0f / period * (offset + delta + directionMultiplier * timer))) + 1;
	return result == tmp3 || result == tmp4 - tmp3;
}

void setDirections(int &spawnDX, int &spawnDY, int &initX, int &initY, int &dir, int x, int y, int tmp)
{
	dir = tmp;
	if (isUp(dir))
	{
		spawnDX = 0;
		spawnDY = dir == EXFN_TOP ? -1 : 1;
	}
	else
	{
		spawnDX = dir == EXFN_LEFT ? -1 : 1;
		spawnDY = 0;
	}

	initX = x + spawnDX;
	initY = y + spawnDY;
}

int Element_EXFN_draw_beam(GRAPHICS_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	auto& sim = *gfctx.sim;

	// Color for the beam
	int r, g, b;
	if (!cpart->tmp2)
	{
		r = 84;
		g = 127;
		b = 255;
	}
	else
	{
		r = 255;
		g = 152;
		b = 84;
	}

	// Draw the output funnels
	int period = cpart->tmp4 * 4; // Period = 4 * height

	int spawnDX, spawnDY, initX, initY, dir;
	setDirections(spawnDX, spawnDY, initX, initY, dir, cpart->x, cpart->y, cpart->tmp);

	if (cpart->life && period)
	{
		while (initX >= 0 && initY >= 0 && initX < XRES && initY < YRES)
		{
			// Don't overwrite any solids except GLAS and MVSD
			auto r2 = sim.pmap[initY][initX];
			if (r2 && (elements[TYP(r2)].Properties & TYPE_SOLID) && TYP(r2) != PT_GLAS &&  TYP(r2) != PT_MVSD)
			{
				break;
			}

			/* Function to graph: tmp3 = (tmp4 / 2) + (tmp4 / 2)sin(a(initX - parts[i].x - time))
			 * where a = 2pi / period, tmp4 is 2 * amplitude, tmp3 is y value from 0 to tmp4
			 *
			 * The above example is for a wave going left. If the wave is going right then
			 * direction_multiplier is *= -1.
			 *
			 * If the wave is going up or down then initX - cpart->x is replaced with y
			 * Delta reverse is to map it on the "wrong" axis to make a double back and forth
			 * scanning line
			 *
			 * Other fun things: add -(period * 0.5) after the -time in the sin() to skew the graph
			 */

			int directionMultiplier = !cpart->tmp2 ? 1 : -1; // Direction wave animates
			int delta = initX - cpart->x;
			int deltaReverse = initY - cpart->y;

			if (dir == EXFN_RIGHT || dir == EXFN_BOTTOM)
			{
				directionMultiplier *= -1;
			}

			if (isUp(dir))
			{
				delta = initY - cpart->y;
				deltaReverse = initX - cpart->x;
			}

			if (isPartOfWave(cpart->tmp3, cpart->tmp4, period, delta, directionMultiplier, sim.currentTick, 0))
			{
				DrawGlowyPixel(gfctx.renderer, initX, initY, r, g, b, 255);
			}

			// The portal 2 graphics have another 3rd wave offset
			// about 1/3 of a period with a different color
			if (isPartOfWave(cpart->tmp3, cpart->tmp4, period, delta, directionMultiplier, sim.currentTick, period / 3))
			{
				DrawGlowyPixel(gfctx.renderer, initX, initY, r, g, b, 80);
			}

			// Scanning lines
			if (isPartOfWave(cpart->tmp3, cpart->tmp4, period, deltaReverse, directionMultiplier, sim.currentTick, 0))
			{
				DrawGlowyPixel(gfctx.renderer, initX, initY, r, g, b, 80);
			}

			initX += spawnDX;
			initY += spawnDY;
		}
	}
	return 0;
}

void Element::Element_EXFN()
{
	Identifier = "DEFAULT_PT_EXFN";
	Name = "EXFN";
	Colour = 0x645E8A_rgb;
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
	Hardness = 0;

	Weight = 100;

	HeatConduct = 0;
	Description = "Excursion Funnel. Creates a stasis beam (direction depends on tmp). Toggle reverse with GOLD / TTAN";

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
	Graphics = &graphics;

	DefaultProperties.temp = 273.15f;
}

static int update(UPDATE_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	int spawnDX, spawnDY, initX, initY, dir;
	setDirections(spawnDX, spawnDY, initX, initY, dir, parts[i].x, parts[i].y, parts[i].tmp);

	if (parts[i].temp >= 256.0f + 273.15f)
	{
		parts[i].temp = 256.0f + 273.15f;
	}
	if (parts[i].temp <= -256.0f + 273.15f)
	{
		parts[i].temp = -256.0f + 273.15f;
	}

	/* EXFN properties with not EXFN:
	 * - SPRK (.ctype GOLD): toggle self tmp2 = 0
	 * - SPRK (.ctype TTAN): toggle self tmp2 = 10
	 * - SPRK (.ctype PSCN): toggle life = 10
	 * - SPRK (.ctype NSCN): toggle life = 0
	 */

	for (auto rx = -1; rx <= 1; rx++)
	{
		for (auto ry = -1; ry <= 1; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					continue;
				}
				auto id = ID(r);

				if (TYP(r) == PT_SPRK)
				{
					if (parts[id].ctype == PT_GOLD || parts[id].ctype == PT_TTAN)
					{
						sim->flood_prop(parts[i].x, parts[i].y, AccessProperty{ FIELD_TMP2, parts[id].ctype == PT_GOLD ? 10 : 0 });
					}
					else if (parts[id].ctype == PT_PSCN || parts[id].ctype == PT_NSCN)
					{
						sim->flood_prop(parts[i].x, parts[i].y, AccessProperty{ FIELD_LIFE, parts[id].ctype == PT_PSCN ? 10 : 0 });
					}
				}
			}
		}
	}

	/* tmp4 store total formation height / width
	 * If particle is rightmost or bottommost this is the tmp3 */
	if (!parts[i].tmp4)
	{
		int tX = parts[i].x + (isUp(dir) ? 1 : 0); // Intentionally swapped dy and dx
		int tY = parts[i].y + (isUp(dir) ? 0 : 1);
		if (tY >= 0 && tY >= 0 && tX < XRES && tY < YRES && TYP(pmap[tY][tX]) != PT_EXFN)
		{
			sim->flood_prop(parts[i].x, parts[i].y, AccessProperty{ FIELD_TMP4, parts[i].tmp3 });
		}
	}

	/* EXFN properties with other EXFN
	 * - life: powered state, 1 = on, 0 = off. If life > 10 will slowly decrease to 0
	 *         and "spread" its powered state to others. Likewise, if life = 1 and a nearby
	 *         particle has life = 0, it will "spread" to life = 0. Toggles with NSCN and PSCN
	 * - tmp: 0 = left, 1 = top, 2 = right, 3 = bottom
	 * - tmp2: reversed, 1 = reversed, 0 = not. Spreads like life, toggles with GOLD
	 * - tmp3: depending on direction, if
	 *         particle is leftmost or topmost, it will have a tmp3 of 1
	 *         any other particle = particle to left or top + 1
	 */

	// Slowly decrement life and tmp2 to 1 if > 1
	if (parts[i].life > 1)
	{
		parts[i].life--;
	}

	if (parts[i].tmp2 > 1)
	{
		parts[i].tmp2--;
	}

	// Count particles and set appropriate tmp3
	// NOTE: Depends on particles being updated top->bottom, left->right
	int initialTmp3 = parts[i].tmp3;
	int delta = 1;
	while (delta >= 0)
	{
		int r;
		if (EXFN_TOP == dir || EXFN_BOTTOM == dir) // Vertical dir, go across
		{
			r = pmap[y][x - delta];
		}
		else
		{
			r = pmap[y - delta][x]; // Horizontal beam, go vertical
		}

		// Rightmost or bottommost
		if (!r)
		{
			break;
		}

		if (parts[ID(r)].tmp3)
		{
			parts[i].tmp3 = parts[ID(r)].tmp3 + delta;
			break;
		}

		delta--;
	}

	// Leftmost or topmost
	if (!parts[i].tmp3)
	{
		parts[i].tmp3 = 1;
	}

	// Initial tmp3 differs, reset tmp4!
	if (parts[i].tmp3 != initialTmp3)
	{
		parts[i].tmp4 = 0;
	}

	// Check for other EXFN
	for (auto rx = -2; rx <= 2; rx++)
	{
		for (auto ry = -2; ry <= 2; ry++)
		{
			if (rx || ry)
			{
				auto r = pmap[y + ry][x + rx];
				if (!r)
				{
					continue;
				}
				auto id = ID(r);

				if (TYP(r) == PT_EXFN)
				{
					if (parts[i].life == 1 && !parts[id].life)
					{
						parts[i].life = 0;
					}
					else if (!parts[i].life && parts[id].life > 1)
					{
						parts[i].life = 10;
					}
					if (parts[i].tmp2 == 1 && !parts[id].tmp2)
					{
						parts[i].tmp2 = 0;
					}
					else if (!parts[i].tmp2 && parts[id].tmp2 > 1)
					{
						parts[i].tmp2 = 10;
					}
				}
			}
		}
	}

	int period = parts[i].tmp4 * 4; // Period = 4 * height
	int rv = !parts[i].tmp2 ? 1 : -1;

	if (parts[i].life && period)
	{
		while (initX >= 0 && initY >= 0 && initX < XRES && initY < YRES)
		{
			// Don't overwrite any solids except GLAS and MVSD
			auto r2 = pmap[initY][initX];
			if (r2 && (elements[TYP(r2)].Properties & TYPE_SOLID) && TYP(r2) != PT_GLAS &&  TYP(r2) != PT_MVSD)
			{
				break;
			}

			// Set the stasis field (/4 since it resets every 4 frames)
			sim->stasisVX[initY / STASIS_CELL][initX / STASIS_CELL] += rv * spawnDX / 4.0f;
			sim->stasisVY[initY / STASIS_CELL][initX / STASIS_CELL] += rv * spawnDY / 4.0f;
			sim->stasisStrength[initY / STASIS_CELL][initX / STASIS_CELL] = (parts[i].temp - (273.15f - 256.0f)) * 1.6f / 512.0f;

			// Beams should push gently towards center
			float nudgeSpeed;
			if (parts[i].tmp3 < parts[i].tmp4 / 2)
			{
				nudgeSpeed = 0.5f;
			}
			else if(parts[i].tmp3 == parts[i].tmp4 / 2)
			{
				nudgeSpeed = 0.0f;
			}
			else
			{
				nudgeSpeed = -0.5f;
			}

			if (isUp(dir))
			{
				sim->stasisVX[initY / STASIS_CELL][initX / STASIS_CELL] += nudgeSpeed / 4.0f;
			}
			else
			{
				sim->stasisVY[initY / STASIS_CELL][initX / STASIS_CELL] += nudgeSpeed / 4.0f;
			}

			initX += spawnDX;
			initY += spawnDY;
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	// Brighten color if powered
	if (cpart->life > 0)
	{
		*colb *= 2;
		*colg *= 2;
		*colr *= 2;
	}

	Element_EXFN_draw_beam(GRAPHICS_FUNC_SUBCALL_ARGS);

	return 0;
}
