# BIRD

![Element](https://i.imgur.com/vOou4Gl.gif)

**Description:**  *Birds, flies in flocks, eats ants, fish, and seeds. Flammable.*

A powder that flies around in flocks. Will head towards and eat SEED, ANT and FISH. At above 6.0f pressure turns into BLOD, below -20 C or above 90 C will turn into DUST. Birds will avoid objects above 90 C, and the flock will scatter when near dangerous elements such as FIRE or BOMB. Birds can sense objects in a 5px range around them (square range, a 11x11 square centered on the BIRD)

Birds cannot see GLAS and will die if they crash into GLAS at high speeds, turning into BLOD. Birds turn into DUST when near SPRK or LIGH. Randomly, birds might decide to perch onto solid objects for a random amount of time. Birds will avoid the edges of the screen.

The behavior algorithim for BIRD is a simple boid flocking algorithim (For instance, see [here](http://www.vergenet.net/~conrad/boids/pseudocode.html)). If the bird isn't near any other birds or particles, it will randomly change its velocity every so often.

## Technical

#### Properties
**life:** Does the bird currently "want" to perch?

**tmp:** Is the bird currently perching.

**tmp2:** Time left to perch, when this reaches -1 stops perching.

**tmp3:** Spawn x location, unused

**tmp4:** Spawn y location, unused
