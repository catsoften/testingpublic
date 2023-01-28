# VOLT

![Voltage source](https://i.imgur.com/liLlNt8.png)

**Description:**  *Voltage source. Use PSCN/COPR and NSCN/ZINC for terminals. Set output voltage with tmp3.*

Set the tmp3 for voltage. If there is a blob of voltage sources, the average voltage over the blob
(at least the region the circuit skeleton passes through) will be used. Voltage sources must be connected on 
both ends, current will flow out of the PSCN/COPR (+) end and into the NSCN/ZINC (-) end.

Voltage sources will melt at 2000 C. VOLT doesn't do anything for SPRK.

## Technical
#### Properties
**tmp3:** Voltage (in volts)
