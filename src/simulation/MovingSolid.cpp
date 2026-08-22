#include "MovingSolid.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>

#include "Misc.h"
#include "ultimata/ElementUtils.h"

typedef union { float flVal; uint32_t nVal; } floatIntUnion;

// https://www.dsprelated.com/showarticle/1052.php
float FastAtan2(float y, float x)
{
	constexpr float n1 = 0.97239411f;
	constexpr float n2 = -0.19194795f;
	float result = 0.0f;
	if (x != 0.0f)
	{
		const floatIntUnion tYSign = { y };
		const floatIntUnion tXSign = { x };
		if (std::abs(x) >= std::abs(y))
		{
			floatIntUnion tOffset = { std::numbers::pi_v<float> };
			// Add or subtract PI based on y's sign.
			tOffset.nVal |= tYSign.nVal & 0x80000000u;
			// No offset if x is positive, so multiply by 0 or based on x's sign.
			tOffset.nVal *= tXSign.nVal >> 31;
			result = tOffset.flVal;
			const float z = y / x;
			result += (n1 + n2 * z * z) * z;
		}
		else // Use atan(y/x) = pi/2 - atan(x/y) if |y/x| > 1.
		{
			floatIntUnion tOffset = { std::numbers::pi_v<float> / 2 };
			// Add or subtract PI/2 based on y's sign.
			tOffset.nVal |= tYSign.nVal & 0x80000000u;
			result = tOffset.flVal;
			const float z = x / y;
			result -= (n1 + n2 * z * z) * z;
		}
	}
	else if (y > 0.0f)
	{
		result = std::numbers::pi_v<float> / 2;
	}
	else if (y < 0.0f)
	{
		result = -std::numbers::pi_v<float> / 2;
	}
	return result;
}

MovingSolid::Collision::Collision(Type type_, int a_, int b_, Vec2<int> wallPos_) :
	type(type_),
	a(a_),
	b(b_),
	wallPos(wallPos_)
{

}

// TODO ULTIMATA: Save or recreate this somehow
std::unordered_map<int, MovingSolid> MovingSolid::solids;

void MovingSolid::CreateMovingSolid(Particle *parts, int pmap[YRES][XRES], int i, int type)
{
	int stateId = solids.size() + 1;
	while (solids.find(stateId) != solids.end())
	{
		stateId++;
	}

	std::vector<int> particles;

	Vec2<int> sum{ 0, 0 };
	int n = 0;

	FloodfillHelper(parts, (int)(parts[i].x + 0.5), (int)(parts[i].y + 0.5), pmap, stateId, particles, sum, n, type);

	// We found nothing, can occur when stamps trigger a creation
	if (n <= 0)
	{
		return;
	}

	solids[stateId] = MovingSolid(particles, type, stateId);
	solids[stateId].center = sum / n;
}

void MovingSolid::Reset(int x, int y, Particle *parts, int pmap[YRES][XRES], int stateId)
{
	if (x < 0 || y < 0 || x >= XRES || y >= YRES)
	{
		return;
	}

	auto id = ID(pmap[y][x]);
	if (parts[id].type != PT_MVSD || parts[id].tmp2 != stateId)
	{
		return;
	}

	parts[id].tmp2 = 0;

	Reset(x - 1, y, parts, pmap, stateId);
	Reset(x + 1, y, parts, pmap, stateId);
	Reset(x, y - 1, parts, pmap, stateId);
	Reset(x, y + 1, parts, pmap, stateId);
}

MovingSolid::MovingSolid(const std::vector<int> &particles_, int type_, int stateId_) : MovingSolid()
{
	particles = particles_;
	stateId = stateId_;
	type = type_;
}

