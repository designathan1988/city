"""What each language's compiler says a call runs: the compiler resolves names, overloads, templates and the types
of what is called on as the language itself does, where reading the text alone can only guess. C++ is read by
clang (its library, in this process), TypeScript and JavaScript by the TypeScript compiler (run by node); a language
with no compiler here (GDScript) is the flow's alone.

Each answer is for one call, by its place (the file, the bytes it spans), and is one of:
- exact: the definitions it runs, and nothing else (a function, an overload chosen, a template's function, a method
  that is not virtual, one called through its class or its base by name);
- dispatched: a method run by the class of the object called on: the one declared or one of the same name below its
  class (a virtual method in C++, a method in TypeScript);
- outside: a function of the language's library or of code not read (no definition here runs).
A call through a value holding a function (a pointer, a variable, a parameter, a member, a lambda, a call's result)
has no answer: what it holds is the flow's to follow, as is any call a compiler does not resolve, or one written in a
file it was not given. Where a compiler is not installed, said once, and the flow answers alone.
"""
from __future__ import annotations

import hashlib
import json
import math
import os
import re
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path

from .configs import norm as unit_name
from .languages import (CLANG, DISPATCHED_CALL, NODE, OUTSIDE_CALL, PATH_KEY, PATH_VARIABLE, SEARCH_ENDS,
                        SEARCH_STARTS, SYSTEM_PATHS, TOOLCHAINS, TOOLS, TS_SCRIPT, TSC)
from .vocab import FUNCTION, SHADER

STANDARD = "-std=c++17"                  # what clang reads C++ as, where a compilation database does not say
SECONDS = 600                            # the longest a compiler may take, where no ceiling says less
BOM = b"\xef\xbb\xbf"
ASKED_SECONDS = 30                       # the longest the system's compiler may take to say where its headers are


@dataclass(frozen=True, slots=True)
class Answer:
    targets: tuple                       # the definitions it runs (none: a function of code not read)
    dispatched: bool = False             # a method run by the object's class: one of its name below may run instead


def answers(program, ceiling=None, say=lambda words: None) -> dict:
    """{(file, start byte, end byte) of a call: its Answer}, from every compiler there is for the files read."""
    out = {}
    by = {}
    for f in program.files:
        if f.path is not None and f.lang.compiler in COMPILERS and f.lang.prepare != SHADER:
            by.setdefault(f.lang.compiler, []).append(f)
    places = Places(program)
    for name, files in by.items():
        began = time.monotonic()
        try:
            found, problem = COMPILERS[name](program, files, places, ceiling)
        except Exception as error:       # a compiler that fails answers nothing: the flow answers alone (said)
            found, problem = {}, f"failed ({type(error).__name__}: {error})"
        out.update(found)
        say(f"{name}: {len(found)} calls answered in {time.monotonic() - began:.1f} s" +
            (f" ({problem})" if problem else ""))
        if problem:
            program.configs.notes.append(f"{name}: {problem}")
    return out


class Places:
    """The definition written at a place of a file read: the innermost function holding it (of that name, where any
    is)."""
    def __init__(self, program):
        self.p = program
        self.by_file = {}
        self.raw = {}
        self.index = {}
        self.known = {}                  # a path as written -> its file (or None), worked out once
        for f in program.files:
            if f.path is not None:
                self.index[norm(f.path)] = f.index

    def file(self, path) -> int | None:
        path = str(path)
        if path not in self.known:
            self.known[path] = self.index.get(norm(path))
        return self.known[path]

    def bytes_of(self, fi: int) -> bytes:
        if fi not in self.raw:
            self.raw[fi] = Path(self.p.files[fi].path).read_bytes()
        return self.raw[fi]

    def defined(self, fi: int, offset: int, name: str) -> int | None:
        if fi not in self.by_file:
            f = self.p.files[fi]
            self.by_file[fi] = [(d.node.start_byte, d.node.end_byte, d.id) for d in self.p.defs[f.span.start:f.span.stop]
                                if d.kind == FUNCTION and d.file == fi]
        holding = [(end - start, d) for start, end, d in self.by_file[fi] if start <= offset < end]
        named = [one for one in holding if self.p.defs[one[1]].name == name]
        best = min(named or holding, default=None)
        return best[1] if best is not None else None

    def offset(self, fi: int, line: int, column16: int) -> int:
        """The byte a line and a column counted in UTF-16 units (as the TypeScript compiler counts) stand for."""
        raw = self.bytes_of(fi)
        lines = self.lines(fi)
        if line >= len(lines):
            return len(raw)
        start = lines[line]
        if line == 0 and raw.startswith(BOM):
            start += len(BOM)
        end = raw.find(b"\n", start)
        text = raw[start:len(raw) if end < 0 else end]
        if text.isascii():
            return start + column16
        units = text.decode("utf-8", errors="replace").encode("utf-16-le")[:2 * column16]
        return start + len(units.decode("utf-16-le", errors="replace").encode("utf-8"))

    def lines(self, fi: int) -> list:
        key = (fi, None)
        if key not in self.raw:
            raw, starts, at = self.bytes_of(fi), [0], -1
            while True:
                at = raw.find(b"\n", at + 1)
                if at < 0:
                    break
                starts.append(at + 1)
            self.raw[key] = starts
        return self.raw[key]


