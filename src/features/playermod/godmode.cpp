#include "playermod.h"
#include "../../utils/il2cpp.h"

namespace PlayerMod {
  bool OnApplyDamage(void* thisPtr)
  {
    if (Settings::bGodMode) {
      auto localPMC = IL2CPP::GetLocalPlayerMoveC();
      if (localPMC && thisPtr) {
        auto thisPMC = IL2CPP::ReadField<void*>(thisPtr, Offsets::PlayerDamageable::playerMoveC);
        if (thisPMC == localPMC) {
          return true;  // Block damage
        }
      }
    }
    return false;
  }

  bool OnEventFired(void* thisPtr, uint8_t eventCode)
  {
    if (Settings::bGodMode) {
      if (thisPtr != nullptr && thisPtr == IL2CPP::GetLocalPlayerMoveC()) {
        // Block network damage RPCs
        // 10 = ApplyDamageRPC, 11 = ApplyDebuffRPC, 24 = GetDamageRPC, 47 = HitByVehicleRPC
        if (eventCode == 10 || eventCode == 11 || eventCode == 24 || eventCode == 47) {
          return true;  // Block event
        }
      }
    }
    return false;
  }
}  // namespace PlayerMod
