# CYTK
### Cybertruck

[Click here (Large image)](https://imgur.com/b0JLPg8)


**Description:**  Tesla Cybertruck. STKM can ride, comes with several upgrades.

A rideable cybertruck. Can be damaged by heat or collisions, will dissolve into a cybertruck shaped pile of powder when destroyed. Any STKM can ride it, if STKM or STKM2 enters you can control with keyboard. If FIGH enters it will try to kill STKM by running him over or by using the gun. FIGH will target STKM inside of other cybertrucks.

If player enters the cybertruck with rocket boots the cybertruck will gain cyberthrusters and be able to fly (hold down). If the player has ctype FIRE the cybertruck gets a Boring Company Flamethrower weapon (hold down), if the player has ctype BOMB the cybertruck can shoot BOMB (again, hold down).

Controls:
```
LEFT  - Go left
RIGHT - GO right
UP    - Get out of cybertruck
DOWN  - Activate ability (fly or gun if equipped)
```

Cybertrucks will emit sparks and fire when damaged. Cybertrucks will instantly be crushed by TANK. Cybertrucks can also run over or crush groups of STKM elements if they're fast enough.



## Technical
The cybertruck is just a 1 px powder, the truck is entirely a graphic. It does some hacky collision detection to mimic rotation. Also the cybertruck heals when electrons touch the cybercore (the pixel in the center of the cybertruck, yeah its small), like how STKM heals from PLNT.

Note that any heat damage can only be detected by the cybercore (the center pixel), so for "realism" turn on ambient heat. Will instantly die if its temperature goes over 1000 K, otherwise if above 473.15 K decreases 1 HP per frame.


#### Properties
**vx, vy:** Velocity of the cybertruck

**ctype:** Element of STKM when it entered

**tmp2:** Which STKM controls it (1 = STKM, 2 = SKTM2, 3 = FIGH, 0 = Empty)

**tmp:** Equipped upgrade, 0 = none, 1 = cyberthruster, 2 = flamethrower, 3 = bomb artillery

**life:** Health, starts at 100, goes at most 120.

**tmp3:** Rotation in radians, 0 = flat facing to left

**tmp4:** Direction of travel, 0 = left, 1 = right