void MovingSolid::Update(Simulation *sim)
{
	if (particles.empty())
	{
		return;
	}

	auto &parts = sim->parts;
	auto pmap = sim->pmap;

	// Collision handling
	// ---------------------------------------------
	bool shouldBounce = false; // Should the particle bounce off the surface or ignore bounce
	bool shouldFreeze = false; // Should the particle freeze? (ie on a surface) or ignore
	bool velocityIsFast = std::abs(velocity.X) > 0.5 || std::abs(velocity.Y) > 0.5; // Fast enough to bounce?

	Vec2<float> deflect{ 0.0f, 0.0f }; // Keep track of deflection dir, this is like a "net force" vector
	Vec2<int> totalRepel{ 0, 0 }; // Tracks direction to "repel" away from other particles / solids
	//float rotateSum = 0; // Keep track of total torque

	float velocityMag = std::hypot(velocity.X, velocity.Y);

	std::vector<MovingSolid*> movingSolidsToBounce; // Fake ass momentum conservation

	for (auto &i :collisions)
	{
		// Here we distinct between merely being "beside" and touching an object and actually colliding with it (ie, is there a normal force from the collision?)
		// If a particle exists 1px in the direction of the velocity vector, OR the displacement particle that detected the collision to the collided particle is nearly along the current velocity vector, we have collided

		if (i.b == -1)
		{
			// TODO ULTIMATA: MVSD wall collisions D:
			continue;
		}

		if (!parts[i.a].type || !parts[i.b].type)
		{
			continue;
		}

		Vec2<int> posA = { (int)(parts[i.a].x + 0.5f), (int)(parts[i.a].y + 0.5f) };
		Vec2<int> posB = { (int)(parts[i.b].x + 0.5f), (int)(parts[i.b].y + 0.5f) };

		// Determine if moving in the direction of vx or vy will intersect another particle
		// Since high speed collisions are handled by MVSD we can assume vx and vy are not that big and just check the location directly without doing a line intersection
		int testX = posB.X + isign(velocity.X);
		int testY = posB.Y + isign(velocity.Y);
		if (testX < 0 || testX >= XRES || testY < 0 || testY >= YRES)
		{
			continue;
		}

		auto r = pmap[testY][testX];
		if (!r)
		{
			continue;
		}

		// Should collision be super-bouncy? (If portal gel ctype)
		if (parts[i.a].ctype == PT_PGEL)
		{
			bigBounce = true;
		}

		MovingSolid *otherSolid = &solids[parts[i.b].tmp2];

		if ((posB - posA == Vec2{ isign(velocity.X), isign(velocity.Y) }) || (r && (TYP(r) != type || parts[ID(r)].tmp2 != stateId)))
		{
			bool notOtherIsSmallMVSD = i.type != Collision::Type::Moving || otherSolid->GetParticlesSize() > BIG_SOLID;

			if (velocityIsFast && notOtherIsSmallMVSD) // If we're approaching the other object at "high speed" then bounce
			{
				shouldBounce = true;
			}
			else if (!shouldBounce && notOtherIsSmallMVSD) // Since it's a collision and if the solid continues on its current velocity it will intersect another particle we mark that it should stop (since it's not bouncing)
			{
				shouldFreeze = true;
				totalRepel += posA - posB;
			}

			// Update net force direction
			deflect -= Vec2<float>{ posB - posA };

			// Update net torque
			// ???
		}

		// Collision with another moving solid
		// Determine if we should deflect it
		if (i.type == Collision::Type::Moving && velocityIsFast)
		{
			if (!otherSolid->velocity.X && !otherSolid->velocity.Y)
			{
				movingSolidsToBounce.push_back(otherSolid);
			}
		}
		else if (i.type == Collision::Type::NonStatic) // Collision with powder
		{
			if (parts[i.b].type == type)
			{
				continue;
			}

			parts[i.b].vx += velocity.X * 2;
			parts[i.b].vy += velocity.Y * 2;
		}

		// Make impact on fall
		if (impactPressure && collisions.size() != previousCollisionSize && velocityMag > MIN_VELOCITY_TO_DAMAGE)
		{
			sim->pv[posB.Y / CELL][posB.X / CELL] += MAX_IMPACT_PRESSURE * velocityMag / (1.141 * MAX_VELOCITY);
		}
	}

	// To prevent phasing through solids we negate solids that should be stopped (ie resting on a surface).
	// Every 50 frames we randomly impart a repeling velocity from the direction of objects it collided with (DO NOT CHANGE THE MAGIC CONSTANTS BELOW) to avoid particles being "stuck" inside one another (We also apply this repeling force during a force change)
	if (shouldFreeze)
	{
		if (sim->currentTick % 50 == 0 || overlap)
		{
			velocity = totalRepel / 15.0f / collisions.size();
		}
		else
		{
			velocity = { 0.0f, 0.0f };
		}
	}

	// Solid should bounce. If a dx or dy is indicated the solid will bounce anyways because that only happens if velocity is extremely large and solid risks phasing into solids
	float angle = FastAtan2(deflect.Y, deflect.X);

	if (!shouldFreeze && (usedX || usedY || shouldBounce))
	{
		// Only bounce if deflect is large enough, otherwise you might get extreme deflection angles from 1 or 2px of collision
		if (deflect.X || deflect.Y)
		{
			if (bigBounce)
			{
				velocityMag *= 2.0f;
			}
			velocity = Vec2{ std::cos(angle), std::sin(angle) } * velocityMag * BOUNCE;
		}
	}

	for (auto &i :movingSolidsToBounce)
	{
		i->velocity = Vec2{ std::cos(angle + std::numbers::pi_v<float>), std::sin(angle + std::numbers::pi_v<float>) } * velocityMag * BOUNCE;
	}

	// Limit max velocity
	velocity.X = restrict_flt(velocity.X, -MAX_VELOCITY, MAX_VELOCITY);
	velocity.Y = restrict_flt(velocity.Y, -MAX_VELOCITY, MAX_VELOCITY);


	// Update all individual particles
	// ---------------------------------------------
	bool particleRemoved = false;

	auto itr = particles.begin();
	while (itr != particles.end())
	{
		// Particle no longer exists, delete it
		if (parts[*itr].type != type || parts[*itr].tmp2 != stateId)
		{
			itr = particles.erase(itr);
			particleRemoved = true;
		}
		else
		{
			parts[*itr].x += velocity.X;
			parts[*itr].y += velocity.Y;

			itr++;
		}
	}

	// Trigger recalculation of groups if particles were removed
	// In case, for example, solid was cut
	if (particleRemoved && !particles.empty())
	{
		parts[particles[0]].flags = 1;
		parts[particles[0]].tmp3 = FromFloat(velocity.X);
		parts[particles[0]].tmp4 = FromFloat(velocity.Y);
	}

	CalcCenter(parts);
	center.X = std::clamp(center.X, 0, XRES - 1);
	center.Y = std::clamp(center.Y, 0, YRES - 1);

	// Reset everything for next cycle
	// ---------------------------------------------
	usedX = false;
	usedY = false;
	overlap = false;
	bigBounce = false;
	previousCollisionSize = collisions.size();
	collisions.clear();
	CalcForces(sim);
}

