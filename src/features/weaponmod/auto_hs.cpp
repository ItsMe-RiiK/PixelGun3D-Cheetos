#include "weaponmod.h"
#include "../../utils/il2cpp.h"
#include "../../utils/offsets.h"

namespace WeaponMod
{
  void ApplyAutoHeadshot(void* thisPtr, int& damageType)
  {
    if (Settings::bAutoHeadshot && thisPtr) {
      auto localPMC = IL2CPP::GetLocalPlayerMoveC();
      if (localPMC) {
        auto thisPMC = IL2CPP::ReadField<void*>(thisPtr, Offsets::PlayerDamageable::playerMoveC);
        if (thisPMC != localPMC) {
          damageType = 2;
        }
      }
    }
  }
}  // namespace WeaponMod
