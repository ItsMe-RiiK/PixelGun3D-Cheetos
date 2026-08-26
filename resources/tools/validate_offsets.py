#!/usr/bin/env python3
import re
import sys
import os

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
DUMP_FILE = os.path.join(PROJECT_ROOT, "resources", "dumped", "static", "dump.cs")
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

    with open(OFFSETS_FILE, "r", encoding="utf-8") as f:
        lines = f.readlines()

    ns_to_class = {}
    last_class_name = None
    for line in lines:
        class_match = re.search(r'//\s+([\w_\.]+)\s*\(TypeDefIndex', line)
        if class_match:
            last_class_name = class_match.group(1)
            if last_class_name.startswith("UnityEngine."):
                last_class_name = last_class_name.replace("UnityEngine.", "")
            continue

        ns_match = re.match(r'\s*namespace\s+(\w+)', line)
        if ns_match and last_class_name:
            ns_name = ns_match.group(1)
            if ns_name != "Offsets":
                ns_to_class[ns_name] = last_class_name
            last_class_name = None

    current_ns = ""
    updated_lines = []
    updates_count = 0
    missing_count = 0

    for line in lines:
        # Match start of namespace
        ns_match = re.match(r'\s*namespace\s+(\w+)', line)
        if ns_match:
            current_ns = ns_match.group(1)
            updated_lines.append(line)
            continue

        # Match end of namespace
        if "}" in line and "namespace" in line:
            current_ns = ""

        # Match a constexpr uintptr_t variable
        constexpr_match = re.search(r'(constexpr\s+uintptr_t\s+(\w+)\s*=\s*)(0x[0-9a-fA-F]+)(;.*)', line)
        if constexpr_match:
            prefix = constexpr_match.group(1)
            var_name = constexpr_match.group(2)
            old_hex = constexpr_match.group(3)
            suffix = constexpr_match.group(4)

            if current_ns in ns_to_class:
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
                        # Fallback check: is the old_hex still present in the class under ANY obfuscated name?
                        found_obf_name = None
                        search_dict = class_data.get(target_class, {}).get("methods", {}) if var_name.endswith("_RVA") else class_data.get(target_class, {}).get("fields", {})

                        for obf_name, obf_hex in search_dict.items():
                            if obf_hex.lower() == old_hex.lower():
                                found_obf_name = obf_name
                                break

                        if found_obf_name:
                            print(f"[OK] {current_ns}::{var_name} is up-to-date ({old_hex} via obfuscated '{found_obf_name}').")
                        else:
                            print(f"[WARNING] {current_ns}::{var_name} NOT FOUND in dump.cs (class {target_class})!")
                            missing_count += 1
            else:
                if current_ns == "IL2CPPStructs":
                    print(f"[IGNORED] {current_ns}::{var_name} (Internal IL2CPP struct, skipping validation)")
                elif current_ns == "LiveWeapon":
                    print(f"[IGNORED] {current_ns}::{var_name} (Dynamic resolution fallback, skipping validation)")
                else:
                    print(f"[WARNING] {current_ns}::{var_name} ABANDONED (No TypeDefIndex mapping for namespace {current_ns})!")
                    missing_count += 1

        updated_lines.append(line)

    if "--update" in sys.argv:
        with open(OFFSETS_FILE, "w", encoding="utf-8") as f:
            f.writelines(updated_lines)
        print("\n[INFO] Applied updates to offsets.h")
    else:
        print("\n[INFO] Run with 'update' to apply these changes to offsets.h")

    print("\n=== Validating dynamic offsets in il2cpp.cpp ===")
    il2cpp_file = os.path.join(PROJECT_ROOT, "src", "utils", "il2cpp.cpp")

    dynamic_var_to_class = {}

    if os.path.exists(il2cpp_file):
        with open(il2cpp_file, "r", encoding="utf-8") as f:
            content = f.read()

            # Parse namespaces to map dynamic variables
            ns_blocks = re.finditer(r'namespace\s+Offsets\s*\{\s*namespace\s+(\w+)\s*\{([\s\S]*?)\}\s*\}', content)
            for block_match in ns_blocks:
                ns_name = block_match.group(1)
                block_content = block_match.group(2)

                var_matches = re.finditer(r'(\w+)\s*=\s*IL2CPP::ResolveFieldOffset', block_content)
                for v_match in var_matches:
                    var_name = v_match.group(1)
                    dynamic_var_to_class[var_name] = ns_to_class.get(ns_name, None)

            matches = re.finditer(r'(\w+)\s*=\s*IL2CPP::ResolveFieldOffset\([^,]+,\s*\{([^}]+)\}', content)
            for m in matches:
                var_name = m.group(1)
                names_str = m.group(2)
                names = re.findall(r'"([^"]+)"', names_str)

                target_class = dynamic_var_to_class.get(var_name)

                found = False
                for name in names:
                    if target_class is None:
                        for cls, data in class_data.items():
                            if name in data.get("fields", {}):
                                found = True
                                print(f"[OK] Dynamic offset {var_name} resolved via '{name}' (found in {cls})")
                                break
                    else:
                        if name in class_data.get(target_class, {}).get("fields", {}):
                            found = True
                            print(f"[OK] Dynamic offset {target_class}::{var_name} resolved via '{name}'")
                            break
                    if found:
                        break
                if not found:
                    if target_class is None:
                        print(f"[WARNING] Dynamic offset {var_name} NOT FOUND in dump.cs (tried: {names})! You may need to add its obfuscated name to the fallback list in il2cpp.cpp.")
                    else:
                        print(f"[WARNING] Dynamic offset {target_class}::{var_name} NOT FOUND in dump.cs (tried: {names})! You may need to add its obfuscated name to the fallback list in il2cpp.cpp.")
                    missing_count += 1

    print("\n==================================")
    print(f"[SUCCESS] Updated offsets.h with {updates_count} changes. {missing_count} fields missing.")

if __name__ == "__main__":
    run_update()
