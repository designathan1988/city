"""Shader cost read offline: each entry of a shader turned into plain GLSL, compiled to SPIR-V by the Khronos compiler, its instructions counted.
Counts of SPIR-V do not depend on a graphics card. The language's built-ins come from the engine's own documentation, kept outside the program."""
import concurrent.futures
import glob
import os
import re
import shutil
import struct
import subprocess
import tempfile
import urllib.request

from ..core import program_root

TABLE_ROW = re.compile(r"\|\s*(?:in|out|inout)?\s*([a-zA-Z_]\w*)\s+\*\*([A-Z_][A-Z0-9_]*)\*\*")
PAGES = ("spatial_shader", "canvas_item_shader", "particle_shader", "sky_shader", "fog_shader", "shading_language")
DOCS = "https://raw.githubusercontent.com/godotengine/godot-docs/master/tutorials/shaders/shader_reference/"
UNIFORM = re.compile(r"^\s*(?:global\s+|instance\s+)?uniform\s+(?:\w+\s+)?([\w]+)\s+(\w+)((?:\[\d*\w*\])?)[^;]*;", re.M)
VARYING = re.compile(r"^\s*varying\s+(?:flat\s+|smooth\s+)?(?:\w+\s+)?(\w+)\s+(\w+)((?:\[\w*\])?)\s*;", re.M)
ENTRY = re.compile(r"^\s*void\s+(vertex|fragment|light|start|process|sky|fog)\s*\(\s*\)", re.M)
CONDITION = re.compile(r"^\s*#(?:el)?if\s+([A-Za-z_]\w*)\s*(?:==|!=)\s*([A-Za-z_]\w*)", re.M)
IDENT = re.compile(r"\b[A-Z_][A-Z0-9_]*\b")
STAGE = {"vertex": "vert"}
SAMPLES = set(range(87, 98))
BRANCHES = {247, 250, 251}


def compiler():
    found = shutil.which("glslangValidator") or shutil.which("glslang")
    if found:
        return found
    sdk = os.environ.get("VULKAN_SDK")
    bases = [os.path.join(sdk, "Bin")] if sdk else sorted(glob.glob(os.path.join(os.path.splitdrive(program_root())[0] + os.sep, "VulkanSDK", "*", "Bin")))
    for base in reversed(bases):
        path = os.path.join(base, "glslangValidator.exe")
        if os.path.exists(path):
            return path
    return None


def built_ins():
    """name -> type, from the documentation pages, fetched once."""
    folder = os.path.join(program_root(), "dist", "arch", "external")
    out = {}
    for page in PAGES:
        path = os.path.join(folder, page + ".rst")
        if not os.path.exists(path):
            try:
                os.makedirs(folder, exist_ok=True)
                urllib.request.urlretrieve(DOCS + page + ".rst", path)
            except OSError:
                continue
        with open(path, encoding="utf8") as f:
            for type_, name in TABLE_ROW.findall(f.read()):
                out.setdefault(name, type_)
    return out


def _expand(text, read, seen):
    def one(m):
        path = m.group(1)
        if path in seen:
            return ""
        seen.add(path)
        return _expand(read(path) or "", read, seen)
    return re.sub(r'^\s*#include\s+"([^"]+)"', one, text, flags=re.M)


def _plain(text, entry, names, define):
    """The shader as GLSL for one entry: its declarations rewritten, other entries left out."""
    text = re.sub(r"^\s*(shader_type|render_mode|stencil_mode|group_uniforms)\b[^;]*;", "", text, flags=re.M)
    text = UNIFORM.sub(lambda m: f"uniform {m.group(1)} {m.group(2)}{m.group(3)};" if "sampler" in m.group(1) else f"{m.group(1)} {m.group(2)}{m.group(3)};", text)
    text = VARYING.sub(lambda m: f"{m.group(1)} {m.group(2)}{m.group(3)};", text)
    for other in set(ENTRY.findall(text)) - {entry}:
        m = re.search(rf"^\s*void\s+{other}\s*\(\s*\)\s*\{{", text, re.M)
        if m:
            depth, i = 0, m.end() - 1
            while i < len(text):
                depth += (text[i] == "{") - (text[i] == "}")
                if depth == 0:
                    break
                i += 1
            text = text[:m.start()] + text[i + 1:]
    used = {w for w in IDENT.findall(text) if w in names}
    head = ["#version 450", *define]
    if entry == "vertex":
        head += [f"#define {word}(v) (v)" for word in ("dFdx", "dFdy", "fwidth")]
    head += [f"{names[w]} {w};" if "sampler" not in names[w] else f"uniform {names[w]} {w};" for w in sorted(used)]
    return "\n".join(head) + "\n" + text + f"\nvoid main() {{ {entry}(); }}\n"


