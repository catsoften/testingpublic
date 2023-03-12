#include "simulation/ElementCommon.h"

static int update(UPDATE_FUNC_ARGS);
static int graphics(GRAPHICS_FUNC_ARGS);
static void create(ELEMENT_CREATE_FUNC_ARGS);
void Element_CYTK_get_target(Simulation *sim, Particle *parts, int &tarx, int &tary);

#define BEE_SEARCH_RANGE 3
#define MAX_BEE_BUILD_SPEED 1.0f
#define MAX_BEE_SPEED 2.0f
#define MAX_RADIUS_TO_MAKE_WAX 20
#define EDGE_RANGE 20
#define SWARM_RANGE 90
#define PREFERED_TEMP 30.0f + 273.15f

void Element::Element_BEE() {
	Identifier = "DEFAULT_PT_BEE";
	Name = "BEE";
	Colour = PIXPACK(0xFFC414);
	MenuVisible = 1;
	MenuSection = SC_ORGANIC;
	Enabled = 1;

	Advection = 0.7f;
	AirDrag = 0.0000f * CFDS;
	AirLoss = 0.96f;
	Loss = 1.0f;
	Collision = 0.00f;
	Gravity = 0.00f;
	Diffusion = 0.01f;
	HotAir = 0.0010f * CFDS;
	Falldown = 0;

	Flammable = 0;
	Explosive = 0;
	Meltable = 0;
	Hardness = 0;

	Weight = 20;

	HeatConduct = 70;
	Description = "Bees. Pollinates PLNT, makes honey and wax hives where it spawns.";

	Properties = TYPE_GAS | PROP_DEADLY;

	LowPressure = IPL;
	LowPressureTransition = NT;
	HighPressure = 10.0f;
	HighPressureTransition = PT_DUST;
	LowTemperature = 273.15f - 5.0f;
	LowTemperatureTransition = PT_SNOW;
	HighTemperature = 273.15f + 70.0f;
	HighTemperatureTransition = PT_DUST;

	Update = &update;
	Graphics = &graphics;
	Create = &create;
}

static void create(ELEMENT_CREATE_FUNC_ARGS) {
	sim->parts[i].dcolour = RNG::Ref().between(0, 7);
	sim->parts[i].tmp2 = RNG::Ref().chance(1, 2);

	if (RNG::Ref().chance(1, 500)) // Queen bee
		sim->parts[i].tmp2 = 2;

	sim->parts[i].tmp3 = sim->parts[i].x;
	sim->parts[i].tmp4 = sim->parts[i].y;
}

