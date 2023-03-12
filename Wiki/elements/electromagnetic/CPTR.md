# CPTR

![Really bad gif lol](https://i.imgur.com/siF5vv7.gifv)

**Description:**  *Polarized electrolytic capacitor. Stores and releases charge. tmp3 = capacitance.*

Functions like an IRL capacitor. Is polarized so use PSCN / COPR as the positive terminal (INPUT) and NSCN / ZINC as negative terminal (output). When a current flows through the capacitor it will charge up until it reaches a limiting value (slows down as it approaches this limit). When not charging it discharges exponentially, acting as a voltage source.

Like IRL capacitors, the lower the capacitance the faster it charges, but higher capacitance stores more energy.

If the current is too great (10000 A) or the current is wrong direction and > 1 A, the capacitor will explode.

Capacitors explode based on the following rule set:
```
< 0.01 capacitance: Sizzle into SMKE, very small temp and pressure increase.
< 10 capacitance: Explode into steam and BRMT
< 100 capacitance: Explode into fire and BRMT
else: Explode into superheated EXOT
```
(EXOT implies you somehow made an electrolytic capacitor using EXOT (I mean how else did you get such high capacitance?)

Capacitors melt at 1273.0 K.

Capacitor also works for SPRK, any SPRK touching any single particle will charge it (only accepts SPRK from a positive terminal),
and discharges to negative terminal.


## Technical
Listed as PT_CAPR in the code.

#### Properties
**tmp:** SPRK stored (for use as SPRK battery)

**tmp2:** If != 0 will explode

**tmp3:** Capacitance (in farads)

**tmp4:** Voltage drop across capacitor, updated based on current.
