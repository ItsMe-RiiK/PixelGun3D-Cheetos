#include "playermod.h"
#include "../utils/settings.h"
#include "../utils/il2cpp.h"

namespace PlayerMod {

  void ApplyAutoHeal()
  {
    auto pmc = IL2CPP::GetLocalPlayerMoveC();
    if (!pmc)
      return;

    // Reset last damage time so game's passive regen kicks in immediately
    IL2CPP::WriteField(pmc, Offsets::PlayerMoveC::lastDamageTime, 0.0f);
    // Set healingByPlayer to self to trigger teammate-heal logic
    IL2CPP::WriteField(pmc, Offsets::PlayerMoveC::healingByPlayer, pmc);
  }

  void ApplyMovementMods()
  {
    // RPG_Controller is the global movement manager for local player
    if (!Offsets::Classes::RPG_Controller)
      return;

    void* staticData = IL2CPP::GetStaticFieldData((void*) Offsets::Classes::RPG_Controller);
    if (!staticData)
      return;

    void* instance = IL2CPP::ReadField<void*>(staticData, Offsets::RPG_Controller::instance);
    if (!instance)
      return;

    static float originalJump    = -1.0f;
    static float originalGravity = -1.0f;

    if (originalJump < 0.0f) {
      originalJump    = IL2CPP::ReadField<float>(instance, Offsets::RPG_Controller::jumpHeight);
      originalGravity = IL2CPP::ReadField<float>(instance, Offsets::RPG_Controller::gravity);
    }

    if (Features::bHighJump) {
      IL2CPP::WriteField<float>(
        instance, Offsets::RPG_Controller::jumpHeight, originalJump * Features::fJumpMultiplier
      );
    }
    else if (originalJump >= 0.0f) {
      IL2CPP::WriteField<float>(instance, Offsets::RPG_Controller::jumpHeight, originalJump);
    }

    if (Features::bFly) {
      IL2CPP::WriteField<float>(instance, Offsets::RPG_Controller::gravity, 0.0f);
    }
    else if (originalGravity >= 0.0f) {
      IL2CPP::WriteField<float>(instance, Offsets::RPG_Controller::gravity, originalGravity);
    }
  }

  void Tick()
  {
    try {
      ApplyMovementMods();

      if (Features::bAutoHeal)
        ApplyAutoHeal();


    } catch (...) {
      // Prevent crash
    }
  }
}  // namespace PlayerMod
