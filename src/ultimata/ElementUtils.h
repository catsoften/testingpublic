#pragma once

#include "common/Vec2.h"

int FromFloat(float x);
float ToFloat(int x);

class Renderer;
class Simulation;

Vec2<int> IntersectLine(Simulation *sim, int sx, int sy, float vx, float vy, int type = 1, int type2 = 0);
void DrawGlowyPixel(Renderer *ren, int x, int y, int colr, int colg, int colb, int cola);
void TimeDilation(Simulation *sim, int x, int y, int radius, int val);
