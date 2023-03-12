# INDC

![Really bad gif lol](https://i.imgur.com/jkdjZHv.gifv)

**Description:**  *Inductor. Resists sudden changes in current. tmp3 = inductance.*

An inductor effectively functions as a current source, forcing the current to slowly change 
to its final value rather than suddenly change. Inductors are not polarized and do not have a max voltage 
or anything (although it may still melt from ohmaic heating). Like IRL inductors, the greater the inductance,
the slower the change to current is. Note that because the time constant is L/R, a large resistance will cause 
a shorter time, unlike a capacitor.

Inductors have a 100 frame cool down for SPRK, disallowing a lot of SPRK at a time through.

Inductors melt at 1085 C.

## Technical
#### Properties
**tmp3:** Inductance (in henries)

**tmp4:** Current value of inductor (in amps)
