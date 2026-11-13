# Where this machine's tools are (Godot, Git for Windows' bash, llvm-mingw, the C API library of clang): the one owner of their locations, so no path of one
# machine is written in the tools. A tool is its environment variable, else that name in tools/tasks/local.json (this machine's
# own file, ignored by git), else what its locator finds on the PATH; with none of them the selection stops naming what to set.
# Run as a script it prints a tool's location (tools/build/pgo.sh): python tools/tasks/locations.py godot
from __future__ import annotations

import json
import os
import shutil
import sys
from pathlib import Path

LOCAL = Path(__file__).resolve().parent / "local.json"
GIT_DEPTH = 3  # git.exe lies at most this deep under Git for Windows' root (cmd/, mingw64/bin/, ucrt64/bin/)


class Missing(ValueError):
    """A tool found nowhere: the message names its variable and the local file."""


def _local(key: str) -> str:
    if not LOCAL.is_file():
        return ""
    value = json.loads(LOCAL.read_text(encoding="utf-8")).get(key, "")
    return value if isinstance(value, str) else ""


def _godot() -> str:
    for name in ("godot_console", "godot4", "godot"):  # the console build first: it writes to the run's log
        found = shutil.which(name)
        if found:
            return Path(found).as_posix()
    return ""


def _bash() -> str:
    """Git for Windows' own bash, found from its git (never the WSL bash System32 puts on the PATH); bash elsewhere."""
    if os.name != "nt":
        found = shutil.which("bash")
        return Path(found).as_posix() if found else ""
    git = shutil.which("git")
    if not git:
        return ""
    folder = Path(git).resolve().parent
    for _ in range(GIT_DEPTH):
        bash = folder / "bin" / "bash.exe"
        if bash.is_file():
            return bash.as_posix()
        folder = folder.parent
    return ""


def _mingw() -> str:
    """llvm-mingw's root: the folder above the bin/ holding its Windows cross compiler."""
    found = shutil.which("x86_64-w64-mingw32-clang++")
    return Path(found).resolve().parent.parent.as_posix() if found else ""


def _libclang() -> str:
    """The C API library of the clang on the PATH (libclang: what clang's Python binding loads), beside its compiler; the
    binding's own copy may be older than the headers a toolchain ships and read them wrong."""
    found = shutil.which("clang")
    if not found:
        return ""
    folder = Path(found).resolve().parent
    for name in ("libclang.dll", "libclang.so", "libclang.dylib"):
        for place in (folder / name, folder.parent / "lib" / name):
            if place.is_file():
                return place.as_posix()
    return ""


def _nowhere() -> str:
    """A place no search finds: it is this machine's to name."""
    return ""


TOOLS = {  # name -> (environment variable, locator)
    "godot": ("GODOT", _godot),
    "bash": ("BASH_EXE", _bash),
    "mingw": ("MINGW", _mingw),
    "libclang": ("LIBCLANG", _libclang),
    "roadcraft_lab": ("ROADCRAFT_LAB", _nowhere),  # the reference game's tree, read for behaviour only (LAB_COPY's store)
}


def locate(name: str) -> str:
    env, finder = TOOLS[name]
    value = os.environ.get(env, "") or _local(env) or finder()
    if not value:
        raise Missing(f"{name}: not found on the PATH; set {env} in the environment or in {LOCAL} as {{\"{env}\": \"<path>\"}}")
    return value


if __name__ == "__main__":
    if len(sys.argv) != 2 or sys.argv[1] not in TOOLS:
        print(f"usage: locations.py <{'|'.join(TOOLS)}>", file=sys.stderr)
        sys.exit(2)
    try:
        print(locate(sys.argv[1]))
    except Missing as e:
        print(e, file=sys.stderr)
        sys.exit(1)
