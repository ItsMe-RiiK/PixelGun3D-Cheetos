# Pixel Gun 3D: PC Edition - Internal Cheat

![Cheat Version](https://img.shields.io/badge/Version-0.0.1-green.svg)
![Game Version](https://img.shields.io/badge/Game%20Version-1.7.3-brightgreen.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
[![Build Status](https://github.com/ItsMe-RiiK/PixelGun3D-Cheetos/actions/workflows/build.yml/badge.svg)](https://github.com/ItsMe-RiiK/PixelGun3D-Cheetos/actions)

An internal cheat for **Pixel Gun 3D (PC Edition)**

## Features

<details>
<summary><b>Combat</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Aimbot** | Adjustable FOV, Target Head Only | <span title="Maintenance">🚧</span> |
| **No Recoil** | Eliminates weapon recoil | <span title="Maintenance">🚧</span> |
| **No Spread** | Eliminates bullet spread | <span title="Maintenance">🚧</span> |
| **Rapid Fire** | Increases firing rate | <span title="Maintenance">🚧</span> |
| **AOE Bullets** | Area of Effect with adjustable radius | <span title="Work">✅</span> |
| **Instant Charge** | Instantly charges weapons | <span title="Maintenance">🚧</span> |

</details>

<details>
<summary><b>Visuals</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Player ESP** | Box and Health only (Name ESP in progress) | <span title="Work">✅</span> |
| **Skeleton ESP** | Draws player skeletons | <span title="Maintenance">🚧</span> |

</details>

<details>
<summary><b>Weapon Mods</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Infinite Ammo** | Unlimited ammunition | <span title="Maintenance">🚧</span> |
| **No Reload** | Removes reload delays | <span title="Maintenance">🚧</span> |
| **Guaranteed Criticals** | 100% Crit Chance with adjustable multiplier | <span title="Work">✅</span> |
| **Reach** | Extended melee range | <span title="Maintenance">🚧</span> |

</details>

<details>
<summary><b>Player Mods</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Speed Hack** | Adjustable movement speed multiplier | <span title="Maintenance">🚧</span> |
| **Invisibility** | Hides the player model | <span title="Maintenance">🚧</span> |
| **No Fall Damage** | Prevents damage from falling | <span title="Maintenance">🚧</span> |
| **God Mode** | Invulnerability | <span title="Maintenance">🚧</span> |
| **Auto Heal** | Automatically regenerates health | <span title="Maintenance">🚧</span> |

</details>

<details>
<summary><b>System</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Bypass Anti-Cheat** | Enabled by default | <span title="Work">✅</span> |

</details>

## How to Build

### Prerequisites
- **CMake** (3.20 or higher)
- **MinGW-w64** (for cross-compiling on Linux or building natively)
- **vcpkg** (The project includes it, just make sure it's initialized)

### Build Steps (Linux cross-compilation)

1. **Clone the repository (with submodules):**
   ```bash
   git clone --recursive https://github.com/ItsMe-RiiK/PixelGun3D-Cheetos.git
   cd PixelGun3D-Cheetos
   ```
   *(If `vcpkg` isn't bootstrapped yet, run `./vcpkg/bootstrap-vcpkg.sh` first)*

2. **Configure the project:**
   Generate the build files using CMake and configure vcpkg to use the MinGW static triplet:
   ```bash
   cmake -B build -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-static -DCMAKE_BUILD_TYPE=Release
   ```

3. **Compile the code:**
   ```bash
   cmake --build build -j$(nproc)
   ```

4. **Locate the Output:**
   Once successfully built, the resulting `trainer.dll` and `PG3D_Injector.exe` will be located inside the `build/bin/` directory.

## How to Run

Whether you compiled the code yourself or downloaded a pre-built Release archive, we provide smart launch scripts to make injecting easy:

**On Windows:**
1. Start Pixel Gun 3D via Steam.
2. Run `runWindows.bat` (if downloaded from Releases) or `scripts\runWindows.bat` (if built from source).

**On Linux (Proton/Wine):**
1. Start Pixel Gun 3D via Steam.
2. Open a terminal and run `./runLinux.sh` (if downloaded from Releases) or `./scripts/runLinux.sh` (if built from source).
*(Note: This requires `protontricks` installed on your system to correctly inject into the Steam Proton prefix).*

## Technical Details

- **Architecture**: Internal DLL injected directly into the game process.
- **Languages & Libraries**: C++20, MinHook, ImGui, nlohmann/json, Direct3D 11.
- **Build System**: CMake with MinGW (`x86_64-w64-mingw32`) / vcpkg.


## LICENSE
This project under [MIT LICENSE](license)

## Disclaimer
This project is created for **educational and reverse engineering purposes only**. It demonstrates memory manipulation, function hooking, and DirectX overlay techniques.
Any use of this project for cheating or hacking in online games is illegal and unethical. and we dont take any responsibility for the actions of users who use this project for cheating or hacking in online games.

