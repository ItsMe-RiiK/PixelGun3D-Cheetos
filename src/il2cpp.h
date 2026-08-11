#pragma once
// =====================================================
// IL2CPP Runtime Helper
// Resolves classes, reads fields, and calls methods
// via GameAssembly.dll exported IL2CPP API
// =====================================================

#include "offsets.h"

#include <Windows.h>
#include <string>
#include <unordered_map>

namespace IL2CPP {
  // Function pointer typedefs for IL2CPP exports
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

  // Exported function pointers — resolved in Init()
  inline fn_il2cpp_domain_get                  domain_get                  = nullptr;
  inline fn_il2cpp_domain_get_assemblies       domain_get_assemblies       = nullptr;
  inline fn_il2cpp_assembly_get_image          assembly_get_image          = nullptr;
  inline fn_il2cpp_image_get_name              image_get_name              = nullptr;
  inline fn_il2cpp_class_from_name             class_from_name             = nullptr;
  inline fn_il2cpp_class_get_static_field_data class_get_static_field_data = nullptr;
  inline fn_il2cpp_class_get_method_from_name  class_get_method_from_name  = nullptr;
  inline fn_il2cpp_object_new                  object_new                  = nullptr;
  inline fn_il2cpp_string_new                  string_new                  = nullptr;
  inline fn_il2cpp_class_get_fields            class_get_fields            = nullptr;
  inline fn_il2cpp_field_get_name              field_get_name              = nullptr;
  inline fn_il2cpp_field_get_offset            field_get_offset            = nullptr;
  inline fn_il2cpp_class_get_type              class_get_type              = nullptr;
  inline fn_il2cpp_type_get_object             type_get_object             = nullptr;

  // Module handles
  inline HMODULE   hGameAssembly    = nullptr;
  inline uintptr_t gameAssemblyBase = 0;

  // Cached image pointer for Assembly-CSharp
  inline void* assemblyCSharpImage = nullptr;

  // Initialize all IL2CPP function pointers
  inline bool Init()
  {
    hGameAssembly = GetModuleHandleA("GameAssembly.dll");
    if (!hGameAssembly)
      return false;

    gameAssemblyBase = reinterpret_cast<uintptr_t>(hGameAssembly);

#define RESOLVE(name) \
  name = reinterpret_cast<fn_il2cpp_##name>(GetProcAddress(hGameAssembly, "il2cpp_" #name)); \
  if (!name) \
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

    // Optional — these may not exist in all IL2CPP versions
    class_get_type = reinterpret_cast<fn_il2cpp_class_get_type>(
      GetProcAddress(hGameAssembly, "il2cpp_class_get_type")
    );
    type_get_object = reinterpret_cast<fn_il2cpp_type_get_object>(
      GetProcAddress(hGameAssembly, "il2cpp_type_get_object")
    );

#undef RESOLVE

    // Find Assembly-CSharp image
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

  // Resolve a class by name from Assembly-CSharp
  inline void* GetClass(const char* name, const char* namespaze = "")
  {
    if (!assemblyCSharpImage)
      return nullptr;
    return class_from_name(assemblyCSharpImage, namespaze, name);
  }

  // Get pointer to static field data for a class
  inline void* GetStaticFieldData(void* klass)
  {
    if (!klass || !class_get_static_field_data)
      return nullptr;
    return class_get_static_field_data(klass);
  }

  // Get an RVA-based method address (absolute)
  inline uintptr_t GetMethodAddress(uintptr_t rva) { return gameAssemblyBase + rva; }

  // Read a field from an IL2CPP object instance
  template<typename T>
  inline T ReadField(void* obj, uintptr_t offset)
  {
    if (!obj)
      return T{};
    return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(obj) + offset);
  }

  // Write a field on an IL2CPP object instance
  template<typename T>
  inline void WriteField(void* obj, uintptr_t offset, T value)
  {
    if (!obj)
      return;
    *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(obj) + offset) = value;
  }

  // Resolve all game classes and cache them
  inline bool ResolveClasses()
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

    // Validate critical classes
    return Offsets::Classes::WeaponManager != 0 && Offsets::Classes::PlayerMoveC != 0
        && Offsets::Classes::WeaponSounds != 0;
  }

  // Get the WeaponManager singleton instance
  inline void* GetWeaponManagerInstance()
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

  // Get local Player_move_c from WeaponManager
  inline void* GetLocalPlayerMoveC()
  {
    auto wm = GetWeaponManagerInstance();
    if (!wm)
      return nullptr;
    return ReadField<void*>(wm, Offsets::WeaponManager::myPlayerMoveC);
  }

  // Get current WeaponSounds from WeaponManager
  inline void* GetCurrentWeaponSounds()
  {
    auto wm = GetWeaponManagerInstance();
    if (!wm)
      return nullptr;
    return ReadField<void*>(wm, Offsets::WeaponManager::currentWeaponSounds);
  }

  // Get PlayerDamageable from local player
  inline void* GetLocalPlayerDamageable()
  {
    auto pmc = GetLocalPlayerMoveC();
    if (!pmc)
      return nullptr;
    return ReadField<void*>(pmc, Offsets::PlayerMoveC::playerDamageable);
  }
}  // namespace IL2CPP
