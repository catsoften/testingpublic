#include "simulation/ElementCommon.h"
#include "simulation/ToolCommon.h"

#include "ultimata/ElementUtils.h"

static int perform(SimTool *tool, Simulation * sim, Particle * cpart, int x, int y, int brushX, int brushYy, float strength);

void SimTool::Tool_FAST() {
    Identifier = "DEFAULT_TOOL_FAST";
    Name = "FAST";
    Colour = 0xff7438_rgb;
    Description = "Speed up local time.";
    Perform = &perform;
}

static int perform(SimTool *tool, Simulation * sim, Particle * cpart, int x, int y, int brushX, int brushYy, float strength) {
    TimeDilation(sim, x, y, 1, strength * MAX_TIME_DILATION);
    return 1;
}
