#include "il2cpp.h"

namespace IL2CPP {
  // Function pointer typedefs
  using fn_il2cpp_domain_get                  = void* (*) ();
  using fn_il2cpp_domain_get_assemblies       = void** (*) (void* domain, size_t* size);
  using fn_il2cpp_assembly_get_image          = void* (*) (void* assembly);
  using fn_il2cpp_image_get_name              = const char* (*) (void* image);
  using fn_il2cpp_class_from_name             = void* (*) (void*       image,
                                                           const char* namespaze,
                                                           const char* name);
  using fn_il2cpp_class_get_static_field_data = void* (*) (void* klass);
  using fn_il2cpp_class_get_method_from_name  = void* (*) (void*       klass,
                                                           const char* name,
                                                           int         argsCount);
  using fn_il2cpp_field_static_get_value      = void (*)(void* field, void* value);
  using fn_il2cpp_runtime_object_init         = void (*)(void* obj);
  using fn_il2cpp_object_new                  = void* (*) (void* klass);
  using fn_il2cpp_string_new                  = void* (*) (const char* str);
  using fn_il2cpp_class_get_fields            = void* (*) (void* klass, void** iter);
  using fn_il2cpp_field_get_name              = const char* (*) (void* field);
  using fn_il2cpp_field_get_offset            = size_t (*)(void* field);
  using fn_il2cpp_type_get_object             = void* (*) (void* type);
  using fn_il2cpp_class_get_type              = void* (*) (void* klass);

  // Globals
  HMODULE   hGameAssembly       = nullptr;
  uintptr_t gameAssemblyBase    = 0;
  void*     assemblyCSharpImage = nullptr;

  // Exported function pointers
  fn_il2cpp_domain_get                  ptr_domain_get                  = nullptr;
  fn_il2cpp_domain_get_assemblies       ptr_domain_get_assemblies       = nullptr;
  fn_il2cpp_assembly_get_image          ptr_assembly_get_image          = nullptr;
  fn_il2cpp_image_get_name              ptr_image_get_name              = nullptr;
  fn_il2cpp_class_from_name             ptr_class_from_name             = nullptr;
  fn_il2cpp_class_get_static_field_data ptr_class_get_static_field_data = nullptr;
  fn_il2cpp_class_get_method_from_name  ptr_class_get_method_from_name  = nullptr;
  fn_il2cpp_object_new                  ptr_object_new                  = nullptr;
  fn_il2cpp_string_new                  ptr_string_new                  = nullptr;
  fn_il2cpp_class_get_fields            ptr_class_get_fields            = nullptr;
  fn_il2cpp_field_get_name              ptr_field_get_name              = nullptr;
  fn_il2cpp_field_get_offset            ptr_field_get_offset            = nullptr;
  fn_il2cpp_class_get_type              ptr_class_get_type              = nullptr;
  fn_il2cpp_type_get_object             ptr_type_get_object             = nullptr;

