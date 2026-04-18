# Minecraft Pocket Edition (v0.6.1) Source Code

forked from a few forks of `deepfriedwaffles/minecraft-pe-source-code`

currently achieved: (linux-build branch)
- linux port
- fixed mouse turning issues
- f3 debug menu
- added a system for creating settings options more easily (hopefully)
- added left hand (actual left hand)
- made render distance changeable
- fps limiting settings
- fixed ambient occlusion for linux
- fixed keyboard and mouse input bugs
- scrolling wheel works in uis with scrolling
- changeable gamemode right in the world
- probably more

planned:
- a working windows build
- more settings (zoom, fov, etc)
- optimizations
- probably more

right now it can be built via github actions or on your system via the build script `./build target release_type`, e.g. `./build.sh MinecraftClient release` or `./build.sh MinecraftServer debug` (the windows build fails rn)
