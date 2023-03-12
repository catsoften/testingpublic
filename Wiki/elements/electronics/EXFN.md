# EXFN
### Excursion Funnel

![Element](https://i.imgur.com/1pFXrBd.gif)
*A horizontal beam emitter (Vertical formation)*

**Description:**  Excursion Funnel. Creates a stasis beam depending on tmp. Toggle reverse with GOLD / TTAN

Like the excursion funnel from *Portal 2*, it will create a funnel in the direction where particles will be carried towards or away from it. Toggle ON with PSCN, OFF with NSCN. If toggled with GOLD particles will move towards the emitter, if toggled with TTAN will move away.

Like many powered elements, you can adjust the strength by changing its temp from -256 C to 256 C, which corresponds to a stasis strength value of 0 to 1.6.

The beam will pass through non-solids and will stop upon reaching a solid element or the edge of the screen (Except GLAS). Any non-solid element will be moved by the beam, including energy particles. The width of the beam depends on how many EXFN you place down in a line.

To select a direction to aim the beam, set the `tmp` value.
```
0: LEFT  1: UP
2: RIGHT 3: DOWN
```

It's recommended that you create a *straight* line 1 element thick for the beam emitter. If you see errors in the wave pattern, you can reset it by using the PROP tool and setting tmp3 to 0, then drawing over all the EXFN.

## Technical
#### Properties
EXFN uses several of its properties to store values about the formation:

**life:** Saves the "powered" on state, 0 = OFF, anything else > 0 is ON. Will floodfill the ON/OFF property when sparked with NSCN / PSCN. If life > 1 will decrease to 1.
- If life > 1, it will "spread" its ON state to other EXFN and will not recieve an OFF state
- If life = 1 it will turn OFF if any nearby EXFN is OFF
- If life = 0 it will spread its OFF state to any EXFN with life = 1

**tmp:** Saves the direction to fire the beam (See above)
**tmp2:** Saves the beam direction (pull or push), 0 = PUSH, anything else > 0 is PULL. Will floodfill the ON/OFF property when sparked with GOLD / TTAN
- Same mechanics as life property.

**tmp3:** Saves the current position along the formation. For horizontal beam emitters (a vertical formation) stores distance from topmost EXFN, with top having tmp3 = 1. For vertical beam emitters (a horizontal formation) stores distance from leftmost EXFN, leftmost having tmp3 = 1.

**flags:** Saves total size of the formation. If it emits a horizontal beam stores the height of the emitter, otherwise the width (for vertical beams). Should be recalculated when the formation size is altered, or when flags=0.

#### Graphics
The wave graphics are drawn directly using the renderer. The basic sine wave function used is

formation_size = (formation_size / 2) * (1 + sin(f * (currentx - partx (+-) time))
Where f = 2pi / period. By default, period = formation_size * 4

For each particle it iterates down the beam until it hits a non-GLAS solid element, and calculates the above function. If the above function = 0 or formation_size it renders a glowing pixel (You can imagine each particle making a tiny slice of a sine wave). The scanning lines are sine waves mapped to the opposite axis, which creates lines that move in a sine wave pattern.

The glow effect is created using `ren->addpixel` if the current renderer is not HEAT and glow is enabled.

#### Stasis field
The way the stasis beam is created is by setting the stasis field layer
`ren->sim->stasis->vx[(inity) / STASIS_CELL][(initx) / STASIS_CELL]`,
`ren->sim->stasis->vy[(inity) / STASIS_CELL][(initx) / STASIS_CELL]`, and
`ren->sim->stasis->strength[(inity) / STASIS_CELL][(initx) / STASIS_CELL] = (cpart->temp - (273.15f - 256.0f)) * 1.6f / 512;`

The stasis field strength is non-zero it will ease the vx and vy of any particles at the location to that. Every 4 frames the stasis field will reset to 0, meaning that if the EXFN disappears or turns off the stasis field will decay.

The stasis field is oriented so particles will tend towards the center of the beam, where the particle will be suspended.

By default, STASIS_CELL = 2
