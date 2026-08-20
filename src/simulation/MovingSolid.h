#pragma once

#include <unordered_map>
#include <vector>

#include "common/Vec2.h"
#include "ElementClasses.h"
#include "Particle.h"
#include "Simulation.h"
#include "SimulationConfig.h"

float FastAtan2(float y, float x);

// A group of particles (a moving solid)
class MovingSolid
{
public:
	constexpr static float GRAVITY = 0.09f;
	constexpr static float NEWTON_GRAVITY = 0.14f;
	constexpr static float AIR = 0.05f; //0.006f;
	constexpr static float BOUNCE = 0.5f;

	constexpr static float MIN_VELOCITY_TO_DAMAGE = 3.0f;
	constexpr static float MAX_VELOCITY = 15.0f;

	constexpr static float MAX_IMPACT_PRESSURE = 10.0f;

	constexpr static int BIG_SOLID = 10; // Min size for a moving solid to do certain checks, leave it small (<20)
	constexpr static int MAX_SOLID_SIZE = 17176; // Should be no larger than 17176 or it will stack overflow on large solids


	// A collision point
	struct Collision
	{
		// Collision types
		enum class Type
		{
			Static,
			NonStatic,
			Moving,
		};

		Type type;
		int a, b;
		Vec2<int> wallPos;

		Collision(Type type_, int a_, int b_, Vec2<int> wallPos = { -1, -1 });
	};


	static std::unordered_map<int, MovingSolid> solids; // Hash map of all moving solids

	static void CreateMovingSolid(Particle *parts, int pmap[YRES][XRES], int i, int type = PT_MVSD); // Floodfill creation
	static void Reset(int x, int y, Particle *parts, int pmap[YRES][XRES], int stateId);


	Vec2<int> center = { 0, 0 };
	Vec2<float> velocity = { 0.0f, 0.0f };

	MovingSolid() = default;
	MovingSolid(const std::vector<int> &particles_, int type_, int stateId_);

	void Update(Simulation *sim);
	void CalcCenter(Particle *parts); // Recalculate the center of mass
	void AddCollision(Collision c);

	void ShouldImpactPressure(bool val);
	void FlagOverlap();
	void FlagBigBounce();

	int GetParticlesSize();
	int GetStateID();

private:
	static void FloodfillHelper(Particle *parts, int x, int y, int pmap[YRES][XRES], int stateId, std::vector<int> &toAddId, Vec2<int> &sum, int &count, int type = PT_MVSD); // Floodfill creation


	std::vector<int> particles;

	int type, stateId;

	Vec2<float> force = { 0.0f, 0.0f };

	bool usedX = false, usedY = false;

	unsigned int previousCollisionSize = 0;

	bool impactPressure = false; // Make pressure on impact?
	bool overlap = false; // Is the MVSD phased into another particle?
	bool bigBounce = false; // Did it hit repulsion gel?

	std::vector<Collision> collisions; // Collision handling, saves where the solid has collided with other blocks

	void CalcForces(Simulation *sim); // Compute forces due to gravity, pressure, etc...
};
