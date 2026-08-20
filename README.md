# Pixel Gun 3D: PC Edition - Internal Cheat

![Cheat Version](https://img.shields.io/badge/Version-0.0.3-green.svg)
![Game Version](https://img.shields.io/badge/Game%20Version-26.10.2.0-brightgreen.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
[![Build Status](https://github.com/ItsMe-RiiK/PixelGun3D-Cheetos/actions/workflows/build.yml/badge.svg)](https://github.com/ItsMe-RiiK/PixelGun3D-Cheetos/actions)

An internal cheat for **Pixel Gun 3D (PC Edition)**

## Features

<details>
<summary><b>Combat</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **AOE Bullets** | Area of Effect with adjustable radius | <span title="Work">✅</span> |

</details>

<details>
<summary><b>Visuals</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Player ESP** | Box, Health, Name | <span title="Work">✅</span> |
| **Skeleton ESP** | Draws player skeletons |  <span title="Work">✅</span>|

</details>

<details>
<summary><b>Weapon Mods</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **Infinite Ammo** | Unlimited ammunition | <span title="Work">✅</span> |
| **Guaranteed Criticals** | 100% Crit Chance with adjustable multiplier | <span title="Work">✅</span> |
| **Reach** | Extended melee range |  <span title="Work">✅</span>|
| **Wallshot** | Shoot from behind the wall | <span title="Maintenance">⚠️</span> |

</details>

<details>
<summary><b>Player Mods</b></summary>

| Feature | Description | Status |
| :--- | :--- | :--- |
| **God Mode** | Invicible | <span title="Work">✅</span> |
| **Auto Heal** | Automatically regenerates health | <span title="Work">✅</span> |
| **High Jump** | Adjustable jump multiplier | <span title="Work">✅</span> |
| **Fly** | Enable fly mode | <span title="Work">✅</span> |

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

### Build Steps (cross-compilation)

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
1. Start the game via Steam.
2. Click `runWindows.bat` (if downloaded from Releases) or `scripts\runWindows.bat` (if built from source).

**Linux or macOS (Proton/Wine):**
1. Start the game via Steam.
2. Open a terminal and run `./runLinux.sh` (if downloaded from Releases) or `./scripts/runLinux.sh` (if built from source).
*(Note: This requires `protontricks` installed on your system to correctly inject into the Steam Proton prefix).*

## Technical Details

- **Architecture**: Internal DLL injected directly into the game process.
- **Languages & Libraries**: C++20, MinHook, ImGui, nlohmann/json, Direct3D 11.
- **Build System**: CMake with MinGW (`x86_64-w64-mingw32`) / vcpkg.


## LICENSE
This project under [MIT LICENSE](license)