static int update(UPDATE_FUNC_ARGS) {
	/**
	 * Properties:
	 * tmp  - color variation (0 - 6 inclusive)
	 * tmp2 - working stage, bees alternate between finding food and
	 * 		  tending for the hive. 0 = work, 1 = find
	 * life - pollen level, 0 to 100, stops pollinating when at 100
	 * tmp3/4 - Spawn coordinates
	 * flags - aggressive? SMKE sets this to 100, decrements each frame
	 */
	int rx, ry, r, rt;
	bool freeze = false; // Dont move (when pollinating)
	bool already_pollinated = false; // Already pollinated a plant this frame

	if (parts[i].flags > 0)
		--parts[i].flags;
	if (parts[i].flags < 0)
		parts[i].flags = 0;

	// Queen bee behavior differs: spawn a random worker every 500 frames
	if (parts[i].tmp2 == 2 && sim->timer % 500 == 0) {
		parts[i].vx = parts[i].vy = 0;

		for (rx = -1; rx < 2; ++rx)
			for (ry = -1; ry < 2; ++ry)
				if (BOUNDS_CHECK && (rx || ry)) {
					r = pmap[y + ry][x + rx];
					if (!r) {
						int ni = sim->create_part(-1, x + rx, y + ry, PT_BEE);
						parts[ni].tmp2 = RNG::Ref().chance(1, 2); // New bee cannot be queen
						parts[ni].tmp3 = parts[i].tmp3;
						parts[ni].tmp4 = parts[i].tmp4;
						return 0;
					}
				}
		return 0;
	}


	// Normal worker behavior
	float tendx = 0.0f, tendy = 0.0f;
	int object_count = 0;

	bool near_hive = fabs(x - parts[i].tmp3) < MAX_RADIUS_TO_MAKE_WAX &&
					 fabs(y - parts[i].tmp4) < MAX_RADIUS_TO_MAKE_WAX;

	// For constructing wax honeycomb, it won't construct if another
	// solid is near (bee space)
	int waxrx = 0, waxry = 0;
	bool should_make_wax = true;

	// Avoid edges
	if (x < EDGE_RANGE) tendx += 10.0f;
	if (y < EDGE_RANGE) tendy += 10.0f;
	if (x > XRES - EDGE_RANGE) tendx -= 10.0f;
	if (y > YRES - EDGE_RANGE) tendy -= 10.0f;
	if (tendx || tendy) object_count = 1;

	// Follow dance if contained
	if (parts[i].tmp > 0 && parts[i].tmp % 2 == 1 && parts[i].tmp2 == 1 && parts[i].life == 0) {
		float angle = (parts[i].tmp % 1000) / 180 * 3.1415f;
		tendx -= 2.0f * cos(angle);
		tendy -= 2.0f * sin(angle);
	}

	// Randomly "fail" and decide to return to hive
	if (RNG::Ref().chance(1, 5000)) {
		parts[i].tmp = -1;
	}
	
	for (rx = -BEE_SEARCH_RANGE; rx <= BEE_SEARCH_RANGE; ++rx)
		for (ry = -BEE_SEARCH_RANGE; ry <= BEE_SEARCH_RANGE; ++ry)
			if (BOUNDS_CHECK && (rx || rx)) {
				r = pmap[y + ry][x + rx];
				if (!r) {
					// Builder, try to expand the hive here
					// Hives follow a hexagon pattern:
					//  xx  xx
					// x  xx  x
					//  xx  xx
					if (!(waxrx || waxry) &&
							abs(rx) < 2 && abs(ry) < 2 &&    // Right next to spot and not already building WAX
							((x + rx) / 2) % 2 == (y + ry) % 2 &&         // Hexagon shape
							!parts[i].tmp2 && RNG::Ref().chance(1, 50) && // Is building
							near_hive // Dont make too far from spawn
						) {
						waxrx = rx, waxry = ry;
					}
					continue;
				}
				rt = TYP(r);

				// Shouldn't make WAX if near non-HONY non-WAX solid or powder
				if ((rt != PT_WAX && rt != PT_HONY && rt != PT_BEE) && (
					sim->elements[rt].Properties & TYPE_SOLID ||
					sim->elements[rt].Properties & TYPE_PART
				))
					should_make_wax = false;

				// Instantly get zapped by electricity
				if (rt == PT_LIGH || rt == PT_SPRK) {
					sim->part_change_type(i, x, y, PT_DUST);
					return 0;
				}

				// Passive if smoked
				if (rt == PT_SMKE)
					parts[i].flags = 100;

				// Foraging
				if (parts[i].tmp2) {
					// Head towards PLNT, SUGR, WATR and SWTR
					if (rt == PT_PLNT || rt == PT_WATR || rt == PT_SWTR || rt == PT_SUGR) {
						if (parts[i].life < 100) {
							tendx += rx, tendy += ry;
						} else { // Go away if full to leave room for other bees
							tendx -= rx, tendy -= ry; }
						++object_count;

						// Pollinate PLNT, fuel up on SUGR or water
						// Can only pollinate 1px of PLNT at a time
						if (abs(rx) < 2 && abs(ry) < 2 && parts[i].life < 100 && !already_pollinated) {
							freeze = already_pollinated = true;
							++parts[i].life;

							// Encode dance data
							if (parts[i].tmp == 0 || parts[i].tmp % 2 == 1) {
								int dis = sqrt((x - parts[i].tmp3) * (x - parts[i].tmp3)
											+ (y - parts[i].tmp4) * (y - parts[i].tmp4));
								int angle = atan2(-y + parts[i].tmp4, x - parts[i].tmp3) * 180 / 3.1415f;
								while (angle < 0) angle += 360;
								while (angle > 360) angle -= 360;
								parts[i].tmp = angle + 1000 * dis;

								// Make sure tmp % 2 = 0
								if (parts[i].tmp % 2 != 0)
									++parts[i].tmp;
							}

							// Pollinate PLNT (makes it grow)
							if (rt == PT_PLNT && RNG::Ref().chance(1, 500))
								sim->part_change_type(ID(r), parts[ID(r)].x, parts[ID(r)].y, PT_VINE);
							
							// Consume SUGR or SWTR
							else if ((rt == PT_SUGR || rt == PT_SWTR || rt == PT_WATR) && RNG::Ref().chance(2, 100))
								sim->kill_part(ID(r));
						}
					}
					// Dont avoid other BEEs, WAX and HONY
					else if (rt == PT_BEE || rt == PT_HONY) {
						if (parts[ID(r)].temp > PREFERED_TEMP && RNG::Ref().chance(1, 100))
							--parts[ID(r)].temp;
						else if (parts[ID(r)].temp > PREFERED_TEMP && RNG::Ref().chance(1, 100))
							++parts[ID(r)].temp;

						// Listen to other bees' dances
						if (rt == PT_BEE && parts[ID(r)].tmp2 == 1 && // Other bee is dancing
							parts[ID(r)].tmp > 0 && parts[ID(r)].tmp % 2 == 0 &&
							parts[i].tmp <= 0 && parts[i].tmp2 == 1 && near_hive && parts[i].life == 0 // Self is ready to find again
						) {
							parts[i].tmp = parts[ID(r)].tmp + 1; // Make sure % 2 == 1
						}
					}
					else if (rt == PT_WAX) {
						// Regulate temperature
						if (parts[ID(r)].temp > PREFERED_TEMP && RNG::Ref().chance(1, 100))
							--parts[ID(r)].temp;
						else if (parts[ID(r)].temp > PREFERED_TEMP && RNG::Ref().chance(1, 100))
							++parts[ID(r)].temp;

						if (parts[i].life >= 100 && near_hive) {
							// We found some WAX, if near spawn lets deposit honey on it
							for (int rx2 = -1; rx2 < 2; ++rx2)
							for (int ry2 = -1; ry2 < 2; ++ry2)
								if (BOUNDS_CHECK && (rx || ry)) {
									r = pmap[y + ry + ry2][x + rx + rx2];
									if (!r) { // Deposit honey
										parts[i].life = 0;
										sim->create_part(-1, x + rx + rx2, y + ry + ry2, PT_HONY);
										return 0;
									}
								}
							return 0;
						}
					}
					// Avoid non-gases or hot objects
					else if (rt != PT_WEB && (!(sim->elements[rt].Properties & TYPE_GAS) || parts[ID(r)].temp > 273.15f + 50.0f)) {
						tendx -= rx;
						tendy -= ry;
						++object_count;
					}
				}
			}

	// Honeycomb construction
	if (should_make_wax && (waxrx || waxry))
		sim->create_part(-1, x + waxrx, y + waxry, PT_WAX);

	// Swarm the player
	if (parts[i].flags == 0) {
		int tarx = -1, tary = -1;
		Element_CYTK_get_target(sim, parts, tarx, tary);

		if (tarx >= 0 && tary >= 0 && abs(x - tarx) < SWARM_RANGE && abs(y - tary) < SWARM_RANGE) {
			parts[i].vx -= (x - tarx) / 800.0f;
			parts[i].vy -= (y - tary) / 800.0f;
		}
	}

	// Reset "give up" if near spawn
	if (near_hive && RNG::Ref().chance(1, 100))
		parts[i].tmp = 0;

	if (freeze) {
		parts[i].vx = 0;
		parts[i].vy = 0;
	} else {
		// Tend towards / away from objects
		if (object_count) {
			parts[i].vx += tendx / object_count;
			parts[i].vy += tendy / object_count;
		}

		// Dancing overrides leaving the hive once HONY is deposisted
		// Conditions: finder bee, no HONY, near the hive, has a direction to encode, direction % 2 == 0
		// (Meaning tell others the direction)
		if (parts[i].tmp2 == 1 && parts[i].life == 0 && near_hive && parts[i].tmp > 0 && parts[i].tmp % 2 == 0) {
			int dis = parts[i].tmp / 1000;

			parts[i].vx -= 15.0f * cos(sim->timer / 3.0f);
			parts[i].vy -= 15.0f * sin(sim->timer / 3.0f);

			// Length of dance depends on distance
			if (RNG::Ref().chance(1, dis))
				parts[i].tmp = 0;
		} else {
			// Tend away from spawn if foraging, else towards
			// If full on pollen return to spawn
			// If give up return to spawn
			int m = parts[i].tmp2 && (parts[i].life < 100 || parts[i].tmp < 0) ? 1 : -1;
			if ((m < 0 && !near_hive) || (m > 0 && near_hive)) {
				parts[i].vx += m * (parts[i].x - parts[i].tmp3) / 1500.0f;
				parts[i].vy += m * (parts[i].y - parts[i].tmp4) / 1500.0f;
			}
		}
	}

	// Restrict speed
	int MAX_SPEED = parts[i].tmp2 ? MAX_BEE_SPEED : MAX_BEE_BUILD_SPEED;
	if (fabs(parts[i].vx) > MAX_SPEED) parts[i].vx = isign(parts[i].vx) * MAX_SPEED;
	if (fabs(parts[i].vy) > MAX_SPEED) parts[i].vy = isign(parts[i].vy) * MAX_SPEED;
	
	return 0;
}

static int graphics(GRAPHICS_FUNC_ARGS) {
	*pixel_mode |= NO_DECO;
	*colr *= ((float)cpart->dcolour + 1) / 6;
	*colg *= ((float)cpart->dcolour + 1) / 6;
	*colb *= ((float)cpart->dcolour + 1) / 6;

	if (cpart->tmp2 == 2) // Queen bee is fatter by 1 px
		ren->drawrect(cpart->x + 1, cpart->y, 1, 1, *colr, *colg, *colb, 255);
	return 0;
}
