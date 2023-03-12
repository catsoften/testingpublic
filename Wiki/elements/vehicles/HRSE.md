# HRSE
### Horse

TODO IMAGE


**Description:**  Horse. Can be processed into GLUE, STKM can ride it, press down to dismount.

In passive mode, horses will bend down and eat anything edible if there is, healing itself. Otherwise it randomly walks and neighs. 
Horses can be ridden by STKM (complete with rocket upgrade), press up to jump / rocket, down to dismount. If a horse is near pressure (> 0.2), deadly elements (PROP_DEADLY), or heat (> 60 C), it will gallop away from the danger, overriding any commands.

When a horse dies in water > 50 C it will turn into GLUE. Otherwise, horses will turn into a pile of DUST. Horses bleed BLOD when life < 25.
Yes the back leg joints on the horse are backwards, but they were hard and I'm not good enough at coding to fix it.

## Technical
The horse is just a 1 px powder, the horse is entirely a graphic. It does some hacky collision detection to mimic rotation.

Note that any heat damage can only be detected by the core (the center pixel), so for "realism" turn on ambient heat. 


#### Properties
**vx, vy:** (velocity)

**ctype:** = element of STKM when it entered

**tmp2:** = which STKM controls it (0 = AI horse, 1 = STKM, 2 = STK2, 3 = FIGH)

**tmp | 1:** = STKM rocket boot state or such

**tmp | 2:** = Direction of travel (true = right, false = left)

**tmp | 4:** = Fleeing to left

**tmp | 8:** = Fleeing to right

**life:** = health

**tmp3:** = rotation

**tmp4:** = neck rotation
