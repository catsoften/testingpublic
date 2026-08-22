#include "simulation/ElementCommon.h"
#include "simulation/ToolCommon.h"

#include "ultimata/ElementUtils.h"

static int perform(SimTool *tool, Simulation * sim, Particle * cpart, int x, int y, int brushX, int brushYy, float strength);

void SimTool::Tool_SLOW() {
    Identifier = "DEFAULT_TOOL_SLOW";
    Name = "SLOW";
    Colour = 0x73e8fa_rgb;
    Description = "Slow down local time.";
    Perform = &perform;
}

static int perform(SimTool *tool, Simulation * sim, Particle * cpart, int x, int y, int brushX, int brushYy, float strength) {
    TimeDilation(sim, x, y, 1, strength * MIN_TIME_DILATION);
    return 1;
}
