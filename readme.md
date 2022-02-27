20000318

My one and probably only foray into GameBoy programming...
Uses the GBDK free devkit. It got boring, to be honest.

It's not really based on Atari's Lunar Lander, it's just a generic
lander, that's why there is no rotation (I honestly didn't even
think about that...)

Either button is thrust, both for double thrust. You need to land with
a speed under 25. You get 100 units of fuel back to try again. ;) It
goes till you crash.

There are 4 different screens. You get 100 points every time you can
clear all 4 (it's really hard!).

Collision detection is really loose.. only the bottom of your ship
collides, so yes, the tunnel is passable. ;)

Scoring:
Land Over +20fps- 0  pts        (Craft Damage)
Land 10-20 fps  - 5  pts        (Hard Landing)
Land 5-10  fps  - 10 pts        (Soft Landing)
Land Under 5fps - 50 pts        (Perfect!)
All four stages -100 pts

