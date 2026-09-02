#!/usr/bin/env python3
"""
Il2CppDumper Unicode Deobfuscator
Renames classes/methods/fields that contain Chinese/Japanese characters
to a readable name and generates a mapping.
"""

import re
import sys
from pathlib import Path
from collections import defaultdict

# Detect CJK characters (Chinese / Japanese / Korean)
CJK_RANGE = re.compile(
    r'[\u4e00-\u9fff\u3400-\u4dbf\u3040-\u309f\u30a0-\u30ff\uac00-\ud7af]'
)

def is_obfuscated(name: str) -> bool:
    """Check if the name contains CJK characters or is obfuscated"""
    if not name:
        return False
    if CJK_RANGE.search(name):
        return True
    # Names that are just weird symbols are also considered obfuscated
    if re.fullmatch(r'[^\w\s\.<>]+', name):
        return True
    return False


def clean_and_deobfuscate(input_path: str, output_path: str = "cleanDump.cs"):
    input_path = Path(input_path)
    output_path = Path(output_path)
    mapping_path = output_path.with_name(output_path.stem + "_mapping.txt")

    print(f"[+] Reading {input_path} ...")

    # Counter for new names
    counters = defaultdict(int)
    name_map = {}          # original -> new
    reverse_map = {}       # new -> original

    def get_new_name(original: str, kind: str = "Class") -> str:
        if original in name_map:
            return name_map[original]

        counters[kind] += 1
        new_name = f"{kind}_{counters[kind]:04d}"
        name_map[original] = new_name
        reverse_map[new_name] = original
        return new_name

    # Pattern to capture declarations
    # class / struct / interface / enum
    type_decl = re.compile(
        r'^(\s*(?:public|private|protected|internal|static|sealed|abstract|partial)*\s*)'
        r'(class|struct|interface|enum)\s+([^\s:{]+)'
    )

    # method / constructor
    method_decl = re.compile(
        r'^(\s*(?:public|private|protected|internal|static|virtual|override|abstract|sealed|extern|new)*\s*)'
        r'(?:[\w.<>,\[\]\s]+\s+)?([^\s\(]+)\s*\('
    )

    # field
    field_decl = re.compile(
        r'^(\s*(?:public|private|protected|internal|static|readonly|const)*\s*)'
        r'([\w.<>,\[\]]+)\s+([^\s;=]+)\s*[;=]'
    )

    # property
    prop_decl = re.compile(
        r'^(\s*(?:public|private|protected|internal|static|virtual|override)*\s*)'
        r'([\w.<>,\[\]]+)\s+([^\s{;]+)\s*\{'
    )

    skip_patterns = [
        re.compile(r'^\s*//\s*RVA:'),
        re.compile(r'^\s*/\*\s*GenericInstMethod'),
        re.compile(r'^\s*\|-'),
        re.compile(r'^\s*\*/'),
        re.compile(r'//\s*TypeDefIndex:\s*\d+'),
        re.compile(r'//\s*0x[0-9A-Fa-f]+'),
        re.compile(r'\[DebuggerNonUserCode\]'),
        re.compile(r'\[DebuggerHidden\]'),
        re.compile(r'\[DebuggerBrowsable\(null\)\]'),
        re.compile(r'\[CompilerGenerated\]'),
    ]

    attr_clean = re.compile(r'^\s*\[.*?\]\s*$')

    cleaned_lines = []
    writing = False
    line_count = 0
    kept = 0
    image_list_done = False

    with input_path.open("r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            line_count += 1
            if line_count % 400000 == 0:
                print(f"    ... {line_count:,} lines | renamed: {len(name_map)}")

            # Skip the Image list
            if not image_list_done:
                if line.startswith("// Image "):
                    continue
                if line.strip() == "" or "Namespace" in line or any(x in line for x in ["class ", "struct ", "interface ", "enum "]):
                    image_list_done = True
                    writing = True
                    cleaned_lines.append("// ===== Assembly-CSharp.dll (Unicode Deobfuscated) =====\n\n")
                else:
                    continue

            if not writing:
                continue

            if line.startswith("// Image ") and "Assembly-CSharp" not in line:
                break

            # Skip noise
            if any(p.search(line) for p in skip_patterns):
                continue
            if attr_clean.match(line) and not any(x in line for x in
                ["SerializeField", "Header", "Tooltip", "Range", "Obsolete", "Flags", "Serializable"]):
                continue

            original_line = line

            # === Deobfuscate Type (class/struct/interface/enum) ===
            m = type_decl.match(line)
            if m:
                prefix, kind, name = m.groups()
                # Get the pure name (remove generics)
                pure_name = name.split('<')[0].split('.')[-1]
                if is_obfuscated(pure_name):
                    new_name = get_new_name(pure_name, kind.capitalize())
                    line = line.replace(name, name.replace(pure_name, new_name), 1)

            # === Deobfuscate Method ===
            else:
                m = method_decl.match(line)
                if m and '(' in line:
                    name = m.group(2)
                    pure_name = name.split('<')[0].split('.')[-1]
                    # Skip constructor & operator
                    if pure_name not in ('.ctor', '.cctor') and not pure_name.startswith('op_'):
                        if is_obfuscated(pure_name):
                            new_name = get_new_name(pure_name, "Method")
                            line = line.replace(name, name.replace(pure_name, new_name), 1)

            # === Deobfuscate Field ===
            m = field_decl.match(line)
            if m:
                name = m.group(3)
                pure_name = name.split('<')[0]
                if is_obfuscated(pure_name):
                    new_name = get_new_name(pure_name, "Field")
                    line = line.replace(name, new_name, 1)

            # === Deobfuscate Property ===
            m = prop_decl.match(line)
            if m:
                name = m.group(3)
                pure_name = name.split('<')[0]
                if is_obfuscated(pure_name):
                    new_name = get_new_name(pure_name, "Prop")
                    line = line.replace(name, new_name, 1)

            # Clean up remaining TypeDefIndex & offset comments
            line = re.sub(r'\s*//\s*TypeDefIndex:\s*\d+', '', line)
            line = re.sub(r';\s*//\s*0x[0-9A-Fa-f]+', '', line)

            cleaned_lines.append(line)
            kept += 1

    # Write clean output
    print(f"[+] Writing {output_path} ...")
    with output_path.open("w", encoding="utf-8") as out:
        out.writelines(cleaned_lines)

    # Write mapping
    print(f"[+] Writing mapping to {mapping_path} ...")
    with mapping_path.open("w", encoding="utf-8") as mp:
        mp.write("# Mapping: NewName -> Original Obfuscated Name\n\n")
        for new, orig in sorted(reverse_map.items()):
            mp.write(f"{new:20}  -->  {orig}\n")

    print(f"\n[✓] Done!")
    print(f"    Original lines : {line_count:,}")
    print(f"    Cleaned lines  : {kept:,}")
    print(f"    Names replaced : {len(name_map):,}")
    print(f"    Output         : {output_path.resolve()}")
    print(f"    Mapping        : {mapping_path.resolve()}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python deobfuscate_unicode.py <dump.cs> [output.cs]")
        sys.exit(1)

    inp = sys.argv[1]
    out = sys.argv[2] if len(sys.argv) > 2 else "cleanDump.cs"
    clean_and_deobfuscate(inp, out)
