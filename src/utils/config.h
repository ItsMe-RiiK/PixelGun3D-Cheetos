#pragma once
#include <string>

namespace Config
{
  extern std::string configPath;

  void SetConfigPath(const std::string& dllPath);
  void Save();
  void Load();
}  // namespace Config