def norm(path) -> str:
    return os.path.normcase(os.path.abspath(str(path)))


def over(ceiling) -> bool:
    return ceiling is not None and ceiling.over()


def left(ceiling) -> float:
    """The seconds a compiler may still take."""
    if ceiling is None or math.isinf(ceiling.until):
        return SECONDS
    return max(1.0, min(SECONDS, ceiling.until - time.monotonic()))


# ---- the system's programs ----------------------------------------------------------------------------------------
def found_program(name: str) -> str | None:
    """A program on the path this process was given, else on the one the system gives a new process now (one
    installed after this process started)."""
    hit = shutil.which(name)
    if hit or not sys.platform.startswith("win"):
        return hit
    try:
        import winreg
    except ImportError:
        return None
    folders = []
    for hive, key in SYSTEM_PATHS:
        try:
            with winreg.OpenKey(getattr(winreg, hive), key) as opened:
                folders += os.path.expandvars(winreg.QueryValueEx(opened, PATH_KEY)[0]).split(os.pathsep)
        except OSError:
            continue
    return shutil.which(name, path=os.pathsep.join(f for f in folders if f)) if folders else None


_TOOLCHAIN = []


def toolchain() -> list:
    """What clang reads a C++ file with to find the standard library: the headers' folders of the system's C++
    compiler and what that compiler compiles for; none where there is no such compiler."""
    if _TOOLCHAIN:
        return _TOOLCHAIN[0]
    flags = []
    for name in TOOLCHAINS:
        exe = found_program(name)
        if exe is None:
            continue
        try:
            asked = subprocess.run([exe, "-E", "-x", "c++", "-", "-v"], input="", capture_output=True, text=True,
                                   timeout=ASKED_SECONDS)
            machine = subprocess.run([exe, "-dumpmachine"], capture_output=True, text=True, timeout=ASKED_SECONDS)
        except (OSError, subprocess.SubprocessError):
            continue
        lines = asked.stderr.splitlines()
        if SEARCH_STARTS not in lines:
            continue
        folders = []
        for line in lines[lines.index(SEARCH_STARTS) + 1:]:
            if line.strip() == SEARCH_ENDS:
                break
            folders.append(str(Path(line.strip()).resolve()))
        if machine.stdout.strip():
            flags.append(f"--target={machine.stdout.strip()}")
        # Where that compiler is installed (the folder holding its programs' folder): told so, clang does not look
        # for it on the disk at every unit (a GNU target's search: ~50 ms each, most of a unit's cost).
        flags.append(f"--sysroot={Path(exe).resolve().parent.parent}")
        for folder in folders:
            flags += ["-isystem", folder]
        break
    _TOOLCHAIN.append(flags)
    return flags


# ---- C++: clang ----------------------------------------------------------------------------------------------------
UNITS = "compiled-units"                 # clang's answers kept by unit, beside the saved analyses (`index`)
MOST_UNITS = 50000                       # the most units kept (the least recently read go)
LAST_READ = []                           # the units clang read again in the last analysis (the others were kept)


