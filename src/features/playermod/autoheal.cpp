#include "playermod.h"
#include "../../utils/il2cpp.h"

namespace PlayerMod
{
  void ApplyAutoHeal()
  {
    auto damageable = IL2CPP::GetLocalPlayerDamageable();
    if (!damageable)
      return;

    static size_t healthOffset = 0;
    static size_t armorOffset  = 0;

    if (healthOffset == 0) {
      void* klass  = reinterpret_cast<void*>(Offsets::Classes::PlayerDamageable);
      healthOffset = IL2CPP::GetFieldOffset(klass, "health");
      armorOffset  = IL2CPP::GetFieldOffset(klass, "armor");
    }

    if (healthOffset > 0) {
      IL2CPP::WriteField(damageable, healthOffset, 100.0f);
    }
    if (armorOffset > 0) {
      IL2CPP::WriteField(damageable, armorOffset, 100.0f);
    }
  }
}  // namespace PlayerMod
