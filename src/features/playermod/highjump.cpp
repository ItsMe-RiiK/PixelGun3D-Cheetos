#include "playermod.h"
#include "../../utils/il2cpp.h"
#include <windows.h>

namespace PlayerMod {
  void ApplyHighJump()
  {
    auto pmc = IL2CPP::GetLocalPlayerMoveC();
    if (!pmc)
      return;

    static bool wasSpacePressed = false;
    bool        isSpacePressed  = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

    if (isSpacePressed && !wasSpacePressed) {
      void* myTransform = IL2CPP::ReadField<void*>(pmc, Offsets::PlayerMoveC::myPlayerTransform);
      if (myTransform) {
        auto pos = IL2CPP::GetTransformPosition(myTransform);
        pos.y += Settings::fJumpMultiplier > 0.0f ? (Settings::fJumpMultiplier * 0.5f) : 2.0f;
        IL2CPP::SetTransformPosition(myTransform, pos.x, pos.y, pos.z);
      }
    }
    wasSpacePressed = isSpacePressed;
  }
}  // namespace PlayerMod