def cpp(program, files: list, places: Places, ceiling) -> tuple:
    """clang's answers, unit by unit (a file and what it includes). A unit read before whose files (its own and the
    ones of the program it includes) are as they were, with the same flags, is not read again: its answers are kept
    (`UNITS`), so a change to one file reads again only the units holding it."""
    try:
        from clang import cindex
        index = cindex.Index.create(excludeDecls=True)   # what the precompiled headers declare is not walked
    except Exception as error:
        return {}, f"not installed ({error})"
    from . import index as saved
    K = cindex.CursorKind
    kinds = clang_kinds(K)
    common = [STANDARD] + toolchain()
    included = {Path(one).name for f in files for one in f.includes}
    written = {}                                 # a file's name -> how includes write it (`sub/x.h`)
    for f in files:
        for one in f.includes:
            written.setdefault(Path(one).name, set()).add(Path(one))
    folders = []                                 # the folders an include written so finds the file from
    for f in files:
        here = Path(f.path)
        for one in written.get(here.name, ()):
            depth = len(one.parts)
            if depth <= len(here.parents) and Path(*here.parts[-depth:]) == one:
                folders.append(str(here.parents[depth - 1]))
    folders = list(dict.fromkeys(folders))
    system = [common[i + 1] for i, word in enumerate(common) if word == "-isystem"]
    order = sorted(files, key=lambda f: (Path(f.path).name in included, f.index))
    kept = load_units(saved)
    LAST_READ.clear()
    seen_files, calls, by_usr, skipped, read_again = set(), {}, {}, 0, 0
    operators = files[0].lang.operators
    firsts = {}                                  # (a file's first standard headers, flags) -> the flags reading them

    def merge(unit) -> None:
        _, deps, unit_calls, unit_defs = unit
        for path in deps:
            fi = places.file(path)
            if fi is not None:
                seen_files.add(fi)
        for (path, s, e), got in unit_calls.items():
            fi = places.file(path)
            if fi is None:
                continue
            key = (fi, s, e)
            if got != THROUGH and got is not None:
                (usr, place), dispatched = got[0][0], got[1]
                got = (((usr, (places.file(place[0]),) + tuple(place[1:])
                         if place and places.file(place[0]) is not None else None),), dispatched)
            if key not in calls:
                calls[key] = got
            elif calls[key] != got:
                calls[key] = None
        for usr, (path, offset, spelled) in unit_defs.items():
            fi = places.file(path)
            if fi is not None:
                by_usr.setdefault(usr, (fi, offset, spelled))

    sources = [f for f in order if Path(f.path).name not in included]
    headers = [f for f in order if Path(f.path).name in included]
    for group in (sources, headers):             # a header is read alone only where no unit read it
        jobs = []
        for f in group:
            if f.index in seen_files:
                continue
            database = program.configs.of(f.lang.project, f.path)
            units = database.includes.get(unit_name(f.path)) or database.every_include
            flags = ["-x", "c++"] + common + [f"-I{folder}" for folder in map(str, units)]
            for folder in folders:       # where a file of the program is looked for (`"..."` alone: no `<...>`
                flags += ["-iquote", folder]  # of the standard library looks there)
            name = norm(f.path)
            unit = kept.get(name)
            if unit is not None and unit[0] == flags and all(stamp(path) == was for path, was in unit[1].items()):
                merge(unit)
                continue
            key = (preamble(f.path), tuple(flags))
            if key not in firsts:
                firsts[key] = prefix(index, cindex, f.path, flags, system, saved)
            jobs.append([name, str(f.path), flags, firsts[key]])
        got, left_out = read_units(jobs, sorted(places.index), operators, ceiling, index, cindex, kinds, K)
        for name, unit in got:
            if unit is None:
                continue
            kept.pop(name, None)                 # the newest last: the oldest are the ones let go
            kept[name] = unit
            read_again += 1
            LAST_READ.append(name)
            merge(unit)
        for name in left_out:
            ceiling.skip(f"the compiler's answers for {name}")
        skipped += len(left_out)
    if read_again:
        save_units(saved, kept)
    out = {}
    for key, got in calls.items():
        if got is None or got == THROUGH:
            continue
        refs, dispatched = got
        targets = []
        for usr, place in refs:
            place = by_usr.get(usr) or place
            if place is None:
                continue             # declared in code not read, defined in none read: of code not read
            d = places.defined(*place)
            if d is None:
                targets = None
                break
            targets.append(d)
        if targets is not None:
            out[key] = Answer(tuple(dict.fromkeys(targets)), dispatched and bool(targets))
    return out, (f"{skipped} files left at the ceiling" if skipped else "")


def stamp(path: str):
    """What says a file changed: its size and the time it was written (None: gone)."""
    try:
        st = os.stat(path)
        return st.st_size, st.st_mtime_ns
    except OSError:
        return None


