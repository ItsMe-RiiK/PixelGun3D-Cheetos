#include "playermod.h"
#include "../../utils/il2cpp.h"
#include "../../ui/menu.h"

namespace PlayerMod {
  void Tick()
  {
    try {
      if (Settings::bAutoHeal)
        ApplyAutoHeal();

      if (Settings::bHighJump)
        ApplyHighJump();

      if (Settings::bFly)
        ApplyFly();
    } catch (...) {
      // Prevent crash
    }
  }

  void InitMenu()
  {
    Menu::AddMenuItem({"-- PLAYER --", Menu::ItemType::Header});
    Menu::AddMenuItem({"God Mode", Menu::ItemType::Bool, &Settings::bGodMode});
    Menu::AddMenuItem({"Auto Heal", Menu::ItemType::Bool, &Settings::bAutoHeal});
    Menu::AddMenuItem({"High Jump", Menu::ItemType::Bool, &Settings::bHighJump});
    Menu::AddMenuItem(
      {"Jump Multiplier", Menu::ItemType::Float, nullptr, &Settings::fJumpMultiplier, 1.0f, 10.0f,
       0.5f}
    );
    Menu::AddMenuItem({"Fly", Menu::ItemType::Bool, &Settings::bFly});
  }
}  // namespace PlayerMod
