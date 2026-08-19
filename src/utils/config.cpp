#include "config.h"
#include "settings.h"
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
    j["combat"]["aimbot"]         = Features::bAimbot;
    j["combat"]["aimbot_fov"]     = Features::fAimbotFOV;
    j["combat"]["aimbot_head"]    = Features::bAimbotHeadOnly;
    j["combat"]["no_recoil"]      = Features::bNoRecoil;
    j["combat"]["rapid_fire"]     = Features::bRapidFire;
    j["combat"]["aoe_bullets"]    = Features::bAOEBullets;
    j["combat"]["aoe_radius"]     = Features::fAOERadius;
    j["combat"]["instant_charge"] = Features::bInstantCharge;

    // Visual
    j["visual"]["player_esp"]    = Features::bPlayerESP;
    j["visual"]["esp_teammates"] = Features::bPlayerESPTeammates;
    j["visual"]["esp_boxes"]     = Features::bPlayerESPBoxes;
    j["visual"]["esp_names"]     = Features::bPlayerESPNames;
    j["visual"]["skeleton_esp"]  = Features::bSkeletonESP;

    // Weapon Mod
    j["weaponmod"]["infinite_ammo"]   = Features::bInfiniteAmmo;
    j["weaponmod"]["crit_chance"]     = Features::bCritChance100;
    j["weaponmod"]["crit_multiplier"] = Features::fCritMultiplier;
    j["weaponmod"]["reach"]           = Features::bReach;
    j["weaponmod"]["reach_mult"]      = Features::fReachMultiplier;

    j["playermod"]["speed_hack"]       = Features::bSpeedHack;
    j["playermod"]["speed_multiplier"] = Features::fSpeedMultiplier;
    j["playermod"]["high_jump"]        = Features::bHighJump;
    j["playermod"]["jump_multiplier"]  = Features::fJumpMultiplier;
    j["playermod"]["fly"]              = Features::bFly;
    j["playermod"]["god_mode"]         = Features::bGodMode;
    j["playermod"]["auto_heal"]        = Features::bAutoHeal;

    // System
    j["system"]["anticheat_bypass"] = Features::bAntiCheatBypass;

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
    LOAD_BOOL("combat", "aimbot", Features::bAimbot);
    LOAD_FLOAT("combat", "aimbot_fov", Features::fAimbotFOV);
    LOAD_BOOL("combat", "aimbot_head", Features::bAimbotHeadOnly);
    LOAD_BOOL("combat", "no_recoil", Features::bNoRecoil);
    LOAD_BOOL("combat", "rapid_fire", Features::bRapidFire);
    LOAD_BOOL("combat", "aoe_bullets", Features::bAOEBullets);
    LOAD_FLOAT("combat", "aoe_radius", Features::fAOERadius);
    LOAD_BOOL("combat", "instant_charge", Features::bInstantCharge);

    // Visual
    LOAD_BOOL("visual", "player_esp", Features::bPlayerESP);
    LOAD_BOOL("visual", "esp_teammates", Features::bPlayerESPTeammates);
    LOAD_BOOL("visual", "esp_boxes", Features::bPlayerESPBoxes);
    LOAD_BOOL("visual", "esp_names", Features::bPlayerESPNames);
    LOAD_BOOL("visual", "skeleton_esp", Features::bSkeletonESP);

    // Weapon Mod
    LOAD_BOOL("weaponmod", "infinite_ammo", Features::bInfiniteAmmo);
    LOAD_BOOL("weaponmod", "crit_chance", Features::bCritChance100);
    LOAD_FLOAT("weaponmod", "crit_multiplier", Features::fCritMultiplier);
    LOAD_BOOL("weaponmod", "reach", Features::bReach);
    LOAD_FLOAT("weaponmod", "reach_mult", Features::fReachMultiplier);

    LOAD_BOOL("playermod", "speed_hack", Features::bSpeedHack);
    LOAD_FLOAT("playermod", "speed_multiplier", Features::fSpeedMultiplier);
    LOAD_BOOL("playermod", "high_jump", Features::bHighJump);
    LOAD_FLOAT("playermod", "jump_multiplier", Features::fJumpMultiplier);
    LOAD_BOOL("playermod", "fly", Features::bFly);
    LOAD_BOOL("playermod", "god_mode", Features::bGodMode);
    LOAD_BOOL("playermod", "auto_heal", Features::bAutoHeal);

    // System
    LOAD_BOOL("system", "anticheat_bypass", Features::bAntiCheatBypass);

#undef LOAD_BOOL
#undef LOAD_FLOAT
  }
}  // namespace Config
