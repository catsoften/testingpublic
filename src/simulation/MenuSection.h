#pragma once
#include "common/String.h"

struct menu_section
{
	String::value_type icon;
	String name;
	int itemcount;
	int doshow;
};

constexpr int SC_WALL      =  0;
constexpr int SC_ELEC      =  1;
constexpr int SC_POWERED   =  2;
constexpr int SC_ELECTROMAG=  3;
constexpr int SC_SENSOR    =  4;
constexpr int SC_FORCE     =  5;
constexpr int SC_EXPLOSIVE =  6;
constexpr int SC_GAS       =  7;
constexpr int SC_LIQUID    =  8;
constexpr int SC_POWDERS   =  9;
constexpr int SC_SOLIDS    = 10;
constexpr int SC_ORGANIC   = 11;
constexpr int SC_NUCLEAR   = 12;
constexpr int SC_SPECIAL   = 13;
constexpr int SC_RANDOM    = 14;
constexpr int SC_LIFE      = 15;
constexpr int SC_TOOL      = 16;
constexpr int SC_FAVORITES = 17;
constexpr int SC_DECO      = 18;
