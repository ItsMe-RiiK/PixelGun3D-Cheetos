#include "weaponmod.h"
#include "../../utils/il2cpp.h"

namespace WeaponMod {
  void ApplyInfiniteAmmo(void* ws)
  {
    if (!ws)
      return;

    using namespace Offsets::WeaponSounds;
    static size_t isUnlimitedAmmoOffset = 0;
    static bool   fetchedAmmoOffset     = false;
    if (!fetchedAmmoOffset) {
      isUnlimitedAmmoOffset =
        IL2CPP::GetFieldOffset((void*) Offsets::Classes::WeaponSounds, "isUnlimitedAmmo");
      if (isUnlimitedAmmoOffset == 0)
        isUnlimitedAmmoOffset = Offsets::WeaponSounds::isUnlimitedAmmo;
      fetchedAmmoOffset = true;
    }
    IL2CPP::WriteField(ws, isUnlimitedAmmoOffset, true);

    // Update template config
    int initAmmo = IL2CPP::ReadField<int>(ws, Offsets::WeaponSounds::InitialAmmo);
    if (initAmmo <= 0)
      initAmmo = 999;

    IL2CPP::WriteField(ws, Offsets::WeaponSounds::ammoInClip, initAmmo);

    // Update live active weapon instance
    auto pmc = IL2CPP::GetLocalPlayerMoveC();
    if (pmc) {
      void* charWeaponCurrent =
        IL2CPP::ReadField<void*>(pmc, Offsets::PlayerMoveC::charWeaponCurrent);
      if (charWeaponCurrent) {
        void* charWeaponClass = IL2CPP::object_get_class(charWeaponCurrent);
        if (charWeaponClass) {
          static size_t liveAmmoInClipOffset = 0;
          static size_t liveAmmoOffset       = 0;
          static bool   fetchedOffsets       = false;
          if (!fetchedOffsets) {
            liveAmmoInClipOffset = IL2CPP::GetFieldOffset(charWeaponClass, "ammoInClip");
            if (liveAmmoInClipOffset == 0)
              liveAmmoInClipOffset = IL2CPP::GetFieldOffset(charWeaponClass, "currentAmmoInClip");
            if (liveAmmoInClipOffset == 0)
              liveAmmoInClipOffset = IL2CPP::GetFieldOffset(
                charWeaponClass, "Obf_B551CFFA"
              );  // hashed name for ammo in clip
            if (liveAmmoInClipOffset == 0)
              liveAmmoInClipOffset = 0x38;  // hard fallback

            liveAmmoOffset = IL2CPP::GetFieldOffset(charWeaponClass, "ammo");
            if (liveAmmoOffset == 0)
              liveAmmoOffset = IL2CPP::GetFieldOffset(charWeaponClass, "currentAmmoInBackpack");
            if (liveAmmoOffset == 0)
              liveAmmoOffset = 0x48;  // fallback

            fetchedOffsets = true;
          }

          // Update Weapon struct
          IL2CPP::WriteField(charWeaponCurrent, liveAmmoInClipOffset, initAmmo);
          IL2CPP::WriteField(charWeaponCurrent, liveAmmoOffset, initAmmo);

          // Update WeaponContainer (0x28)
          void* weaponContainer = IL2CPP::ReadField<void*>(charWeaponCurrent, 0x28);
          if (weaponContainer) {
            IL2CPP::WriteField(weaponContainer, 0x58, initAmmo);
          }

          // Update ItemRecord (0x20)
          void* itemRecord = IL2CPP::ReadField<void*>(charWeaponCurrent, 0x20);
          if (itemRecord) {
            // Set infinite ammo boolean flags
            static size_t itemUnlimitedOffset =
              IL2CPP::GetFieldOffset(IL2CPP::object_get_class(itemRecord), "isUnlimitedAmmo");
            if (itemUnlimitedOffset > 0)
              IL2CPP::WriteField(itemRecord, itemUnlimitedOffset, true);
            else
              IL2CPP::WriteField(itemRecord, 0x6C, true);  // fallback

            IL2CPP::WriteField(itemRecord, 0x20, initAmmo);  // ammoInClip
          }
        }
      }
    }
  }
}  // namespace WeaponMod
