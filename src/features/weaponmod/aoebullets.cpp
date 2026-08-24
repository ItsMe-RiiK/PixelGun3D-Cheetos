#include "weaponmod.h"
#include "../../utils/il2cpp.h"

namespace WeaponMod
{
  void ApplyAOEBullets(void* ws)
  {
    if (!ws)
      return;
    using namespace Offsets::WeaponSounds;

    IL2CPP::WriteField(ws, isSectorsAOE, true);
    IL2CPP::WriteField(ws, sectorsAOEAngleFront, 360.0f);
    IL2CPP::WriteField(ws, sectorsAOEAngleBack, 360.0f);
    IL2CPP::WriteField(ws, sectorsAOERadius, Settings::fAOERadius);
    IL2CPP::WriteField(ws, sectorsAOEDmgMultFront, 1.0f);
    IL2CPP::WriteField(ws, sectorsAOEDmgMultSide, 1.0f);
    IL2CPP::WriteField(ws, sectorsAOEDmgMultBack, 1.0f);

    if (IL2CPP::ReadField<bool>(ws, bazooka)) {
      IL2CPP::WriteField(ws, bazookaExplosionRadius, Settings::fAOERadius);
    }
  }
}  // namespace WeaponMod
