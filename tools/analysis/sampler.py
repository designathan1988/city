# Generic stack sampler (Windows x64): any process, any compiled language, no name of the program. On every interval it freezes
# each thread of the process, walks its stack (dbghelp StackWalk64 over each module's own unwind tables: unwind.py), lets it go and sums: where
# each thread spends time and where it waits (told by the thread's own CPU time, not by names), the hot functions and chains,
# and the code that triggered engine or driver time (the first frame with symbols). Symbols by llvm-symbolizer for DWARF
# (clang/mingw) builds; a module without debug information is named so it can be rebuilt with it. The timeline in windows shows
# the start-up (the first windows) and the worst stretches without anything said about the program.
# A program it starts (--cmd) runs through tools/tasks/proc: in a job of its own (--hidden: on the private desktop), with a
# deadline of the sampling time plus START_MARGIN_S, and its whole tree is killed when the sampling ends, whatever happened.
# Symbols are kept by (module content hash, address) in the tools/tasks store. Zero samples is a failure (exit 1).
# Use: python tools/analysis/sampler.py (--cmd "line" [--hidden] | --pid N) [--seconds S] [--interval-ms M] [--window-s W]
#      [--symbolizer llvm-symbolizer.exe] [--json out.json]
import argparse
import ctypes as C
import ctypes.wintypes as W
import hashlib
import json
import os
import shutil
import struct
import subprocess
import sys
import time
from collections import defaultdict
from pathlib import Path

from pe_debug import database_missing
from sampler_report import report
from unwind import UnwindTables

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from tasks import proc  # noqa: E402
from tasks.cache import store_path, write_atomic  # noqa: E402

k32 = C.WinDLL("kernel32", use_last_error=True)
dbg = C.WinDLL("dbghelp", use_last_error=True)
psapi = C.WinDLL("psapi", use_last_error=True)
k32.GetCurrentThread.restype = C.c_void_p  # a pseudo-handle (-2): 64 bits, never an int
k32.QueryThreadCycleTime.argtypes = [C.c_void_p, C.POINTER(C.c_uint64)]
PROCESS_ALL = 0x1F0FFF
THREAD_ACCESS = 0x0002 | 0x0008 | 0x0010 | 0x0040  # suspend/resume, get context, query information
CONTEXT_FULL = 0x10000B
IMAGE_FILE_MACHINE_AMD64 = 0x8664
TH32CS_SNAPTHREAD = 0x4
MAX_FRAMES = 96
STACK_COPY = 256 * 1024  # bytes of the live stack copied while a thread is frozen (deeper frames are read live)
STILL_ACTIVE = 259
CALIBRATE_S = 0.1  # spin that long to learn the cycle counter's rate
REFRESH_S = 0.5  # threads and modules listed again this often
START_MARGIN_S = 120  # past the sampling time, the started program's deadline: start-up and the walk's own lag
SYMBOLIZE_TIMEOUT_S = 600  # one llvm-symbolizer call over one module's addresses
TOOL = "sampler-symbols"


class ContextX64(C.Structure):  # CONTEXT (winnt.h, x64): the fields StackWalk64 reads; the rest is room
    _fields_ = [("P", C.c_uint64 * 6), ("ContextFlags", W.DWORD), ("MxCsr", W.DWORD), ("Seg", W.WORD * 6), ("EFlags", W.DWORD),
                ("Dr", C.c_uint64 * 6), ("Rax", C.c_uint64), ("Rcx", C.c_uint64), ("Rdx", C.c_uint64), ("Rbx", C.c_uint64),
                ("Rsp", C.c_uint64), ("Rbp", C.c_uint64), ("Rsi", C.c_uint64), ("Rdi", C.c_uint64), ("R", C.c_uint64 * 8),
                ("Rip", C.c_uint64), ("Rest", C.c_byte * 976)]


class Address(C.Structure):
    _fields_ = [("Offset", C.c_uint64), ("Segment", W.WORD), ("Mode", C.c_int)]


