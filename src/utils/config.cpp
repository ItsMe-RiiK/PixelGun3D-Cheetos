#include "config.h"
#include "../features/combat/combat.h"
#include "../features/visual/visual.h"
#include "../features/playermod/playermod.h"
#include "../features/weaponmod/weaponmod.h"
#include "hooks.h"
#include <nlohmann/json.hpp>
#include <fstream>

namespace Config {
  std::string configPath = "";

  void SetConfigPath(const std::string& dllPath)
  {
    // Config file next to the DLL
    size_t lastSlash = dllPath.find_last_of("\\/");
    if (lastSlash != std::string::npos)
      configPath = dllPath.substr(0, lastSlash + 1) + "pg3d_trainer.json";
    else
      configPath = "pg3d_trainer.json";
  }

  void Save()
  {
    if (configPath.empty())
      return;

    nlohmann::json j;

    // Combat
    j["combat"]["aoe_bullets"] = Combat::Settings::bAOEBullets;
    j["combat"]["aoe_radius"]  = Combat::Settings::fAOERadius;

    // Visual
    j["visual"]["player_esp"]   = Visual::Settings::bPlayerESP;
    j["visual"]["esp_boxes"]    = Visual::Settings::bPlayerESPBoxes;
    j["visual"]["esp_names"]    = Visual::Settings::bPlayerESPNames;
    j["visual"]["skeleton_esp"] = Visual::Settings::bSkeletonESP;

    // Weapon Mod
    j["weaponmod"]["infinite_ammo"]   = WeaponMod::Settings::bInfiniteAmmo;
    j["weaponmod"]["wallshot"]        = WeaponMod::Settings::bWallshot;
    j["weaponmod"]["crit_chance"]     = WeaponMod::Settings::bCritChance100;
    j["weaponmod"]["crit_multiplier"] = WeaponMod::Settings::fCritMultiplier;
    j["weaponmod"]["reach"]           = WeaponMod::Settings::bReach;
    j["weaponmod"]["reach_mult"]      = WeaponMod::Settings::fReachMultiplier;

    j["playermod"]["high_jump"]       = PlayerMod::Settings::bHighJump;
    j["playermod"]["jump_multiplier"] = PlayerMod::Settings::fJumpMultiplier;
    j["playermod"]["fly"]             = PlayerMod::Settings::bFly;
    j["playermod"]["god_mode"]        = PlayerMod::Settings::bGodMode;
    j["playermod"]["auto_heal"]       = PlayerMod::Settings::bAutoHeal;

    // System
    j["system"]["anticheat_bypass"] = Hooks::Settings::bAntiCheatBypass;

    std::ofstream file(configPath);
    if (file.is_open()) {
      file << j.dump(4);
      file.close();
    }
  }

  void Load()
  {
    if (configPath.empty())
      return;

    std::ifstream file(configPath);
    if (!file.is_open())
      return;

    nlohmann::json j;
    try {
      file >> j;
    } catch (...) {
      return;
    }
    file.close();

#define LOAD_BOOL(section, key, var) \
  if (j.contains(section) && j[section].contains(key)) \
  var = j[section][key].get<bool>()
#define LOAD_FLOAT(section, key, var) \
  if (j.contains(section) && j[section].contains(key)) \
  var = j[section][key].get<float>()

    // Combat
    LOAD_BOOL("combat", "aoe_bullets", Combat::Settings::bAOEBullets);
    LOAD_FLOAT("combat", "aoe_radius", Combat::Settings::fAOERadius);

    // Visual
    LOAD_BOOL("visual", "player_esp", Visual::Settings::bPlayerESP);
    LOAD_BOOL("visual", "esp_boxes", Visual::Settings::bPlayerESPBoxes);
    LOAD_BOOL("visual", "esp_names", Visual::Settings::bPlayerESPNames);
    LOAD_BOOL("visual", "skeleton_esp", Visual::Settings::bSkeletonESP);

    // Weapon Mod
    LOAD_BOOL("weaponmod", "infinite_ammo", WeaponMod::Settings::bInfiniteAmmo);
    LOAD_BOOL("weaponmod", "wallshot", WeaponMod::Settings::bWallshot);
    LOAD_BOOL("weaponmod", "crit_chance", WeaponMod::Settings::bCritChance100);
    LOAD_FLOAT("weaponmod", "crit_multiplier", WeaponMod::Settings::fCritMultiplier);
    LOAD_BOOL("weaponmod", "reach", WeaponMod::Settings::bReach);
    LOAD_FLOAT("weaponmod", "reach_mult", WeaponMod::Settings::fReachMultiplier);

    LOAD_BOOL("playermod", "high_jump", PlayerMod::Settings::bHighJump);
    LOAD_FLOAT("playermod", "jump_multiplier", PlayerMod::Settings::fJumpMultiplier);
    LOAD_BOOL("playermod", "fly", PlayerMod::Settings::bFly);
    LOAD_BOOL("playermod", "god_mode", PlayerMod::Settings::bGodMode);
    LOAD_BOOL("playermod", "auto_heal", PlayerMod::Settings::bAutoHeal);

    // System
    LOAD_BOOL("system", "anticheat_bypass", Hooks::Settings::bAntiCheatBypass);

#undef LOAD_BOOL
#undef LOAD_FLOAT
  }
}  // namespace Config
