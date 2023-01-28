## LASR (Laser)
Like BRAY, but randomly deletes particles around it and heats everything to MAX_TEMP. Disappears after 4 frames. Glows white and flares 
when particles hit it. JCB1 spawns this.

## MSSL (Missile)
Guided missile, tries to accelerate to its target x, y coordinates (determined by tmp3, tmp4). If it hits a solid / powder detonates 
(transforms into BOMB). Can go through gas and liquid without interruption. Leaves behind a smoke trail.

## BRKN (Broken)
Represents a generic broken form. Flammability is a bit broken. Mimics ctype's update and graphics. Broken paper 
cannot be colored. Broken stuff can only melt. Chance to fuse when sparked (this is a bug but it's called a feature)

## FOAM
Foamy substance that's made when vinegar and baking soda is mixed. "Grows", tmp is the generation of growth. Disappears when 
< 0 C or > 100 C, or touching GEL or SPNG, or absolute pressure > 5. Light powder, behaves like SAWD.
