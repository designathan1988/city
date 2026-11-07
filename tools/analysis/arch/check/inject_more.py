"""Injected cases that need a file of their own kind: tools, build, project settings, imports and generated files."""


def files(w, expect, api, places, join, stem):
    def case(cid):
        word = w()
        expect.append((cid, word))
        return word

    py, proc, gen, env = case("D17"), case("B88"), case("B89"), case("D15")
    build, imp, filt, aut = case("B91"), case("D12"), w(), w()
    expect.append(("B92", "exclude_filter"))
    out = {}
    tool = join(places["py"] or "tools", f"{py}_tool.py")
    target = join(places["json"] or "project", f"{gen}_made.json")
    out[tool] = f'''"""Injected tool."""
import subprocess

OUT = "{target}"


def {gen}_write():
    with open("{target}", "w") as f:
        f.write("{{}}")


def {proc}_run():
    subprocess.run(["{proc}_tool", "{stem}.json"])
'''
    out[target] = "{}\n"
    out[join(places["py"] or "tools", f"{env}_step.sh")] = f"#!/bin/sh\n# Injected script.\n{env}_FLAG=1 cmake --version\n./{py}_tool.py\n"
    out[join("native", f"{build}_lib/CMakeLists.txt")] = f"# Injected build.\ncmake_minimum_required(VERSION 3.20)\nproject({build}_lib)\nadd_library({build}_lib SHARED {stem}.cpp)\n"
    out["project/project.godot"] = f'config_version=5\n\n[application]\n\nrun/main_scene="res://{stem}.tscn"\nconfig/features=PackedStringArray("4.7")\n\n[autoload]\n\n{aut}_Auto="*res://{stem}.gd"\n\n[shader_globals]\n\n{aut}_global={{"type":"float","value":0.0}}\n'
    out["project/export_presets.cfg"] = f'[preset.0]\n\nname="{filt}"\nexport_filter="all_resources"\ninclude_filter="data/*.json"\nexclude_filter="tests/*"\n'
    tex = join(places["gd"] or "project", f"{imp}_map.png")
    out[tex + ".import"] = f'[remap]\n\nimporter="texture"\nsource_file="res://{imp}_map.png"\n\n[params]\n\ncompress/mode=0\n'
    expect.append(("texture_bandwidth", imp))
    expect.append(("B79", imp))
    key = next(c[1] for c in expect if c[0] == "B52")
    out[join(places["json"] or "project", f"{stem}_strings.csv")] = f"keys,en\n{key}.one,One\n{key}.two,Two\n"
    out[join(places["shader"] or "project", f"{stem}_tiny.gdshader")] = "// Injected small shader.\nshader_type spatial;\nvoid fragment() {\n	ALBEDO = vec3(1.0);\n}\n"
    typed = w()
    expect.append(("B1", typed))
    out[f"tools/{typed}.ts"] = f"// Injected script.\nexport function {typed}_one(): number {{ return 1; }}\nexport function {typed}_two(): number {{ return {typed}_one(); }}\n"
    test = case("B77")
    native = next(c[1] for c in expect if c[0] == "test_code_shipped")
    expect.append(("B99", test))
    out[f"project/tests/{test}_test.gd"] = f'# Injected test.\nextends Node\n\nfunc _ready() -> void:\n	{test}_only()\n\nfunc {test}_only() -> void:\n	self.call(&"{native}_native")\n'
    return out
