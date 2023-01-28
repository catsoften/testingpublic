# FISH

![Element](https://i.imgur.com/gjeBCnw.gif)

**Description:**  *Freshwater fish, swims in water and eats ANT and SEED.*

A powder that swims around randomly. Normally will only swim either vertically or horizontally, but may swim on diagonals to search for SEED and ANT. FISH can sense 2 px around them (a 5x5 pixel square centered at the FISH). FISH will try to avoid going to AIR spots.

Won't die in SLTW or DSTW or SWTR, but buoyancy may be affected.

If fish temperature is greater than 90 C, or after a random amount of time that its not touching any type of water, it'll "die" and turn into DUST. In theory, FISH can "swim" in any liquid, but it'll most likely die before doing so if its not water.

## Technical

#### Properties
**tmp3:** x velocity (-1, 0 or 1) since it moves by swapping, not normal particle movement.

**tmp4:** y velocity (-1, 0 or 1) since it moves by swapping, not normal particle movement.
