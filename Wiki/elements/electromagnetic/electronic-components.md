
**Known bugs with electronic components:**

 Please do not submit a bug report about these bugs they are hard to fix :(
```
- Water under high voltages sometimes turns blood red
- Capacitor is just a resistor that sometimes turns into a battery
- Inductor is just a resistance-reversed capacitor
- Putting high resistance objects near low resistance ones causes entire circuit to burst into flames
- Putting lots of capacitors near each others creates absurd voltages and causes circuit to burst into flames
```
  
  

## VOLT (Voltage source)

Outputs a constant voltage (given by its tmp3 value) to any conductors touching it. (Default = 10 V).

Cannot be melted or destroyed by pressure.

  

## CPTR (Capacitor)

Somewhat mimics a real capacitor, charges under current and discharges slowly depending on its capacitance. Actually just a resistor, that when charging that increases resistance exponentially, and then outputs the highest voltage experienced, decaying exponentially, when no input current is found.

tmp3 sets the capacitance value, higher capacitance = slower decay and increase. CPTR should only be used 1px at a time, CPTR won't conduct to other CPTR.

Melts at 1000.0 C.

## INDC (Inductor)

Somewhat mimics a real inductor, resists sudden changes in current. Actually just a resistor, that when charging that decreases resistance exponentially, and then outputs the highest voltage experienced, decaying exponentially, when no input current is found.

tmp3 sets the inductance value, higher inductance = slower decay and increase. INDC should only be used 1px at a time, INDC won't conduct to other INDC.

Melts at 1000.0 C.
  

## MMSH

Metallic mesh, blocks life (SPDR, BEE, ANT, BIRD, FISH) and allows liquids, powders, gases and all energy particles through. Liquids

and powders might randomly be stopped for a single frame while passing through. Doesn't get sparked by electron.