void MovingSolid::CalcCenter(Particle *parts)
{
	if (particles.empty())
	{
		return;
	}

	Vec2<int> sum{ 0, 0 };
	int count = 0;

	for (auto i : particles)
	{
		// Speed up calculation: for large solids only consider every 3rd particle
		if (particles.size() > BIG_SOLID && i % 3 == 0)
		{
			continue;
		}

		sum += Vec2{ (int)(parts[i].x + 0.5f), (int)(parts[i].y + 0.5f) };
		count++;
	}

	center = sum / count;
}

void MovingSolid::AddCollision(Collision c)
{
	collisions.push_back(c);
}

void MovingSolid::ShouldImpactPressure(bool val)
{
	impactPressure = val;
}

void MovingSolid::FlagOverlap()
{
	overlap = true;
}

void MovingSolid::FlagBigBounce()
{
	bigBounce = true;
}

int MovingSolid::GetParticlesSize()
{
	return particles.size();
}

int MovingSolid::GetStateID()
{
	return stateId;
}

void MovingSolid::FloodfillHelper(Particle *parts, int x, int y, int pmap[YRES][XRES], int stateId, std::vector<int> &toAddId, Vec2<int> &sum, int &count, int type)
{
	// Check if current spot is valid, if not return now
	// Invalid if not right type or already has ID (tmp2 > 0)
	if (x < 0 || x >= XRES || y < 0 || y >= YRES || parts[ID(pmap[y][x])].type != type || parts[ID(pmap[y][x])].tmp2 > 0)
	{
		return;
	}

	parts[ID(pmap[y][x])].tmp2 = stateId;
	toAddId.push_back(ID(pmap[y][x]));

	sum += Vec2{ (int)(parts[ID(pmap[y][x])].x + 0.5), (int)(parts[ID(pmap[y][x])].y + 0.5) };
	count++;

	if (count >= MAX_SOLID_SIZE) // Solids larger than this cause stack overflow lol
	{
		return;
	}

	FloodfillHelper(parts, x - 1, y, pmap, stateId, toAddId, sum, count, type);
	FloodfillHelper(parts, x + 1, y, pmap, stateId, toAddId, sum, count, type);
	FloodfillHelper(parts, x, y - 1, pmap, stateId, toAddId, sum, count, type);
	FloodfillHelper(parts, x, y + 1, pmap, stateId, toAddId, sum, count, type);
}

void MovingSolid::CalcForces(Simulation *sim)
{
	force = { 0.0f, 0.0f };

	// Gravity (we can pretend gravity acts on the center of mass)
	sim->GetGravityField(center.X, center.Y, GRAVITY, NEWTON_GRAVITY, force.X, force.Y);

	// Pressure
	force += Vec2{ sim->vx[center.Y / CELL][center.X / CELL], sim->vy[center.Y / CELL][center.X / CELL] } * AIR;

	// Stasis field (Yes the checks are needed)
	auto strength = sim->stasisStrength[center.Y / STASIS_CELL][center.X / STASIS_CELL];
	if (center.X >= 0 && center.X < XRES && center.Y >= 0 && center.Y < YRES && strength)
	{
		force.X += (sim->stasisVX[center.Y / STASIS_CELL][center.X / STASIS_CELL] - velocity.X) * strength;
		force.Y += (sim->stasisVY[center.Y / STASIS_CELL][center.X / STASIS_CELL] - velocity.Y) * strength;
	}

	// Apply the net force (it's really an acceleration tbh)
	velocity += force;
}