def load_units(saved) -> dict:
    import pickle
    path = saved.folder() / UNITS
    try:
        return pickle.loads(path.read_bytes()) if path.exists() else {}
    except Exception:            # a file written by another version, or cut: read again
        return {}


def save_units(saved, kept: dict) -> None:
    import pickle
    try:
        (saved.folder() / UNITS).write_bytes(pickle.dumps(dict(list(kept.items())[-MOST_UNITS:]),
                                                          protocol=pickle.HIGHEST_PROTOCOL))
    except OSError:
        pass


PCH = "compiled-headers"                 # the standard library's headers read once, beside the saved analyses
MOST_PCH = 64                            # the most kept (the oldest go)
FATAL = 4                                # clang's severity of an error that stops it


def preamble(path) -> tuple:
    """The standard library's headers a file includes first, in order, before anything else it writes (nothing
    between them can change what they declare): what may be read once for every file starting the same way."""
    names = []
    try:
        text = Path(path).read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ()
    for line in text.splitlines():
        s = line.strip()
        if not s or s.startswith("//") or s == "#pragma once":
            continue
        m = re.match(r"#\s*include\s*<([^>]+)>\s*(//.*)?$", s)
        if not m:
            break
        names.append(m.group(1))
    return tuple(names)


def prefix(index, cindex, path, flags: list, system: list, saved) -> list:
    """The flags reading a file with its first standard headers already read (`-include-pch`), made once for each
    such start and flags; none where the file starts otherwise, or a header is not the standard library's."""
    names = []
    for name in preamble(path):
        if not any((Path(folder) / name).is_file() for folder in system):
            break                                # a header of the program (or not found): read with the file
        names.append(name)
    if not names:
        return []
    key = hashlib.sha256(repr((names, flags)).encode()).hexdigest()[:24]
    folder = saved.folder() / PCH
    made = folder / f"{key}.pch"
    if not made.exists():
        try:
            folder.mkdir(parents=True, exist_ok=True)
            head = folder / f"{key}.hpp"
            head.write_text("".join(f"#include <{name}>\n" for name in names), encoding="utf-8")
            tu = index.parse(str(head), args=["-x", "c++-header"] + flags[2:],
                             options=cindex.TranslationUnit.PARSE_INCOMPLETE | FOR_SERIALIZATION)
            if any(d.severity >= FATAL for d in tu.diagnostics):
                return []
            tu.save(str(made))
            for old in sorted(folder.glob("*.pch"), key=lambda p: p.stat().st_mtime, reverse=True)[MOST_PCH:]:
                old.unlink(missing_ok=True)
        except Exception:                        # not made: the file is read whole
            return []
    return ["-include-pch", str(made)]


FOR_SERIALIZATION = 16                   # libclang's flag for a unit to be saved (CXTranslationUnit_ForSerialization)


def read_unit(index, cindex, path: str, flags: list, first: list, ours, operators: str, kinds, K):
    """One unit read by clang: (flags, {file it holds: stamp}, {(file, start, end) of a call: what it runs},
    {a function's own name (USR): where it is defined}), files by their paths; None where clang cannot read it.
    `first`: its standard headers read before (`prefix`); where clang cannot use them, the unit is read whole.
    `ours(path)`: whether a file is one of the program read."""
    try:
        tu = index.parse(path, args=flags + list(first), options=cindex.TranslationUnit.PARSE_INCOMPLETE)
        if first and any(d.severity >= FATAL for d in tu.diagnostics):
            tu = index.parse(path, args=flags, options=cindex.TranslationUnit.PARSE_INCOMPLETE)
    except cindex.TranslationUnitLoadError:
        return None
    deps = {norm(path): stamp(norm(path))}
    for inc in tu.get_includes():
        held = norm(inc.include.name)
        if ours(held):
            deps[held] = stamp(held)
    unit_calls, unit_defs = {}, {}
    stack = list(tu.cursor.get_children())
    while stack:
        cursor = stack.pop()
        where = cursor.location.file
        if where is None or not ours(where.name):
            continue                     # code of a file not read (the standard library's): not walked
        kind = cursor.kind
        if kind in kinds[2] and cursor.is_definition():
            unit_defs.setdefault(cursor.get_usr(), (norm(where.name), cursor.location.offset, cursor.spelling))
        if kind == K.CALL_EXPR:
            answered(cursor, unit_calls, ours, operators, kinds, K)
        stack.extend(cursor.get_children())
    del tu
    return flags, deps, unit_calls, unit_defs


