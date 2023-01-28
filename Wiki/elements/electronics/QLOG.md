# QLOG
### Quantum Logic Gate

![Element](https://i.imgur.com/5qjNoYA.gif)

*A quantum full adder*

**Description:**  Quantum Logic Gate. Fires a beam that alters ION like ARAY. Ctype determines logic gate type, see github wiki.

The quantum logic gate fires a beam at ION, modifying its quantum state. QLOG can fire through IONs or other QLOG, but the beam will be blocked by any other elements. To activate QLOG, simply SPRK it. It's recommended to use **NICH** (nichrome wire), as it conducts SPRK 1 px at a time, unlike other metals in which the SPRK jumps in 3 px increments. This way, you can make sure your gates fire in the correct order.

> (Because ~~the dev was lazy~~ of the no-cloning theorem, quantum states DO NOT presist when the save is reloaded, nor do they save!)

> Quantum states are at most 6 qbits big, any larger states will instantly decohere. This is because the number of computations required grows in powers of two for the number of qbits. There can be at most 1000000000 quantum states present in a sim at one time.

Note that it does NOT fire a beam opposite where it was sparked. Rather, it fires a beam in the direction of its tmp value. To select a direction to aim the beam, set the `tmp` value.
```
0: LEFT  1: UP
2: RIGHT 3: DOWN
```

Gates, when sparked, will also activate any gates adjacent to it in a direction perpendicular to its beam direction. For example, if the gates fire to the left, when the gate is sparked, all connected gates in the same column are activated at the same time. **NOTE: SPRK can only be detected if its directly touching the QLOG (not diagonals)** There is a 5 frame cooldown once QLOG is SPRKed before it can be SPRKed again.

Some gates require more than 1 particle of QLOG. In such gates, QLOG will search for other particles of its ctype in a direction perpendicular to its beam direction. You have to number the other inputs using the tmp2 value, for example, if I had a 3 qbit gate all firing left, and I wanted (top down order) the inputs to be 3 1 2, I could make a column of them like this:
```
ION <-- [tmp2 = 2]
ION <-- [tmp2 = 0]
ION <-- [tmp2 = 1]
```
*Note: quantum gate inputs are 0-indexed*

You can also put other gates between the multi-gate inputs, that won't affect the search.

---

The gate type is determined by its ctype, below are the different types. They are (mostly) all represented by matrices, so you can go on wikipedia and get the matrices used.

**ctype = 0: Filler gate**
Just a filler gate, you can use this to maintain connections between gates that are not adjacent in the circuit. 

**ctype = 1: Pauli X Gate**
Rotates the state 180 degrees around the x axis. You can think of it as a quantum NOT gate. You can use it at the beginning of your circuit to change the |0⟩ states to |1⟩.

**ctype = 2: SWAP Gate (2 qbit gate)**
Swaps the quantum states of 2 particles

**ctype = 3: CNOT Gate (2 qbit gate)**
Negates the 2nd qbit only if the first qbit is true (|1⟩)

**ctype = 4: CCNOT Gate (3 qbit gate)**
Negates the 3rd qbit only if the first and second qbits are true (|1⟩)

**ctype = 5: CSWAP Gate (3 qbit gate)**
Swaps the 2nd and 3rd qbit only if the first qbit is true (|1⟩)

**ctype = 6: OBSERVE Gate**
Measures the quantum state of an ION. (Assumes measurement in the |0⟩ and |1⟩ basis). Will collapse the quantum state, meaning if you measure an ambigious state again it will always be the same result. Supports quantum entanglement. Will output to any nearby conductors or WIFI (not on the diagonals though)

**ctype = 7: H Gate**
Puts a qbit into a superposition with equal probability of being |0⟩ or |1⟩. The H gate is its own inverse.

**ctype = 8: Y Gate**
Rotate state 180 degrees along the Y axis

**ctype = 9: Z Gate**
Rotate state 180 degrees along the Z axis

**ctype = 10: S Gate**
Rotate state 90 degrees along the Z axis

**ctype = 11: S-inverse Gate**
Rotate state backwards 90 degrees along the Z axis

**ctype = 12: T Gate**
Rotate state 45 degrees along the Z axis

**ctype = 13: T-inverse Gate**
Rotate state backwards 45 degrees along the Z axis

**ctype = 14: SQRT NOT Gate**
Does some weird stuff, if you apply it twice you get NOT.

**ctype = 15: RESET Gate**
You'll probably want one of these at the beginning of your circuit, it resets the ION's quantum state back to |0⟩, and breaks any entanglement present.

> For a tutorial on quantum computing, [see our blog](https://hellomouse.net/blog/quantum0.html)


## Technical
#### Properties
**ctype:** Gate type, see above

**tmp:** Beam direction, see above

You should *reeeallllyy* not touch these, these are mostly temp variables that get used for 1 frame:

**flags:** For multi-qbit gates that has a tmp2 > 1 (not the first input), stores particle ID of the first input.

**tmp2:** For multi-qbit gates, stores which input the particle is (0 = INPUT 1, 1 = INPUT 2, 2 = INPUT 3)

**tmp3:** For multi-qbit gates, particle ID of the 2nd input if current gate is 1st input

**tmp4:** For multi-qbit gates, particle ID of the 3rd input if current gate is 1st input

