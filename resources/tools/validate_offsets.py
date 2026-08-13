#!/usr/bin/env python3
import re
import sys
import os

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../.."))
DUMP_FILE = os.path.join(PROJECT_ROOT, "resources", "dumped", "dump.cs")
OFFSETS_FILE = os.path.join(PROJECT_ROOT, "src", "utils", "offsets.h")
IL2CPP_FILE = os.path.join(PROJECT_ROOT, "src", "utils", "il2cpp.cpp")

def parse_offsets_h():
    offsets = {}
    current_namespace = ""
    with open(OFFSETS_FILE, "r") as f:
        for line in f:
            line = line.strip()
            ns_match = re.match(r'namespace\s+(\w+)\s*\{', line)
            if ns_match:
                current_namespace = ns_match.group(1)
                offsets[current_namespace] = []
                continue
            
            constexpr_match = re.search(r'constexpr\s+uintptr_t\s+(\w+)\s*=\s*(0x[0-9a-fA-F]+);', line)
            if constexpr_match:
                offsets[current_namespace].append((constexpr_match.group(1), constexpr_match.group(2)))
    return offsets

def parse_il2cpp_cpp():
    classes = {}
    with open(IL2CPP_FILE, "r") as f:
        for line in f:
            match = re.search(r'Offsets::Classes::(\w+)\s*=\s*reinterpret_cast<uintptr_t>\(GetClass\("([^"]+)"\)\);', line)
            if match:
                classes[match.group(1)] = match.group(2)
    return classes

def run_validation():
    if not os.path.exists(DUMP_FILE):
        print(f"Error: dump.cs not found at {DUMP_FILE}")
        sys.exit(1)

    print("=== Parsing offsets.h & il2cpp.cpp ===")
    offsets = parse_offsets_h()
    il2cpp_classes = parse_il2cpp_cpp()

    print("\n=== Validating IL2CPP Class Mapping ===")
    with open(DUMP_FILE, "r", encoding="utf-8") as f:
        dump_content = f.read()

    mismatches = 0
    for cpp_name, game_name in il2cpp_classes.items():
        if game_name in dump_content:
            print(f"[UNCHANGED] Class '{game_name}' ({cpp_name}) found.")
        else:
            print(f"[CHANGED/MISSING] Class '{game_name}' ({cpp_name}) NOT found!")
            mismatches += 1

    print("\n=== Validating Field & RVA Offsets ===")
    
    with open(DUMP_FILE, "r", encoding="utf-8") as f:
        lines = f.readlines()
        
    classes_lines = {}
    current_class = ""
    for line in lines:
        class_match = re.search(r'class\s+(\w+)\s*.*//\s*TypeDefIndex:\s*\d+', line)
        if class_match:
            current_class = class_match.group(1)
            classes_lines[current_class] = []
        elif current_class != "":
            classes_lines[current_class].append(line)

    ns_to_class = {
        "WeaponManager": "WeaponManager",
        "PlayerMoveC": "Player_move_c",
        "WeaponSounds": "WeaponSounds",
        "PlayerDamageable": "PlayerDamageable",
        "FPSController": "FirstPersonControlSharp",
        "InnerWeaponPars": "InnerWeaponPars",
        "ExtractionWeaponSettings": "ExtractionWeaponSettings"
    }

    # For offset checking, we just verify the exact hexadecimal offset exists in dump.cs
    # Because field names get obfuscated
    for ns, fields in offsets.items():
        if not fields: continue
        target_class = ns_to_class.get(ns)
        
        for name, val_hex in fields:
            if name.endswith("_RVA"):
                method_name = name.replace("_RVA", "")
                if f"// RVA: {val_hex}" not in dump_content:
                    print(f"[CHANGED/MISSING] {ns}::{name}: Method RVA {val_hex} not found in dump.cs")
                    mismatches += 1
                else:
                    print(f"[UNCHANGED] {ns}::{name} ({val_hex})")
            else:
                if val_hex not in dump_content:
                     print(f"[CHANGED/MISSING] {ns}::{name}: Offset {val_hex} not found in dump.cs")
                     mismatches += 1
                else:
                    print(f"[UNCHANGED] {ns}::{name} ({val_hex})")

    print("\n==================================")
    if mismatches == 0:
        print("[SUCCESS] All offsets and IL2CPP classes are valid against dump.cs!")
    else:
        print(f"[WARNING] Found {mismatches} mismatch(es)!")

if __name__ == "__main__":
    run_validation()
