# BEE

![Element](https://i.imgur.com/zvNK2x6.gif)

**Description:**  *Bees. Pollinates PLNT, makes honey and wax hives where it spawns.*

An element that builds hexagonal WAX honeycombs near where it spawns, and looks for PLNT, SUGR, SWTR and WATR to bring back to the hive to make HONY. Freezes into SNOW below -5 C and turns to DUST above 50 C. BEEs are instantly killed by nearby LIGH and SPRK, and will avoid solids and hot objects (other than PLNT, WAX, etc..). BEEs will try to "regulate" the temperature of their hive to around 30 C, although this process is slow and inefficent. BEEs will try to avoid the edges of the screen.

Bees are seperated into 3 types:

**Builder:** Constructs wax honeycombs near where it spawned (At most in a 41x41 px cube). Will not make honeycombs within 2 px of other solids ("Bee space").

**Finder:** Flies away from spawn, looking for WATR, SWTR, SUGR (Which they will "consume"), or PLNT. Once resources are retrieved they fly back to the hive and make honey in the wax honeycombs. There is a chance they'll pollinate the PLNT and cause it to grow like VINE. If it doesn't find any PLNT for a long time (1/5000 chance per frame) it can decide to "give up" and return to the hive.

**Queen:** About 1/500 bees spawned are a "queen", every 500 frames they try to spawn a random worker. Queen bees are 1 px larger than regular bees, although they're hard to spot since they don't ever leave their spawn point.

Bees will swarm around STKM and STKM2 within 90 px, causing slight damage. If a BEE is near SMKE, it will be rendered "passive" for 100 frames and will not try to swarm the player. Bees will not swarm around FIGH because they're racist.

Bees can move freely through HONY and WAX.

Note that you can't decorate BEE with deco color.



## Technical
#### Dance
Upon successfully finding a food source, BEEs will memorize the relative distance and angle and return to the hive, where they dance for a random portion of time (in general, longer distances indicate longer dances) by flying in circles. Any BEEs nearby that see this dance and are able to go find the food will memorize the angle and fly along that angle.

The dance data is stored in the tmp variable in this format:
```
DDDDDD    AAX
Distance  Angle
```
For example, a tmp of 2301013 means there's food 2301 pixels away from the hive at an angle of 13 degrees (mathematicans polar coordinates, 0 = right, 90 = up). The last digit indicates whether the BEE should "pass" the information on to other bees, or whether the bee "recieved" the information and is going to act on it. If the tmp is divisible by 2 then the bee is returning and passing the knowledge, otherwise it recieved the knowledge.


#### Properties
**tmp:** Stores angle, distance, and condition for "dance". See dance above.

**tmp2:** Type, 0 = "builder" (Builds the hive), 1 = "finder" (Finds PLNT and other food, makes HONY), 2 = "queen" (Spawns bees)

**life:** Pollen level, returns to hive when it reaches 100.

**tmp3:** Spawn x coordinate

**tmp4:** Spawn y coordinate

**flags:** Frames to remain passive for. DOES NOT SAVE

**dcolour:** Color (0 to 6 inclusive)