WORKERS = 2                              # the processes reading units at once (no more: the machine is shared)
PARALLEL_FROM = 24                       # fewer units to read than this: read here, no process started
WORKER_SECONDS = 30                      # the longest a worker may take to start


def read_here(jobs: list, ours_paths: list, operators: str, ceiling, index, cindex, kinds, K) -> tuple:
    """The units read in this process, one by one: ([(name, unit or None)], the names left at the ceiling)."""
    known, cache = set(ours_paths), {}

    def ours(path) -> bool:
        if path not in cache:
            cache[path] = norm(path) in known
        return cache[path]
    out, stopped = [], []
    for name, path, flags, first in jobs:
        if over(ceiling):
            stopped.append(name)
            continue
        out.append((name, read_unit(index, cindex, path, flags, first, ours, operators, kinds, K)))
    return out, stopped


def read_units(jobs: list, ours_paths: list, operators: str, ceiling, index, cindex, kinds, K) -> tuple:
    """The units of `jobs` ([name, path, flags, first]) read: ([(name, unit or None)], the names left at the
    ceiling). Many: by worker processes (`serve`), each told the program's files once, given one unit at a time, so
    clang's memory is theirs, and they end when the work does; few, or what no worker took (none could start): here."""
    if len(jobs) < PARALLEL_FROM:
        return read_here(jobs, ours_paths, operators, ceiling, index, cindex, kinds, K)
    import queue
    import threading
    todo = queue.Queue()
    for job in jobs:
        todo.put(job)
    out, lock = [], threading.Lock()
    head = json.dumps([ours_paths, operators]) + "\n"
    here = str(Path(__file__).resolve().parent.parent)

    def work() -> None:
        try:
            proc = subprocess.Popen([sys.executable, "-m", __name__], cwd=here, stdin=subprocess.PIPE,
                                    stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True, encoding="utf-8")
        except OSError:
            return
        timer = threading.Timer(left(ceiling) + WORKER_SECONDS, proc.kill)   # past the ceiling: stopped
        timer.start()
        try:
            proc.stdin.write(head)
            proc.stdin.flush()
            while not over(ceiling):
                try:
                    job = todo.get_nowait()
                except queue.Empty:
                    break
                proc.stdin.write(json.dumps(job) + "\n")
                proc.stdin.flush()
                line = proc.stdout.readline()
                if not line:             # the worker ended (clang failed on that unit): it is not answered
                    with lock:
                        out.append((job[0], None))
                    break
                name, unit = json.loads(line)
                with lock:
                    out.append((name, decoded(unit)))
        except (OSError, ValueError):
            pass
        finally:
            timer.cancel()
            try:
                proc.stdin.close()
                proc.wait(timeout=WORKER_SECONDS)
            except (OSError, subprocess.SubprocessError):
                proc.kill()
    threads = [threading.Thread(target=work) for _ in range(WORKERS)]
    for one in threads:
        one.start()
    for one in threads:
        one.join()
    rest = []
    while not todo.empty():
        rest.append(todo.get_nowait())
    more, stopped = read_here(rest, ours_paths, operators, ceiling, index, cindex, kinds, K)
    return out + more, stopped


def encoded(unit):
    """A unit as JSON can carry it."""
    if unit is None:
        return None
    flags, deps, unit_calls, unit_defs = unit
    return [flags, [[p, list(s) if s else None] for p, s in deps.items()],
            [[p, s, e, None if got is None else [] if got == THROUGH else [[got[0][0][0], list(got[0][0][1])
                                                                            if got[0][0][1] else None], got[1]]]
             for (p, s, e), got in unit_calls.items()],
            [[usr, p, o, sp] for usr, (p, o, sp) in unit_defs.items()]]


def decoded(unit):
    """A unit back from JSON: the tuples it is kept as."""
    if unit is None:
        return None
    flags, deps, unit_calls, unit_defs = unit
    calls_back = {}
    for p, s, e, got in unit_calls:
        if got is None:
            calls_back[(p, s, e)] = None
        elif not got:
            calls_back[(p, s, e)] = THROUGH
        else:
            (usr, place), dispatched = got
            calls_back[(p, s, e)] = (((usr, tuple(place) if place else None),), dispatched)
    return (flags, {p: tuple(s) if s else None for p, s in deps}, calls_back,
            {usr: (p, o, sp) for usr, p, o, sp in unit_defs})


