# Pixel Gun 3D : PC Edition
is a fps game based on unity engine.

## The Challenge
create a trainer menu (cheat) for the game:

**Features**

***A. Combat***
1. aimbot
2. no recoil
3. no spread
4. Rapid Fire
5. AOEBullets
6. Instant Charge for charged weapons

***B. Visual***
1. Player ESP — boxes, health
2. Skeleton ESP

***C. WeaponMod***
1. Infinite Ammo
2. No reload
3. 100% crit chance (guaranteed for crit)
4. Reach (extended melee range)

***D. PlayerMod***
1. Speed Hack (fast move)
2. Invisibility
3. NoFallDamage
4. God Mode / AutoHeal

***E. Economy***
1. Infinite Gem Claim
2. Unlock All Weapons
3. Unlock All Skins


### Fast Way:
**Static analysis**
- the current ghidra analysis at "Github.exe" listing claimed to had all the feature (wroted above), connect to ghidra mcp to see the full analysis that already opened. all the folder inside C:\Users\riik\Downloads\GithubSetup (this is old version, and idk if this is still working)

- the new dump.cs already on  D:\PixelGun3D\resources\dump\dump.cs, use this to find newest and validated offsets needed for the feature

**Dynamic analysis**
- cheat engine mcp already stay and attached to the process, waiting for dynamic analysis. and im on the game right now (standby lobby) if need dynamic analysis. the bridge using this script: D:\PixelGun3D\the-bridge-ce-mcp.lua


### The rules
- we prefer chose c/c++ and some json for config later
- find the offsets for all the feature needed above. also dont forget to find the Anti-Cheat Behaviour and bypass it
- wrote all founded and valid offsets inside the /src/offsets.h