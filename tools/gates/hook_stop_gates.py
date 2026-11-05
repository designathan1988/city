#!/usr/bin/env python3
"""Claude Code Stop hook: when the turn ends with uncommitted changes in the repository, the gates run once and, if
they fail, the session gets the list back and keeps working. A turn without changes ends in silence, and a turn that is
already a continuation of this hook (stop_hook_active) is never held again. A changed C++ source newer than one of
the two DLLs (release and debug) also holds the turn: the build is evidence the program checks, not a claim."""
import json
import os
import subprocess
import sys

IGNORED_SUFFIXES = (".uid",)
REASON_LIMIT = 4000
CPP_ROOTS = ("native/src/", "native/CMakeLists.txt")
DLLS = ("project/bin/libcitycore.windows.template_release.x86_64.dll",
        "project/bin/libcitycore.windows.template_debug.x86_64.dll")
BUILD_CMD = "MINGW=<llvm-mingw> JOBS=4 bash tools/build/build.sh core-windows"


def changed_files(root):
    out = subprocess.run(["git", "status", "--porcelain"], cwd=root, capture_output=True, text=True,
                         encoding="utf8", errors="replace")
    if out.returncode != 0:
        return []
    files = [line[3:].strip() for line in out.stdout.splitlines() if len(line) > 3]
    return [f for f in files if not f.endswith(IGNORED_SUFFIXES)]


def stale_build(root, files):
    """The changed C++ sources newer than the oldest DLL, and the DLLs that do not exist."""
    sources = [f for f in files if f.startswith(CPP_ROOTS) and os.path.exists(os.path.join(root, f))]
    if not sources:
        return [], []
    missing = [d for d in DLLS if not os.path.exists(os.path.join(root, d))]
    if missing:
        return sources, missing
    oldest = min(os.path.getmtime(os.path.join(root, d)) for d in DLLS)
    return [f for f in sources if os.path.getmtime(os.path.join(root, f)) > oldest], []


def main():
    try:
        data = json.load(sys.stdin)
    except ValueError:
        return 0
    if data.get("stop_hook_active"):
        return 0
    root = os.path.abspath(os.environ.get("CLAUDE_PROJECT_DIR") or data.get("cwd") or ".")
    gates = os.path.join(root, "tools", "gates", "run_gates.py")
    files = changed_files(root)
    if not os.path.exists(gates) or not files:
        return 0
    reasons = []
    res = subprocess.run([sys.executable, gates], cwd=root, capture_output=True, text=True, encoding="utf8",
                         errors="replace")
    if res.returncode != 0:
        reasons.append("Gates falharam com as mudanças deste turno. Corrija a causa (código, dados ou mapa), nunca a "
                       "regra:\n" + (res.stdout + res.stderr).strip()[-REASON_LIMIT:])
    newer, missing = stale_build(root, files)
    if newer or missing:
        what = ("DLL ausente: " + ", ".join(missing)) if missing else ("fonte mais nova que a DLL: " + ", ".join(newer))
        reasons.append("C++ mudou e as DLLs de release e de debug não foram recompiladas (" + what + "). Rode " +
                       BUILD_CMD + " e corrija todo erro antes de concluir.")
    if not reasons:
        return 0
    sys.stdout.reconfigure(encoding="utf-8")
    print(json.dumps({"decision": "block", "reason": "\n\n".join(reasons)}, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    sys.exit(main())