def serve() -> None:
    """A worker of `read_units`: told the program's files and the operators' word, then given units one per line,
    it answers each on a line."""
    from clang import cindex
    index = cindex.Index.create(excludeDecls=True)
    K = cindex.CursorKind
    kinds = clang_kinds(K)
    ours_paths, operators = json.loads(sys.stdin.readline())
    known, cache = set(ours_paths), {}

    def ours(path) -> bool:
        if path not in cache:
            cache[path] = norm(path) in known
        return cache[path]
    for line in sys.stdin:
        name, path, flags, first = json.loads(line)
        try:
            unit = read_unit(index, cindex, path, flags, first, ours, operators, kinds, K)
        except Exception:                # a unit clang fails on: not answered
            unit = None
        sys.stdout.write(json.dumps([name, encoded(unit)]) + "\n")
        sys.stdout.flush()


def clang_kinds(K) -> tuple:
    """The functions, what makes, ends and converts objects, and both."""
    kinds = ((K.FUNCTION_DECL, K.CXX_METHOD, K.FUNCTION_TEMPLATE), (K.CONSTRUCTOR, K.DESTRUCTOR, K.CONVERSION_FUNCTION))
    return kinds + (kinds[0] + kinds[1],)


THROUGH = ()                             # a call through a value holding a function, or one not resolved


def answered(cursor, calls: dict, ours, operators: str, kinds, K) -> None:
    """One call as clang resolved it, kept by the bytes it spans (two calls spanning the same bytes, written in one
    macro: neither answered). What makes, ends or converts an object, and an operator, are the flow's: not kept."""
    ext = cursor.extent
    if ext.start.file is None or not ours(ext.start.file.name):
        return
    key = (norm(ext.start.file.name), ext.start.offset, ext.end.offset)
    ref = cursor.referenced
    if ref is not None and (ref.kind in kinds[1] or ref.spelling.startswith(operators)):
        return
    got = THROUGH
    if ref is not None and ref.kind in kinds[0]:
        definition = ref.get_definition()
        target = definition if definition is not None else ref
        at = target.location.file
        place = (norm(at.name), target.location.offset, target.spelling) \
            if at is not None and ours(at.name) else None
        dispatched = ref.kind == K.CXX_METHOD and ref.is_virtual_method() and not qualified(cursor)
        got = (((ref.get_usr(), place),), dispatched)
    if key not in calls:
        calls[key] = got
    elif calls[key] != got:
        calls[key] = None


def qualified(call) -> bool:
    """A method called by its class's name (`Base::m()`, `o.Base::m()`): that one runs, not the object's own."""
    callee = next(iter(call.get_children()), None)
    if callee is None:
        return False
    words = [t.spelling for t in callee.get_tokens()]
    marks = [i for i, w in enumerate(words) if w in (".", "->")]
    after = words[marks[-1] + 1:] if marks else words
    return "::" in after


# ---- TypeScript and JavaScript: the TypeScript compiler -----------------------------------------------------------
def typescript(program, files: list, places: Places, ceiling) -> tuple:
    node = found_program(NODE)
    script = Path(__file__).resolve().parent / TOOLS / TS_SCRIPT
    if node is None:
        return {}, "node is not installed"
    asked = json.dumps([str(program.base or Path(files[0].path).parent), [str(Path(f.path).resolve()) for f in files]])
    try:
        done = subprocess.run([node, str(script)], input=asked, capture_output=True, text=True, encoding="utf-8",
                              timeout=left(ceiling))
    except subprocess.TimeoutExpired:
        return {}, "stopped at the ceiling"
    except OSError as error:
        return {}, str(error)
    if done.returncode != 0 or not done.stdout.strip():
        return {}, (done.stderr.strip().splitlines() or ["no answer"])[-1]
    calls, problem = json.loads(done.stdout)
    out = {}
    for at, line, column, end_line, end_column, how, targets in calls:
        fi = files[at].index
        key = (fi, places.offset(fi, line, column), places.offset(fi, end_line, end_column))
        if how == OUTSIDE_CALL:
            out[key] = Answer(())
            continue
        found = [places.defined(files[t].index, places.offset(files[t].index, tl, tc), name)
                 for t, tl, tc, name in targets]
        if not found or None in found:
            continue
        out[key] = Answer(tuple(dict.fromkeys(found)), how == DISPATCHED_CALL)
    return out, problem


COMPILERS = {CLANG: cpp, TSC: typescript}


if __name__ == "__main__":               # a worker reading units for `cpp` (`read_units`)
    serve()
