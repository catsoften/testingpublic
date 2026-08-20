#pragma once
#include "simulation/ElementDefs.h"

int Element_FILT_graphics(GRAPHICS_FUNC_ARGS);
void Element_FILT_create(ELEMENT_CREATE_FUNC_ARGS);
int Element_FILT_getWavelengths(const Particle* cpart);
int Element_FILT_interactWavelengths(Simulation *sim, Particle* cpart, int origWl);
