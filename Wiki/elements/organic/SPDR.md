# SPDR

### Spider

![Element](https://i.imgur.com/rm4tPuu.gif)

*Initial web frame (Not complete web)*

**Description:**  *Spider. Spins webs and eats captured creatures.*

Spiders wander around and spin webs, which capture living prey such as BIRD, BEE and ANT. Spiders can "stick" to objects when they're near, so they can hang off the bottom of roofs and such. Below -5 C spiders turn into SNOW, above 60 C they turn into dust. Above 5.0 pressure SPDR turns into dead BCTR. 

## Web Spinning
After wandering around randomly, a spider will decide to spin a web. It randomly picks an angle where it can create a web that's between 30 and 350 px long, attached to a solid. It will try 5 different angles before giving up, and continue moving.

If it's able to find an angle, it goes to the midpoint of the newly created line, and creates a 2nd support string with the same requirements. It goes to the midpoint of the 2nd support string.

Once at the midpoint, it will try to create at most 32 evenly spaced spokes around the mid point, and ring them. This process is done instantly because it's hard to code spiders without having a lot of temporary variables store position and such.

Once the web is done, the spider freely (randomly) wanders the web, eating any captured prey it comes upon. If the spider falls off the web (shouldn't happen unless something intentionally pushed it off) in any point in the process it "resets" back to stage 1 and begins randomly wandering.


## Technical
#### Properties
**tmp:** If 0, tends left and up, else if 1, tends right and down

**tmp2:** Stage of web construction:
```
0 = Moving around, haven't built a web yet
1 = Building 1st and 2nd support strands
2 = Building spokes and spiral
3 = Done web, living on the web
```

**tmp3:** x target location to move to

**tmp4:** y target location to move to