class Frame(C.Structure):  # STACKFRAME64
    _fields_ = [("AddrPC", Address), ("AddrReturn", Address), ("AddrFrame", Address), ("AddrStack", Address),
                ("AddrBStore", Address), ("FuncTableEntry", C.c_void_p), ("Params", C.c_uint64 * 4), ("Far", W.BOOL),
                ("Virtual", W.BOOL), ("Reserved", C.c_uint64 * 3), ("KdHelp", C.c_byte * 112)]


class Thread32(C.Structure):
    _fields_ = [("dwSize", W.DWORD), ("cntUsage", W.DWORD), ("th32ThreadID", W.DWORD), ("th32OwnerProcessID", W.DWORD),
                ("tpBasePri", C.c_long), ("tpDeltaPri", C.c_long), ("dwFlags", W.DWORD)]


class ModuleInfo(C.Structure):
    _fields_ = [("base", C.c_void_p), ("size", W.DWORD), ("entry", C.c_void_p)]


def threads_of(pid):
    snap = k32.CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0)
    te, out = Thread32(), []
    te.dwSize = C.sizeof(Thread32)
    ok = k32.Thread32First(snap, C.byref(te))
    while ok:
        if te.th32OwnerProcessID == pid:
            out.append(te.th32ThreadID)
        ok = k32.Thread32Next(snap, C.byref(te))
    k32.CloseHandle(snap)
    return out


