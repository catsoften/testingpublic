# TANK
### KV-2 Soviet Tank

![Element](https://i.imgur.com/CtfINW0.gif)

**Description:**  Soviet Tank. STKM can ride, comes with several upgrades. Hold down to shoot.

A rideable tank. Can be damaged by heat or collisions, will dissolve into a tank shaped pile of powder when destroyed. Any STKM can ride it, if STKM or STKM2 enters you can control with keyboard. If FIGH enters it will try to kill STKM by running him over or by using the gun. FIGH will target STKM inside of other vehicles.

If player enters the tank with rocket boots the cybertruck will gain a plasma flamethrower. (hold down). If the player has ctype FIRE the cybertruck gets a flamethrower weapon (hold down), if the player has ctype BOMB the cybertruck can shoot BOMB shells (again, hold down). By default it shoots FSEP charges.

Controls:
```
LEFT  - Go left
RIGHT - GO right
UP    - Get out of tank
DOWN  - Shoot
```

Tank will emit sparks and fire when damaged. TANK can instantly crush cybertrucks. Tanks can also run over or crush groups of STKM elements if they're fast enough.



## Technical
The tank is just a 1 px powder, the tank is entirely a graphic. It does some hacky collision detection to mimic rotation.

Note that any heat damage can only be detected by the core (the center pixel), so for "realism" turn on ambient heat. Will instantly die if its temperature goes over 3000 K, otherwise if above 473.15 K decreases 1 HP per frame.


#### Properties
**vx, vy:** Velocity of the tank

**ctype:** Element of STKM when it entered

**tmp2:** Which STKM controls it (1 = STKM, 2 = SKTM2, 3 = FIGH, 0 = Empty)

**tmp:** Equipped upgrade, 0 = FSEP gun, 1 = plasma flamethrower, 2 = flamethrower, 3 = bomb gun

**life:** Health, starts at 600.

**tmp3:** Rotation in radians, 0 = flat facing to left

**tmp4:** Direction of travel, 0 = left, 1 = right
