#include "playermod.h"
#include "../../utils/il2cpp.h"
#include <windows.h>

namespace PlayerMod {
  void ApplyFly()
  {
    auto pmc = IL2CPP::GetLocalPlayerMoveC();
    if (!pmc)
      return;

    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
      void* myTransform = IL2CPP::ReadField<void*>(pmc, Offsets::PlayerMoveC::myPlayerTransform);
      if (myTransform) {
        auto pos = IL2CPP::GetTransformPosition(myTransform);
        pos.y += 0.5f;
        IL2CPP::SetTransformPosition(myTransform, pos.x, pos.y, pos.z);
      }
    }
  }
}  // namespace PlayerMod
