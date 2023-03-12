# GNSH

###  UEF T3 Heavy Gunship (Broadsword)

[Image (warning, large!)](https://imgur.com/ihlxrss)

**Description:**  *UEF T3 Heavy Gunship. Now rideable! UP = Shoot, DOWN = Fly, L + R + D = exit*

Another rideable vehicle for STKM and FIGH. LEFT and RIGHT = move left and right, UP = FIRE (and hover, it will shoot at current mouse position), DOWN = fly up. When flying thrusters accelerate the air around it. Any BIRD that flies too close to the thrusters will turn into BLOD. Press LEFT, RIGHT and DOWN at the same time to exit the gunship.

By default, the gunship fires guided missiles. There is a 2nd weapon (microwave laser) but you need to set it manually by typing
```
!set tmp GNSH 1
```
in the console (There is no way to get it without modifying tmp value directly). Gunships have 5000 HP and are damaged by temperatures above 9000 C. When damaged, gunships produce embers and later fire, to show damage level (at 1000 life and 300 life respectively).

FIGH can control gunship, but its AI sucks and it might fly out of the screen.


## Technical
The gunship is just a 1 px powder, the gunship is entirely a graphic. It does some hacky collision detection to mimic rotation.

Note that any heat damage can only be detected by the core (the center pixel), so for "realism" turn on ambient heat. If above 9000 C decreases 1 HP per frame.


#### Properties
**vx, vy:** Velocity of the gunship

**ctype:** Element of STKM when it entered

**tmp2:** Which STKM controls it (1 = STKM, 2 = SKTM2, 3 = FIGH, 0 = Empty)

**tmp:** Equipped upgrade, 0 = Guided missiles, 1 = Maser

**life:** Health, starts at 5000.

**tmp3:** Rotation in radians, 0 = flat facing to left

**tmp4:** Direction of travel, 0 = left, 1 = right
