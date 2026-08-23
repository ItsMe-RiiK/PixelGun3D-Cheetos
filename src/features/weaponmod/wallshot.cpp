/*#include "weaponmod.h"
#include "../../utils/il2cpp.h"

namespace WeaponMod {
  void ApplyWallshot(void* ws)
  {
    if (!ws)
      return;

    using namespace Offsets::WeaponSounds;

    Offsets::WeaponSounds::InitDynamicOffsets();

    // FORCE disable breakout to ensure bullets hit players instead of ignoring all colliders
    IL2CPP::WriteFieldIfValid(ws, breakoutOffset, false);
    IL2CPP::WriteFieldIfValid(ws, superBreakoutOffset, false);

    // Real Wallshot approach: inject WeaponSounds.Obf_61B98209.WallBreak (9) into InShopEffects list
    void* listObj = (void*) IL2CPP::ReadField<uintptr_t>(ws, Offsets::WeaponSounds::inShopEffects);
    if (listObj) {
      void* itemsArray = (void*) IL2CPP::ReadField<uintptr_t>(listObj, Offsets::IL2CPPStructs::listItemsOffset);
      int   size       = IL2CPP::ReadField<int>(listObj, Offsets::IL2CPPStructs::listSizeOffset);
      if (itemsArray && size > 0 && !WeaponMod::backup.addedWallBreak) {
        bool hasWallBreak = false;
        for (int i = 0; i < size; i++) {
          int effect = IL2CPP::ReadField<int>(itemsArray, Offsets::IL2CPPStructs::arrayDataOffset + (i * 4));
          if (effect == 9) {
            hasWallBreak = true;
            break;
          }
        }

        if (!hasWallBreak) {
          int capacity                             = IL2CPP::ReadField<int>(itemsArray, Offsets::IL2CPPStructs::arrayLengthOffset);
          WeaponMod::backup.addedWallBreak         = true;
          WeaponMod::backup.originalEffectListSize = size;

          if (size < capacity) {
            IL2CPP::WriteField<int>(itemsArray, Offsets::IL2CPPStructs::arrayDataOffset + (size * 4), 9);
            IL2CPP::WriteField<int>(listObj, Offsets::IL2CPPStructs::listSizeOffset, size + 1);
            WeaponMod::backup.overwrittenEffect = -1;
          }
          else {
            WeaponMod::backup.overwrittenEffect =
              IL2CPP::ReadField<int>(itemsArray, Offsets::IL2CPPStructs::arrayDataOffset + ((size - 1) * 4));
            IL2CPP::WriteField<int>(itemsArray, Offsets::IL2CPPStructs::arrayDataOffset + ((size - 1) * 4), 9);
          }
        }
      }
    }

    // Also enable piercing for melee
    IL2CPP::WriteField(ws, Offsets::WeaponSounds::isPiercingMelee, true);
    IL2CPP::WriteField(ws, Offsets::WeaponSounds::distancePiercingMelee, 9999.0f);
  }
}  // namespace WeaponMod
*/
