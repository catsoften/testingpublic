# TRBN

### Turbine

[Click here for image (Warning: large)](https://i.imgur.com/L27QpB8.gif)

**Description:**  *Turbine. Makes SPRK when in moving air or particles. tmp3 = air velocity, tmp4 = particle velocity.*

Sparks any adjacent conductors when it detects an air velocity greater than its tmp3 or a particle with velocity greater than its tmp4. (Default: 0.3f and 1.0f respectively.) TRBN allows all particles and air pressure to pass without disturbance. When it reaches the required speed, TRBN will begin animating in a strip pattern (pretend it's "rotating").

Note that TRBN only sparks adjacent conductors, so if you have TRBN off to the side it won't activate because another connected TRBN is activated. The metal must be adjacent to an *active* turbine. At 2000 C TRBN melts into molten METL.

Spinning TRBN will turn BIRD into BLOD.


## Technical
#### Properties
**tmp:** Used for graphics

**tmp3:** Min air speed to activate

**tmp4:** Min particle speed to activate
