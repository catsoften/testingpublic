#include "Transform.h"

// Rotate around origin
void Rotate(Vec2<int> &pos, float rotation)
{
	if (std::abs(rotation) < 0.01)
	{
		return;
	}
	int x2 = std::round(pos.X * std::cos(rotation)) - std::round(pos.Y * std::sin(rotation));
	int y2 = std::round(pos.Y * std::cos(rotation)) + std::round(pos.X * std::sin(rotation));
	pos = { x2, y2 };
}

void Rotate(Vec2<float> &pos, float rotation)
{
	if (std::abs(rotation) < 0.01)
	{
		return;
	}
	float x2 = pos.X * std::cos(rotation) - pos.Y * std::sin(rotation);
	float y2 = pos.Y * std::cos(rotation) + pos.X * std::sin(rotation);
	pos = {x2, y2};
}

// Rotate around a point
void RotateAround(Vec2<int> &pos, Vec2<int> cpos, float rotation)
{
	pos -= cpos;
	Rotate(pos, rotation);
	pos += cpos;
}

void RotateAround(Vec2<float> &pos, Vec2<float> cpos, float rotation)
{
	pos -= cpos;
	Rotate(pos, rotation);
	pos += cpos;
}

// 2 ints -> 1 unique int
int MapKey(int x, int y)
{
	return (x + y) * (x + y + 1) / 2 + y;
}
