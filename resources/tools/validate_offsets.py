#!/usr/bin/env python3
import re
import sys
import os

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
DUMP_FILE = os.path.join(PROJECT_ROOT, "resources", "dumped", "dump.cs")
OFFSETS_FILE = os.path.join(PROJECT_ROOT, "src", "utils", "offsets.h")

def run_update():
    if not os.path.exists(DUMP_FILE):
        print(f"Error: dump.cs not found at {DUMP_FILE}")
        sys.exit(1)

    print("=== Parsing dump.cs ===")
    class_data = {}
    current_class = ""
    pending_rva = None

    with open(DUMP_FILE, "r", encoding="utf-8") as f:
        for line in f:
            # Check for class declaration
            class_match = re.search(r'class\s+(\w+)\s*.*//\s*TypeDefIndex:\s*\d+', line)
            if class_match:
                current_class = class_match.group(1)
                if current_class not in class_data:
                    class_data[current_class] = {"fields": {}, "methods": {}}
                continue

            if not current_class:
                continue

            # Check for fields
            # e.g., public int ammoInClip; // 0x6C
            field_match = re.search(r'\s+(\w+);\s*//\s*(0x[0-9a-fA-F]+)', line)
            if field_match:
                class_data[current_class]["fields"][field_match.group(1)] = field_match.group(2)
                continue

            # Check for methods
            # e.g., // RVA: 0x1DB8410 Offset: 0x1DB7810
            rva_match = re.search(r'//\s*RVA:\s*(0x[0-9a-fA-F]+)', line)
            if rva_match:
                pending_rva = rva_match.group(1)
                continue

            if pending_rva:
                # Next line contains method signature
                # e.g., public void ApplyDamage(...)
                # e.g., private static void ShowBanner()
                method_match = re.search(r'(?:[\w<>\[\]\.,]+\s+)+(\w+)\s*\(', line)
                if method_match:
                    class_data[current_class]["methods"][method_match.group(1)] = pending_rva
                pending_rva = None

    print(f"Parsed {len(class_data)} classes from dump.cs.")

    print("\n=== Updating offsets.h ===")
    
    ns_to_class = {
        "WeaponManager": "WeaponManager",
        "PlayerMoveC": "Player_move_c",
        "WeaponSounds": "WeaponSounds",
        "PlayerDamageable": "PlayerDamageable",
        "Object": "Object",
        "AntiCheat": "CheatDetectedBanner",
    }

    with open(OFFSETS_FILE, "r", encoding="utf-8") as f:
        lines = f.readlines()

    current_ns = ""
    updated_lines = []
    updates_count = 0
    missing_count = 0

    for line in lines:
        # Match start of namespace
        ns_match = re.match(r'\s*namespace\s+(\w+)\s*\{', line)
        if ns_match:
            current_ns = ns_match.group(1)
            updated_lines.append(line)
            continue
        
        # Match end of namespace
        if "}" in line and "namespace" in line:
            current_ns = ""

        # Match a constexpr uintptr_t variable
        constexpr_match = re.search(r'(constexpr\s+uintptr_t\s+(\w+)\s*=\s*)(0x[0-9a-fA-F]+)(;.*)', line)
        if constexpr_match and current_ns in ns_to_class:
            prefix = constexpr_match.group(1)
            var_name = constexpr_match.group(2)
            old_hex = constexpr_match.group(3)
            suffix = constexpr_match.group(4)
            
            target_class = ns_to_class[current_ns]
            new_hex = None

            if var_name.endswith("_RVA"):
                method_name = var_name.replace("_RVA", "")
                if current_ns == "AntiCheat" and method_name.startswith("CBD_"):
                    method_name = method_name.replace("CBD_", "")
                
                new_hex = class_data.get(target_class, {}).get("methods", {}).get(method_name)
            else:
                new_hex = class_data.get(target_class, {}).get("fields", {}).get(var_name)

            if new_hex:
                if new_hex.lower() != old_hex.lower():
                    print(f"[UPDATED] {current_ns}::{var_name}: {old_hex} -> {new_hex}")
                    line = f"{prefix}{new_hex}{suffix}\n"
                    updates_count += 1
                else:
                    print(f"[OK] {current_ns}::{var_name} is up-to-date ({old_hex}).")
            else:
                if var_name != "StaticInstance":
                    print(f"[WARNING] {current_ns}::{var_name} NOT FOUND in dump.cs (class {target_class})!")
                    missing_count += 1

        updated_lines.append(line)

    with open(OFFSETS_FILE, "w", encoding="utf-8") as f:
        f.writelines(updated_lines)

    print("\n==================================")
    print(f"[SUCCESS] Updated offsets.h with {updates_count} changes. {missing_count} fields missing.")

if __name__ == "__main__":
    run_update()
