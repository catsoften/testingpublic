#include "simulation/ElementCommon.h"

#include "simulation/MovingSolid.h"
#include "ultimata/ElementUtils.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);

void Element::Element_MVSD()
{
	Identifier = "DEFAULT_PT_MVSD";
	Name = "MVSD";
	Colour = 0xDB3030_rgb;
	MenuVisible = 1;
	MenuSection = SC_SPECIAL;
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
	Hardness = 20;

	Weight = 100;

	HeatConduct = 255;
	Description = "Moving Solid. Mimics its ctype.";

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
}

static int update(UPDATE_FUNC_ARGS)
{
	/**
	 * Moving solid setup: moving solids are grouped by their ID, which is defined by tmp2
	 * If tmp2 is 0 the particle will automatically begin a floodfill to detect other MVSD that
	 * belongs to this moving solid, and group any that are not part of this moving solid group
	 *
	 * If flags = 1, solid should be reconstructed (Edit was made)
	 *
	 * life stores previous ctype (If it was stained by portal gel)
	 *
	 * tmp3 and tmp4 store the solid's velocity for updating when a solid is cut into 2 solids
	 */

	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	if (!parts[i].tmp2)
	{
		MovingSolid::CreateMovingSolid(parts, pmap, i);
	}

	auto &thisSolid = MovingSolid::solids[parts[i].tmp2];

	bool validCtype = parts[i].ctype > 0 && parts[i].ctype < PT_NUM && elements[parts[i].ctype].Enabled;
	int px = (int)(parts[i].x + 0.5);
	int py = (int)(parts[i].y + 0.5);

	/**
	 * Failed to find group, flood fill current state id to 0 and recreate the solid.
	 * This happens sometimes during undo-redo operations (maybe redo/undo changes particle ids?)
	 * Also if flags = 1 redo, there is a chance the solid was cut
	 */
	if (!MovingSolid::solids.count(parts[i].tmp2) || parts[i].flags == 1)
	{
		MovingSolid::Reset(px, py, parts, pmap, parts[i].tmp2);
		MovingSolid::CreateMovingSolid(parts, pmap, i);

		// If the solid was cut, we'll need to set its velocity to the original
		if (parts[i].flags == 1)
		{
			thisSolid.velocity = { ToFloat(parts[i].tmp3), ToFloat(parts[i].tmp4) };
			parts[i].tmp3 = parts[i].tmp4 = FromFloat(0.0f);
		}

		parts[i].flags = 0;
		return 0;
	}

	// Ctype is not a solid or portal gel
	if (!validCtype || (!(elements[parts[i].ctype].Properties & TYPE_SOLID) && parts[i].ctype != PT_PGEL))
	{
		parts[i].ctype = 0;
	}

	// Basic ctype mimicing
	if (validCtype)
	{
		// Flammability mimic
		if ((elements[parts[i].ctype].Explosive & 2) && sim->pv[y / CELL][x / CELL] > 2.5f)
		{
			parts[i].life = sim->rng.between(180, 259);
			parts[i].temp = restrict_flt(elements[PT_FIRE].DefaultProperties.temp + (elements[parts[i].ctype].Flammable / 2), MIN_TEMP, MAX_TEMP);
			sim->part_change_type(i, x, y, PT_FIRE);
			sim->pv[y / CELL][x / CELL] += 0.25f * CFDS;
			return 0;
		}

		int changeType = -1;

		if (elements[parts[i].ctype].HighTemperatureTransition != NT && parts[i].temp > elements[parts[i].ctype].HighTemperature) // Melt if ctype allows it
		{
			changeType = elements[parts[i].ctype].HighTemperatureTransition;
		}
		else if (elements[parts[i].ctype].HighPressureTransition != NT && -1 < elements[parts[i].ctype].HighPressure && sim->pv[y / CELL][x / CELL] > elements[parts[i].ctype].HighPressure) // Shatter depending on pressure transition
		{
			changeType = elements[parts[i].ctype].HighPressureTransition;
		}
		else if (elements[parts[i].ctype].LowPressureTransition != NT && -1 < elements[parts[i].ctype].LowPressure && sim->pv[y / CELL][x / CELL] < elements[parts[i].ctype].LowPressure)
		{
			changeType = elements[parts[i].ctype].LowPressureTransition;
		}

		// Ice has special transition, default to water
		if (parts[i].ctype == PT_ICEI)
		{
			if (parts[i].temp > 273.15f)
			{
				changeType = PT_WATR;
			}
			else
			{
				changeType = -1;
			}
		}

		// We have to change into something
		if (changeType > -1)
		{
			auto temp = sim->create_part(-3, x, y, changeType);

			if (temp >= 0)
			{
				if (changeType == PT_LAVA)
				{
					parts[temp].ctype = parts[i].ctype;
				}

				parts[temp].temp = parts[i].temp;

				if (!(elements[changeType].Properties & TYPE_SOLID))
				{
					parts[temp].vx = thisSolid.velocity.X;
					parts[temp].vy = thisSolid.velocity.Y;
				}
			}
			sim->kill_part(i);
			return 0;
		}
	}

	// Flag if it overlaps
	if (sim->pmap_count[y][x] > 1)
	{
		thisSolid.FlagOverlap();
	}

	// Should it make pressure on impact?
	thisSolid.ShouldImpactPressure(!parts[i].tmp);

	// If self has a velocity somehow update the moving solid
	if (parts[i].vx || parts[i].vy)
	{
		thisSolid.velocity = { parts[i].vx, parts[i].vy };
		parts[i].vx = parts[i].vy = 0.0f;
	}

	// Better bouncing, but stay flush with surface when stopped
	Vec2<float> vel = thisSolid.velocity;

	int ext = (int)(std::hypot(vel.X, vel.Y) / 2.0f + 0.5f);

	// Check surrounding particles. These are "touching" collisions, as in the solid is flush with another particle
	for (auto rx = -1 - ext; rx <= 1 + ext; rx++)
	{
		for (auto ry = -1 - ext; ry <= 1 + ext; ry++)
		{
			// Allow wall collisions
			if (sim->IsWallBlocking(x + rx, y + ry, PT_MVSD))
			{
				thisSolid.AddCollision(
					MovingSolid::Collision(
						MovingSolid::Collision::Type::Static,
						i,
						-1,
						{ x + rx, y + ry }
					)
				);
			}

			if ((!rx && !ry) || x + rx < 0 || x + rx >= XRES || y + ry < 0 || y + ry >= YRES)
			{
				continue;
			}

			auto r = pmap[y + ry][x + rx];
			if (!r)
			{
				continue;
			}
			auto rt = TYP(r);

			// Flammability mimic
			if (validCtype && (elements[parts[i].ctype].Explosive & 2) && rt == PT_FIRE)
			{
				parts[i].life = sim->rng.between(180, 259);
				parts[i].temp = restrict_flt(elements[PT_FIRE].DefaultProperties.temp + (elements[parts[i].ctype].Flammable / 2), MIN_TEMP, MAX_TEMP);
				sim->part_change_type(i, x, y, PT_FIRE);
				sim->pv[y / CELL][x / CELL] += 0.25f * CFDS;
			}

			// Unstain portal gel
			if (rt == PT_WATR || rt == PT_SLTW || rt == PT_DSTW)
			{
				parts[i].ctype = 0;
			}

			if (rt == PT_MVSD && parts[ID(r)].tmp2 != parts[i].tmp2) // Collision with another moving solid
			{
				thisSolid.AddCollision(
					MovingSolid::Collision(
						MovingSolid::Collision::Type::Moving,
						i,
						ID(r)
					)
				);
			}
			else if (rt != PT_MVSD && rt != PT_FILL) // General collision (Ignore FILL)
			{
				if (elements[rt].Properties & TYPE_SOLID)
				{
					thisSolid.AddCollision(
						MovingSolid::Collision(
							MovingSolid::Collision::Type::Static,
							i,
							ID(r)
						)
					);
				}
				else if (elements[rt].Properties & TYPE_PART || rt == PT_PGEL)
				{
					thisSolid.AddCollision(
						MovingSolid::Collision(
							MovingSolid::Collision::Type::NonStatic,
							i,
							ID(r)
						)
					);
				}
			}
		}
	}

	// Check for phasing into solids
	if (vel.X || vel.Y)
	{
		float largest = std::max(std::abs(vel.X), std::abs(vel.Y));

		// Avoid "division by 0" effect where it ends up scanning the entire map
		if (largest <= 1)
		{
			return 0;
		}

		float dvx = vel.X / largest, dvy = vel.Y / largest;
		float sx = (int)(parts[i].x + 0.5), sy = (int)(parts[i].y + 0.5);
		int sxRound = sx, syRound = sy;
		int px = 0, py = 0;
		int count = 1;

		while (
			sxRound >= 0 && sxRound < XRES && syRound >= 0 && syRound < YRES &&
			std::abs(-dvx + dvx * count) <= std::abs(vel.X) &&
			std::abs(-dvy + dvy * count) <= std::abs(vel.Y)
		)
		{
			if (px != sxRound || py != syRound)
			{
				// Avoid rubberbanding
				if (std::abs(sxRound - px) <= MovingSolid::MAX_VELOCITY && std::abs(syRound - py) <= MovingSolid::MAX_VELOCITY)
				{
					auto r = pmap[syRound][sxRound];
					if (r)
					{
						if (TYP(r) != PT_MVSD && TYP(r) != PT_FILL && (elements[TYP(r)].Properties & TYPE_PART || elements[TYP(r)].Properties & TYPE_SOLID))
						{
							thisSolid.AddCollision(
								MovingSolid::Collision(
									elements[TYP(r)].Properties & TYPE_PART ? MovingSolid::Collision::Type::NonStatic : MovingSolid::Collision::Type::Static,
									i,
									ID(r)
								)
							);

							parts[i].tmp = 1;
							break;
						}
						else if (parts[ID(r)].tmp2 != parts[i].tmp2)
						{
							// Collisions with other solids are handled
							// in simulation/mvsd
						}
					}
				}
			}

			px = sxRound;
			py = syRound;
			sx += dvx;
			sy += dvy;
			sxRound = (int)(sx + 0.5f);
			syRound = (int)(sy + 0.5f);
			count++;
		}
	}

	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS)
{
	auto &sd = SimulationData::CRef();
	auto &elements = sd.elements;

	// Mimic ctype color
	bool validCtype = cpart->ctype > 0 && cpart->ctype < PT_NUM && elements[cpart->ctype].Enabled;
	if (validCtype)
	{
		*colr = elements[cpart->ctype].Colour.Red;
		*colg = elements[cpart->ctype].Colour.Green;
		*colb = elements[cpart->ctype].Colour.Blue;

		// Stolen HOT_GLOW code
		if (elements[cpart->ctype].Properties & PROP_HOT_GLOW && cpart->temp > (elements[cpart->ctype].HighTemperature - 800.0f))
		{
			float gradv = 3.1415 / (2 * elements[cpart->ctype].HighTemperature - elements[cpart->ctype].HighTemperature + 800.0f);
			float caddress = (cpart->temp > elements[cpart->ctype].HighTemperature) ? elements[cpart->ctype].HighTemperature - (elements[cpart->ctype].HighTemperature - 800.0f) : cpart->temp - (elements[cpart->ctype].HighTemperature - 800.0f);
			*colr += std::sin(gradv * caddress) * 226;
			*colg += std::sin(gradv * caddress * 4.55 + 3.14) * 34;
			*colb += std::sin(gradv * caddress * 2.22 + 3.14) * 64;
		}
	}

	return 0;
}