def modules_of(hp):
    handles = (C.c_void_p * 2048)()
    needed = W.DWORD()
    psapi.EnumProcessModulesEx(hp, handles, C.sizeof(handles), C.byref(needed), 3)  # LIST_MODULES_ALL
    out, path, info = [], C.create_unicode_buffer(1024), ModuleInfo()
    for h in handles[:needed.value // C.sizeof(C.c_void_p)]:
        if h and psapi.GetModuleInformation(hp, C.c_void_p(h), C.byref(info), C.sizeof(info)):
            psapi.GetModuleFileNameExW(hp, C.c_void_p(h), path, 1024)
            out.append((info.base, info.base + info.size, path.value))
    return out


def cycles_per_ns():
    """The cycle counter's rate, measured on this thread spinning for CALIBRATE_S: QueryThreadCycleTime counts every cycle
    a thread runs, so the CPU time of a thread between two samples is exact."""
    me, c0, c1 = k32.GetCurrentThread(), C.c_uint64(), C.c_uint64()
    k32.QueryThreadCycleTime(me, C.byref(c0))
    t0 = time.perf_counter()
    while time.perf_counter() - t0 < CALIBRATE_S:
        pass
    k32.QueryThreadCycleTime(me, C.byref(c1))
    return (c1.value - c0.value) / ((time.perf_counter() - t0) * 1e9)


class Sampler:
    def __init__(self, pid):
        self.pid = pid
        self.hp = k32.OpenProcess(PROCESS_ALL, False, pid)
        dbg.SymSetOptions(0x2 | 0x10)  # UNDNAME | DEFERRED_LOADS
        dbg.SymInitializeW(self.hp, None, True)
        self.opened = {}
        self.cycles_per_ns = cycles_per_ns()
        self.last = {}  # tid -> (cycles, stack, cpu) of its last walk: a thread that has not run since has the same stack
        self.main = None  # the first thread seen: the program's main loop
        self.modules = {}  # base -> (end, path), gathered while sampling: the process may be gone at the end
        self.unwind = UnwindTables(self.hp, self.modules)  # every module's own unwind tables, symbols or not

    def stack(self, tid):
        """(the thread's stack, its CPU ns so far)."""
        h = self.opened.get(tid)
        if h is None:
            h = self.opened[tid] = k32.OpenThread(THREAD_ACCESS, False, tid)
        if not h:
            return None, 0
        cycles = C.c_uint64()
        k32.QueryThreadCycleTime(h, C.byref(cycles))
        cpu = cycles.value / self.cycles_per_ns  # exact: the thread times of the OS move in clock ticks (~15.6 ms)
        seen = self.last.get(tid)
        if seen and seen[0] == cycles.value:
            return seen[1], seen[2]  # not one cycle run since: same stack, same CPU time, no walk
        if k32.SuspendThread(h) == 0xFFFFFFFF:
            return None, 0
        buf = (C.c_byte * (C.sizeof(ContextX64) + 16))()
        aligned = (C.addressof(buf) + 15) & ~15  # CONTEXT wants 16-byte alignment
        ctx = ContextX64.from_address(aligned)
        ctx.ContextFlags = CONTEXT_FULL
        try:  # frozen only for the context and a copy of the live stack: the walk runs after the thread goes on
            if not k32.GetThreadContext(h, C.c_void_p(aligned)):
                return None, cpu
            self.unwind.copy_stack(ctx.Rsp, STACK_COPY)
        finally:
            k32.ResumeThread(h)
        f = Frame()
        f.AddrPC.Offset, f.AddrFrame.Offset, f.AddrStack.Offset = ctx.Rip, ctx.Rbp, ctx.Rsp
        f.AddrPC.Mode = f.AddrFrame.Mode = f.AddrStack.Mode = 3  # AddrModeFlat
        out = []
        while len(out) < MAX_FRAMES and dbg.StackWalk64(IMAGE_FILE_MACHINE_AMD64, self.hp, h, C.byref(f), C.c_void_p(aligned),
                                                        self.unwind.read_memory, self.unwind.table_access, self.unwind.module_base, None):
            if not f.AddrPC.Offset:
                break
            out.append(f.AddrPC.Offset)
        self.last[tid] = (cycles.value, tuple(out), cpu)
        return tuple(out), cpu

    def run(self, seconds, interval):
        samples = []  # (t, tid, stack, CPU ns since the thread's previous sample, wall ns since it)
        before, t0, listed, ids, code, turn = {}, time.perf_counter(), 0.0, [], W.DWORD(), 0
        while time.perf_counter() - t0 < seconds:
            now = time.perf_counter() - t0
            if now - listed > REFRESH_S or not ids:
                ids, listed = threads_of(self.pid), now
                dbg.SymRefreshModuleList(self.hp)  # DLLs loaded after the start: without them the walk is lost in them
                for b, end, path in modules_of(self.hp):
                    self.modules[b] = (end, path)
                if ids and self.main is None:
                    self.main = ids[0]
            if not ids or (k32.GetExitCodeProcess(self.hp, C.byref(code)) and code.value != STILL_ACTIVE):
                break
            # the main thread on every sweep; the others take turns while the sweep is within its interval, so a busy pool
            # never starves the main thread of samples (each skipped one is walked first on the next sweep)
            sweep = time.perf_counter()
            others = [x for x in ids if x != self.main]
            turn %= max(1, len(others))
            order = ([self.main] if self.main in ids else []) + others[turn:] + others[:turn]
            for n, tid in enumerate(order):
                if n > 1 and time.perf_counter() - sweep > interval:  # the main thread and at least one other, however late
                    turn += n - 1
                    break
                stack, cpu = self.stack(tid)
                prev = before.get(tid)
                before[tid] = (cpu, now)
                if stack and prev:
                    samples.append((now, tid, stack, cpu - prev[0], (now - prev[1]) * 1e9))
            time.sleep(max(0.0, interval - (time.perf_counter() - sweep)))  # one sweep per interval, the walk included
        return samples


def module_hash(path: str) -> str:
    h = hashlib.sha256()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def symbolize(path: str, offsets: list[int], symbolizer: str) -> dict[int, str | None]:
    """offset from the image base -> 'function (file)' for the offsets of one module; None where there is no file and line.
    Kept by (module content hash, offset): a module already read is not read again."""
    table_path = store_path(TOOL, f"{module_hash(path)}.json")
    table = json.loads(table_path.read_text(encoding="utf-8")) if table_path.is_file() else {}
    ask = sorted({o for o in offsets if str(o) not in table})
    if ask and database_missing(path):  # its debug information is nowhere here: named by offset, never asked for
        table.update({str(o): None for o in ask})
        write_atomic(table_path, json.dumps(table))
    elif ask:
        with open(path, "rb") as f:
            head = f.read(4096)
        pe = struct.unpack_from("<I", head, 0x3C)[0]
        image_base = struct.unpack_from("<Q", head, pe + 24 + 24)[0]
        r = subprocess.run([symbolizer, f"--obj={path}", "--demangle", "--no-inlines"], timeout=SYMBOLIZE_TIMEOUT_S,
                           creationflags=proc.NO_WINDOW,
                           input="\n".join(f"0x{image_base + o:x}" for o in ask), capture_output=True, text=True)
        if r.returncode != 0:
            raise RuntimeError(f"{symbolizer} failed on {path} ({r.returncode}): {r.stderr.strip()[:300]}")
        blocks = [b.splitlines() for b in r.stdout.strip().split("\n\n")]
        if len(blocks) != len(ask):
            raise RuntimeError(f"{symbolizer} answered {len(blocks)} of {len(ask)} addresses of {path}")
        for o, block in zip(ask, blocks):
            # with file and line only: without debug info a symbolizer gives the nearest export, which misleads
            trusted = len(block) > 1 and block[0] not in ("??", "") and not block[1].startswith("??")
            table[str(o)] = f"{block[0]} ({os.path.basename(block[1])})" if trusted else None
        write_atomic(table_path, json.dumps(table))
    return {o: table[str(o)] for o in offsets}


def symbols(sampler, addresses, symbolizer):
    """address -> 'module!function (file)': only with file and line (debug info); else module+offset, never a guess."""
    names, by_module, undebugged = {}, defaultdict(list), set()
    modules = [(b, end, path) for b, (end, path) in sampler.modules.items()]
    for a in addresses:
        hit = next(((b, path) for b, end, path in modules if b <= a < end), None)
        if hit is None:
            names[a] = f"?!0x{a:x}"  # outside every module: a lost frame (generated code, or no unwind data)
            continue
        by_module[hit].append(a)
        names[a] = f"{os.path.basename(hit[1])}!+0x{a - hit[0]:x}"
    windows = os.environ.get("WINDIR", "C:\\Windows").lower()
    for (base, path), items in by_module.items():
        if not path or not symbolizer or not os.path.exists(path):
            continue
        found = symbolize(path, [a - base for a in items], symbolizer)
        trusted = 0
        for a in items:
            if found[a - base]:
                names[a] = f"{os.path.basename(path)}!{found[a - base]}"
                trusted += 1
        if not trusted and not path.lower().startswith(windows):
            undebugged.add(os.path.basename(path))
    return names, undebugged


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cmd")
    ap.add_argument("--pid", type=int)
    ap.add_argument("--hidden", action="store_true")
    ap.add_argument("--seconds", type=float, default=30)
    ap.add_argument("--interval-ms", type=float, default=5)
    ap.add_argument("--window-s", type=float, default=2)
    ap.add_argument("--symbolizer", default=shutil.which("llvm-symbolizer") or "")
    ap.add_argument("--json")
    a = ap.parse_args()
    if bool(a.cmd) == bool(a.pid):
        ap.error("one of --cmd or --pid")
    target = None
    if a.cmd:  # ours: started in a job, killed with its tree at the end and at its deadline
        target = proc.Process(a.cmd, hidden=a.hidden, deadline_s=a.seconds + START_MARGIN_S)
    try:
        s = Sampler(target.pid if target else a.pid)
        samples = s.run(a.seconds, a.interval_ms / 1000.0)
    finally:
        if target:
            target.close()
    if not samples:
        print("FAIL zero samples: the process gave no stack to walk (gone, or not reachable)")
        return 1
    names, undebugged = symbols(s, {e for _, _, st, _, _ in samples for e in st}, a.symbolizer)
    if undebugged:
        print("WARNING: no debug information (build with it to see functions and lines): " + ", ".join(sorted(undebugged)))
    data = report(samples, names, s.main, a.window_s, a.interval_ms)
    if a.json:
        with open(a.json, "w", encoding="utf-8") as f:
            json.dump(data, f, ensure_ascii=False, indent=1)
    return 0


if __name__ == "__main__":
    sys.exit(main())
