// =====================================================
// Pixel Gun 3D Trainer — Standalone Injector
// Injects trainer.dll into the game process
// =====================================================

#include <TlHelp32.h>
#include <Windows.h>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

// Console colors
#define COLOR_DEFAULT 7
#define COLOR_GREEN 10
#define COLOR_RED 12
#define COLOR_CYAN 11
#define COLOR_YELLOW 14
#define COLOR_PURPLE 13

void SetColor(int color) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); }

void PrintBanner()
{
  SetColor(COLOR_PURPLE);
  std::cout << R"(
    ╔══════════════════════════════════════════════╗
    ║       PIXEL GUN 3D PC TRAINER v1.0           ║
    ║                                              ║
    ║       DLL Injector                           ║
    ╚══════════════════════════════════════════════╝
    )" << std::endl;
  SetColor(COLOR_DEFAULT);
}

// Find process ID by name
DWORD FindProcess(const wchar_t* processName)
{
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE)
    return 0;

  PROCESSENTRY32W pe;
  pe.dwSize = sizeof(pe);

  if (Process32FirstW(snapshot, &pe)) {
    do {
      if (_wcsicmp(pe.szExeFile, processName) == 0) {
        CloseHandle(snapshot);
        return pe.th32ProcessID;
      }
    } while (Process32NextW(snapshot, &pe));
  }

  CloseHandle(snapshot);
  return 0;
}

// Check if a module is loaded in the target process
bool IsModuleLoaded(DWORD pid, const wchar_t* moduleName)
{
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
  if (snapshot == INVALID_HANDLE_VALUE)
    return false;

  MODULEENTRY32W me;
  me.dwSize = sizeof(me);

  if (Module32FirstW(snapshot, &me)) {
    do {
      if (_wcsicmp(me.szModule, moduleName) == 0) {
        CloseHandle(snapshot);
        return true;
      }
    } while (Module32NextW(snapshot, &me));
  }

  CloseHandle(snapshot);
  return false;
}

// Inject DLL via LoadLibraryW
bool InjectDLL(DWORD pid, const std::wstring& dllPath)
{
  // Open target process
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (!hProcess) {
    SetColor(COLOR_RED);
    std::cout << "  [!] Failed to open process. Run as Administrator!" << std::endl;
    SetColor(COLOR_DEFAULT);
    return false;
  }

  // Allocate memory in target process for DLL path
  size_t pathSize = (dllPath.length() + 1) * sizeof(wchar_t);
  void*  remoteMem =
    VirtualAllocEx(hProcess, nullptr, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!remoteMem) {
    SetColor(COLOR_RED);
    std::cout << "  [!] Failed to allocate memory in target process." << std::endl;
    SetColor(COLOR_DEFAULT);
    CloseHandle(hProcess);
    return false;
  }

  // Write DLL path to target process
  if (!WriteProcessMemory(hProcess, remoteMem, dllPath.c_str(), pathSize, nullptr)) {
    SetColor(COLOR_RED);
    std::cout << "  [!] Failed to write DLL path to target process." << std::endl;
    SetColor(COLOR_DEFAULT);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return false;
  }

  // Get LoadLibraryW address
  FARPROC loadLibrary = GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");
  if (!loadLibrary) {
    SetColor(COLOR_RED);
    std::cout << "  [!] Failed to resolve LoadLibraryW." << std::endl;
    SetColor(COLOR_DEFAULT);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return false;
  }

  // Create remote thread to call LoadLibraryW
  HANDLE hThread = CreateRemoteThread(
    hProcess, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(loadLibrary), remoteMem, 0,
    nullptr
  );

  if (!hThread) {
    SetColor(COLOR_RED);
    std::cout << "  [!] Failed to create remote thread." << std::endl;
    SetColor(COLOR_DEFAULT);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return false;
  }

  // Wait for injection to complete
  WaitForSingleObject(hThread, 5000);

  // Check exit code
  DWORD exitCode = 0;
  GetExitCodeThread(hThread, &exitCode);

  // Cleanup
  CloseHandle(hThread);
  VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
  CloseHandle(hProcess);

  return exitCode != 0;
}

