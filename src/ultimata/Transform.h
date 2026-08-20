#pragma once

#include <cmath>
#include <map>
#include <vector>

#include "common/Vec2.h"
#include "gui/interface/Colour.h"
#include "SimulationConfig.h"

using PixelVector = std::vector<std::pair<Vec2<int>, ui::Colour>>;
using ElementVector = std::vector<std::pair<Vec2<int>, int>>;

void Rotate(Vec2<int> &pos, float rotation);
void Rotate(Vec2<float> &pos, float rotation);
void RotateAround(Vec2<int> &pos, Vec2<int> cpos, float rotation);
void RotateAround(Vec2<float> &pos, Vec2<float> cpos, float rotation);

int MapKey(int x, int y);

/**
 * Rotate a pixel vector by float radians, and write the result to
 * the specified pixel array (which will be cleared). This is a simple
 * approximation and should not be used for anything where accuracy is
 * needed
 */
template <typename T>
void RotateVector(const T &base, float angle, T &out, Vec2<int> cpos = Vec2<int>{ 0, 0 })
{
	out.clear();
	for (auto &p : base)
	{
		Vec2<int> &pos = p.first;
		RotateAround(pos, cpos, angle);
		out.push_back(std::make_pair(pos, p.second));
	}
}

/**
 * Rotate a pixel vector by scaleX and scaleY, and write the result to
 * the specified pixel array (which will be cleared). This is a simple
 * approximation and should not be used for anything where accuracy is
 * needed. ScaleX and ScaleY can be negative (flips it)
 *
 * Width and height are original (not scaled) selection box width and height
 * ocx and ocy are original selection center, not translated center (which is cx, cy)
 *
 * T  = array type (ie PixelArray)
 * T2 = 2nd array pair type, (ie ui::Colour)
 */
template <typename T, typename T2>
void ScaleVector(const T &base, float scaleX, float scaleY, T &out, int width, int height, float cx = 0.0f, float cy = 0.0f, float ocx = 0.0f, float ocy = 0.0f)
{
	out.clear();

	// Create hashmap of coords
	std::map<std::pair<int, int>, T2> coordMap;
	for (auto &p : base)
	{
		coordMap[std::make_pair(p.first.X, p.first.Y)] = p.second;
	}

	// Iterate all coords in bounding box
	int x1 = std::max(0.0f, cx - width / 2 * scaleX);
	int x2 = std::min((float)XRES, cx + width / 2 * scaleX);
	if (x2 < x1)
	{
		std::swap(x1, x2);
	}

	int y1 = std::max(0.0f, cy - height / 2 * scaleY);
	int y2 = std::min((float)YRES, cy + height / 2 * scaleY);
	if (y2 < y1)
	{
		std::swap(y1, y2);
	}

	int tx, ty;
	std::pair<int, int> c;
	for (auto x = x1; x < x2; x++)
	{
		for (auto y = y1; y < y2; y++)
		{
			tx = std::round((x - cx) / scaleX + ocx);
			ty = std::round((y - cy) / scaleY + ocy);
			c = std::make_pair(tx, ty);

			if (coordMap.find(c) != coordMap.end())
			{
				out.push_back(std::make_pair(Vec2{ x, y }, coordMap[c]));
			}
		}
	}
}
