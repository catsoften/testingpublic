#include "ElementUtils.h"

#include <bit>
#include <cmath>

#include "graphics/Renderer.h"
#include "SimulationConfig.h"
#include "simulation/ElementClasses.h"
#include "simulation/Simulation.h"
#include "simulation/SimulationData.h"

static_assert(sizeof(int) == sizeof(float), "[Ultimata] Sizes of int and float must be equal"); // At least it checks...

int FromFloat(float x)
{
	return std::bit_cast<int>(x);
}

float ToFloat(int x)
{
	return std::bit_cast<float>(x);
}

Vec2<int> IntersectLine(Simulation *sim, int sx, int sy, float vx, float vy, int type, int type2)
{
	/**
	 * Calculate line intersect starting from sx and sy, drawing a line
	 * along velocity vx, vy, the first object it intersects and returns coordinates
	 * of the intersection, storing in x and y. Ignores walls that don't block solids.
	 *
	 * If no intersection is found and it reaches the edge of the screen, returns -1
	 * for each parameter.
	 *
	 * Type:
	 * 	1: Ignore type2 (by default NONE is always ignored)
	 * 	2: Ignore liquid
	 * 	3. Ignore gas
	 * 	4. Solid and powder only
	 * 	5. Solid only
	 *  6. Ignore type2 if within 10px of start
	 *  7. Seek out ctype
	 *  8. Seek out ctype if not within 10px of start, otherwise ignore all
	 *  9. Solid only, return edge location
	 * FFLD always blocks regardless of setting
	 *
	 * Type2: Type parameter for type, ie PT_SPDR
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	float tx = sx, ty = sy;
	int px, py;

	// No movement
	if (vx == 0 && vy == 0)
	{
		return Vec2{ -1, -1 };
	}

	// Reduce velocity magnitudes so neither > 1
	float larger = std::abs(vx) < std::abs(vy) ? std::abs(vy) : std::abs(vx);
	if (larger > 1)
	{
		vx /= larger;
		vy /= larger;
	}

	while (true)
	{
		px = (int)tx, py = (int)ty;
		tx += vx, ty += vy;

		// Out of bounds
		if ((int)tx < 0 || (int)ty < 0 || (int)tx >= XRES || (int)ty >= YRES)
		{
			if (type == 9) // Return location
			{
				int x, y;
				if ((int)tx < 0)
				{
					x = 0;
				}
				else if ((int)tx >= XRES)
				{
					x = XRES - 1;
				}
				else
				{
					x = (int)tx;
				}

				if ((int)ty < 0)
				{
					y = 0;
				}
				else if ((int)ty >= YRES)
				{
					y = YRES - 1;
				}
				else
				{
					y = (int)ty;
				}
				return Vec2{ x, y };
			}
			return Vec2{ -1, -1 };
		}

		// Same position as before we can skip check
		if ((int)tx == px && (int)ty == py)
		{
			continue;
		}

		// Too far from start, change type from 6 to 5 and 8 to 7
		int dis = std::hypot(tx - sx, ty - sy);
		if (dis > 10 && type == 6)
		{
			type = 5;
		}
		if (dis > 10 && type == 8)
		{
			type = 7;
		}

		// Found particle
		auto r = sim->pmap[(int)ty][(int)tx];
		if (
			r && (
			(type == 1 && TYP(r) != type2) ||
			(type == 2 && !(elements[TYP(r)].Properties & TYPE_LIQUID)) ||
			(type == 3 && !(elements[TYP(r)].Properties & TYPE_GAS)) ||
			(type == 4 && (elements[TYP(r)].Properties & TYPE_SOLID || elements[TYP(r)].Properties & TYPE_PART)) ||
			((type == 5 || type == 9) && elements[TYP(r)].Properties & TYPE_SOLID) ||
			(type == 6 && TYP(r) != type2) ||
			(type == 7 && TYP(r) == type2)
		))
		{
			return Vec2{ (int)tx, (int)ty };
		}

		// Blocking wall
		if (sim->IsWallBlocking((int)tx, (int)ty, PT_DMND))
		{
			return Vec2{ (int)tx, (int)ty };
		}
	}
}

void DrawGlowyPixel(Renderer *ren, int x, int y, int colr, int colg, int colb, int cola)
{
	auto color = RGB(colr, colg, colb);

	// Non-glowy render mode
	if (ren->GetColorMode() & COLOUR_HEAT || !(ren->GetColorMode() & PMODE_GLOW))
	{
		ren->AddPixel({ x, y }, color.WithAlpha(cola));
	}
	else
	{
		ren->AddPixel({ x, y }, color.WithAlpha((192 * cola) / 255));
		ren->AddPixel({ x + 1, y }, color.WithAlpha((96 * cola) / 255));
		ren->AddPixel({ x - 1, y }, color.WithAlpha((96 * cola) / 255));
		ren->AddPixel({ x, y + 1 }, color.WithAlpha((96 * cola) / 255));
		ren->AddPixel({ x, y - 1 }, color.WithAlpha((96 * cola) / 255));

		for (auto x = 1; x < 6; x++)
		{
			auto color2 = color.WithAlpha((5 * cola) / 255);

			ren->AddPixel({ x, y - x }, color2);
			ren->AddPixel({ x, y + x }, color2);
			ren->AddPixel({ x - x, y }, color2);
			ren->AddPixel({ x + x, y }, color2);

			for (auto y = 1; y < 6; y++)
			{
				if (x + y > 7)
				{
					continue;
				}

				ren->AddPixel({ x + x, y - y }, color2);
				ren->AddPixel({ x - x, y + y }, color2);
				ren->AddPixel({ x + x, y + y }, color2);
				ren->AddPixel({ x - x, y - y }, color2);
			}
		}
	}
}

void TimeDilation(Simulation *sim, int x, int y, int radius, int val)
{
	x /= CELL;
	y /= CELL;

	for (auto dx = -radius; dx <= radius; dx++)
	{
		for (auto dy = -radius; dy <= radius; dy++)
		{
			float r = std::hypot(dx, dy);
			if (
				r <= radius &&
				x + dx >= 0 && x + dx < XCELLS &&
				y + dy >= 0 && y + dy < YCELLS
			)
			{
				float target = val * (1 - r / radius);
				if (std::abs(target) > std::abs(sim->timeDilation[y + dy][x + dx]))
				{
					sim->timeDilation[y + dy][x + dx] = target;
				}
			}
		}
	}
}