  bool Init()
  {
    hGameAssembly = GetModuleHandleA("GameAssembly.dll");
    if (!hGameAssembly)
      return false;

    gameAssemblyBase = reinterpret_cast<uintptr_t>(hGameAssembly);

#define RESOLVE(name) \
  ptr_##name = reinterpret_cast<fn_il2cpp_##name>(GetProcAddress(hGameAssembly, "il2cpp_" #name)); \
  if (!ptr_##name) \
    return false;

    RESOLVE(domain_get);
    RESOLVE(domain_get_assemblies);
    RESOLVE(assembly_get_image);
    RESOLVE(image_get_name);
    RESOLVE(class_from_name);
    RESOLVE(class_get_static_field_data);
    RESOLVE(class_get_method_from_name);
    RESOLVE(object_new);
    RESOLVE(string_new);
    RESOLVE(class_get_fields);
    RESOLVE(field_get_name);
    RESOLVE(field_get_offset);

    ptr_class_get_type = reinterpret_cast<fn_il2cpp_class_get_type>(
      GetProcAddress(hGameAssembly, "il2cpp_class_get_type")
    );
    ptr_type_get_object = reinterpret_cast<fn_il2cpp_type_get_object>(
      GetProcAddress(hGameAssembly, "il2cpp_type_get_object")
    );

#undef RESOLVE

    auto domain = domain_get();
    if (!domain)
      return false;

    size_t asmCount   = 0;
    auto   assemblies = domain_get_assemblies(domain, &asmCount);
    if (!assemblies || asmCount == 0)
      return false;

    for (size_t i = 0; i < asmCount; i++) {
      auto img = assembly_get_image(assemblies[i]);
      if (!img)
        continue;
      const char* name = image_get_name(img);
      if (name && strstr(name, "Assembly-CSharp")) {
        assemblyCSharpImage = img;
        break;
      }
    }

    return assemblyCSharpImage != nullptr;
  }

  void* GetClass(const char* name, const char* namespaze)
  {
    if (!assemblyCSharpImage)
      return nullptr;
    return IL2CPP::class_from_name(assemblyCSharpImage, namespaze, name);
  }

  void* GetStaticFieldData(void* klass)
  {
    if (!klass || !ptr_class_get_static_field_data)
      return nullptr;
    return ptr_class_get_static_field_data(klass);
  }

  void*  domain_get() { return ptr_domain_get ? ptr_domain_get() : nullptr; }
  void** domain_get_assemblies(void* domain, size_t* size)
  {
    return ptr_domain_get_assemblies ? ptr_domain_get_assemblies(domain, size) : nullptr;
  }
  void* assembly_get_image(void* assembly)
  {
    return ptr_assembly_get_image ? ptr_assembly_get_image(assembly) : nullptr;
  }
  const char* image_get_name(void* image)
  {
    return ptr_image_get_name ? ptr_image_get_name(image) : nullptr;
  }
  void* class_from_name(void* image, const char* namespaze, const char* name)
  {
    return ptr_class_from_name ? ptr_class_from_name(image, namespaze, name) : nullptr;
  }
  void* class_get_method_from_name(void* klass, const char* name, int argsCount)
  {
    return ptr_class_get_method_from_name ? ptr_class_get_method_from_name(klass, name, argsCount)
                                          : nullptr;
  }

  void* GetSystemTypeForClass(void* klass)
  {
    if (!klass || !ptr_class_get_type || !ptr_type_get_object)
      return nullptr;
    void* type = ptr_class_get_type(klass);
    if (!type)
      return nullptr;
    return ptr_type_get_object(type);
  }

  void* FindObjectsOfType(void* type)
  {
    using fn_FindObjectsOfType                     = void* (*) (void*);
    static fn_FindObjectsOfType pFindObjectsOfType = nullptr;

    if (!pFindObjectsOfType) {
      pFindObjectsOfType = reinterpret_cast<fn_FindObjectsOfType>(
        GetMethodAddress(Offsets::Object::FindObjectsOfType_RVA)
      );
    }

    if (!pFindObjectsOfType || !type)
      return nullptr;
    return pFindObjectsOfType(type);
  }

  std::vector<void*> GetPlayers()
  {
    static std::vector<void*> cachedPlayers;
    static uint64_t           lastTime = 0;
    uint64_t                  now      = GetTickCount64();

    if (now - lastTime > 1000 || cachedPlayers.empty()) {
      lastTime = now;
      cachedPlayers.clear();

      void* pmcClass = reinterpret_cast<void*>(Offsets::Classes::PlayerMoveC);
      void* pmcType  = GetSystemTypeForClass(pmcClass);
      if (pmcType) {
        void* arrayObj = FindObjectsOfType(pmcType);
        if (arrayObj) {
          size_t length   = GetArrayLength(arrayObj);
          void** elements = GetArrayElements<void*>(arrayObj);
          if (elements) {
            for (size_t i = 0; i < length; i++) {
              if (elements[i]) {
                cachedPlayers.push_back(elements[i]);
              }
            }
          }
        }
      }
    }

    return cachedPlayers;
  }

  uintptr_t GetMethodAddress(uintptr_t rva) { return gameAssemblyBase + rva; }

  bool ResolveClasses()
  {
    Offsets::Classes::WeaponManager    = reinterpret_cast<uintptr_t>(GetClass("WeaponManager"));
    Offsets::Classes::PlayerMoveC      = reinterpret_cast<uintptr_t>(GetClass("Player_move_c"));
    Offsets::Classes::WeaponSounds     = reinterpret_cast<uintptr_t>(GetClass("WeaponSounds"));
    Offsets::Classes::PlayerDamageable = reinterpret_cast<uintptr_t>(GetClass("PlayerDamageable"));
    Offsets::Classes::FPSController =
      reinterpret_cast<uintptr_t>(GetClass("FirstPersonControlSharp"));
    Offsets::Classes::CheatDetectedBanner =
      reinterpret_cast<uintptr_t>(GetClass("CheatDetectedBanner"));
    Offsets::Classes::ClickerDetector = reinterpret_cast<uintptr_t>(GetClass("ClickerDetector"));

    return Offsets::Classes::WeaponManager != 0 && Offsets::Classes::PlayerMoveC != 0
        && Offsets::Classes::WeaponSounds != 0;
  }

  void* GetWeaponManagerInstance()
  {
    auto klass = reinterpret_cast<void*>(Offsets::Classes::WeaponManager);
    if (!klass)
      return nullptr;

    auto staticData = GetStaticFieldData(klass);
    if (!staticData)
      return nullptr;

    return *reinterpret_cast<void**>(
      reinterpret_cast<uintptr_t>(staticData) + Offsets::WeaponManager::StaticInstance
    );
  }

  void* GetLocalPlayerMoveC()
  {
    auto wm = GetWeaponManagerInstance();
    if (!wm)
      return nullptr;
    return ReadField<void*>(wm, Offsets::WeaponManager::myPlayerMoveC);
  }

  void* GetCurrentWeaponSounds()
  {
    auto wm = GetWeaponManagerInstance();
    if (!wm)
      return nullptr;
    return ReadField<void*>(wm, Offsets::WeaponManager::currentWeaponSounds);
  }

  void* GetLocalPlayerDamageable()
  {
    auto pmc = GetLocalPlayerMoveC();
    if (!pmc)
      return nullptr;
    return ReadField<void*>(pmc, Offsets::PlayerMoveC::playerDamageable);
  }
}  // namespace IL2CPP