def _count(words):
    """Instructions inside functions, by what they cost."""
    ops = samples = branches = 0
    i = 5
    inside = False
    while i < len(words):
        head = words[i]
        op, n = head & 0xFFFF, head >> 16
        if n == 0:
            break
        if op == 54:
            inside = True
        elif op == 56:
            inside = False
        elif inside:
            ops += 1
            samples += op in SAMPLES
            branches += op in BRANCHES
        i += n
    return ops, samples, branches


def _compile(tool, source, stage, tmp, tag):
    src = os.path.join(tmp, f"{tag}.{stage}")
    out = os.path.join(tmp, f"{tag}.spv")
    with open(src, "w", encoding="utf8") as f:
        f.write(source)
    run = subprocess.run((tool, "-V", "-R", "--auto-map-bindings", "--auto-map-locations", "-o", out, src), capture_output=True, check=False)
    if run.returncode != 0 or not os.path.exists(out):
        lines = run.stdout.decode("utf8", "replace").splitlines()
        return None, " | ".join(x for x in lines if "ERROR" in x)[:300]
    with open(out, "rb") as f:
        data = f.read()
    return struct.unpack(f"<{len(data) // 4}I", data[:len(data) // 4 * 4]), ""


def scan(repo, tracked, scope, facts):
    """Adds a size fact to every shader that compiles; what does not compile is reported, with what the compiler said."""
    tool = compiler()
    names = built_ins()
    report = {"compiler": bool(tool), "compiled": 0, "failed": []}
    if tool is None:
        return report
    suffix = {}
    for p in tracked:
        parts = p.split("/")
        for i in range(len(parts)):
            suffix.setdefault("/".join(parts[i:]), p)

    def read(path):
        real = suffix.get(path.replace("res://", ""))
        return repo.text(tracked[real]) if real else None

    jobs = []
    with tempfile.TemporaryDirectory() as tmp:
        for file in sorted(p for p in scope if p.endswith(".gdshader")):
            text = _expand(repo.text(tracked[file]) or "", read, set())
            lefts = sorted({m[0] for m in CONDITION.findall(text)})
            rights = sorted({m[1] for m in CONDITION.findall(text)})
            options = rights or [""]
            for entry in sorted(set(ENTRY.findall(text))):
                for pick, right in enumerate(options):
                    define = [f"#define {r} {i + 1}" for i, r in enumerate(rights)] + [f"#define {w} {pick + 1}" for w in lefts if w not in rights]
                    jobs.append((file, entry, pick, _plain(text, entry, names, define)))
        with concurrent.futures.ThreadPoolExecutor(os.cpu_count()) as pool:
            results = list(pool.map(lambda j: _compile(tool, j[3], STAGE.get(j[1], "frag"), tmp, f"{abs(hash((j[0], j[1], j[2])))}"), jobs))
    sums = {}
    for (file, entry, pick, _), (words, error) in zip(jobs, results):
        if words is None:
            report["failed"].append((file, entry, error))
            continue
        report["compiled"] += 1
        ops, samples, branches = _count(words)
        total = sums.setdefault(file, [0, 0, 0])
        total[0] = max(total[0], ops)
        total[1] = max(total[1], samples)
        total[2] = max(total[2], branches)
    for file, (ops, samples, branches) in sorted(sums.items()):
        facts.cost("sh:" + file, "shader_size", file, 1, 0, (), operations=ops, samples=samples, branches=branches)
    report["failed"] = sorted(report["failed"])
    return report