int main()
{
  SetConsoleTitleW(L"PG3D Trainer Injector");
  PrintBanner();

  // Find DLL path (same directory as injector)
  wchar_t exePath[MAX_PATH];
  GetModuleFileNameW(nullptr, exePath, MAX_PATH);
  std::filesystem::path dllPath = std::filesystem::path(exePath).parent_path() / L"trainer.dll";

  if (!std::filesystem::exists(dllPath)) {
    SetColor(COLOR_RED);
    std::cout << "  [!] trainer.dll not found!" << std::endl;
    std::cout << "  [!] Place trainer.dll next to the injector." << std::endl;
    SetColor(COLOR_DEFAULT);
    std::cout << "\n  Press any key to exit..." << std::endl;
    std::cin.get();
    return 1;
  }

  SetColor(COLOR_GREEN);
  std::cout << "  [+] Found trainer.dll" << std::endl;
  SetColor(COLOR_DEFAULT);

  // Wait for game process
  SetColor(COLOR_YELLOW);
  std::cout << "  [*] Waiting for Pixel Gun 3D..." << std::endl;
  SetColor(COLOR_DEFAULT);

  DWORD pid = 0;
  while (pid == 0) {
    pid = FindProcess(L"Pixel Gun 3D.exe");
    if (pid == 0)
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  SetColor(COLOR_GREEN);
  std::cout << "  [+] Found game process (PID: " << pid << ")" << std::endl;
  SetColor(COLOR_DEFAULT);

  // Wait for GameAssembly.dll to load (IL2CPP runtime)
  SetColor(COLOR_YELLOW);
  std::cout << "  [*] Waiting for GameAssembly.dll..." << std::endl;
  SetColor(COLOR_DEFAULT);

  while (!IsModuleLoaded(pid, L"GameAssembly.dll")) {
    // Check if process is still alive
    if (FindProcess(L"Pixel Gun 3D.exe") == 0) {
      SetColor(COLOR_RED);
      std::cout << "  [!] Game process exited." << std::endl;
      SetColor(COLOR_DEFAULT);
      std::cout << "\n  Press any key to exit..." << std::endl;
      std::cin.get();
      return 1;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  SetColor(COLOR_GREEN);
  std::cout << "  [+] GameAssembly.dll loaded" << std::endl;
  SetColor(COLOR_DEFAULT);

  // Wait a bit for IL2CPP to fully initialize
  SetColor(COLOR_YELLOW);
  std::cout << "  [*] Waiting for IL2CPP initialization..." << std::endl;
  SetColor(COLOR_DEFAULT);
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // Check if trainer is already injected
  if (IsModuleLoaded(pid, L"trainer.dll")) {
    SetColor(COLOR_YELLOW);
    std::cout << "  [!] Trainer is already injected!" << std::endl;
    SetColor(COLOR_DEFAULT);
    std::cout << "\n  Press any key to exit..." << std::endl;
    std::cin.get();
    return 0;
  }

  // Inject
  SetColor(COLOR_CYAN);
  std::cout << "  [*] Injecting trainer.dll..." << std::endl;
  SetColor(COLOR_DEFAULT);

  std::wstring fullDllPath = dllPath.wstring();
  if (InjectDLL(pid, fullDllPath)) {
    SetColor(COLOR_GREEN);
    std::cout << "  [+] Injection successful!" << std::endl;
    std::cout << "  [+] Press INSERT in-game to toggle menu" << std::endl;
    std::cout << "  [+] Press END to unload trainer" << std::endl;
    SetColor(COLOR_DEFAULT);
  }
  else {
    SetColor(COLOR_RED);
    std::cout << "  [!] Injection failed!" << std::endl;
    std::cout << "  [!] Try running as Administrator." << std::endl;
    SetColor(COLOR_DEFAULT);
  }

  std::cout << "\n  Press any key to exit..." << std::endl;
  std::cin.get();
  return 0;
}
