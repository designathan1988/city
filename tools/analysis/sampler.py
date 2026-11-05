# Generic stack sampler (Windows x64): any process, any compiled language, no name of the program. On every interval it freezes
# each thread of the process, walks its stack (dbghelp StackWalk64, the binary's own unwind tables), lets it go and sums: where
# each thread spends time and where it waits (told by the thread's own CPU time, not by names), the hot functions and chains,
# and the code that triggered engine or driver time (the first frame with symbols). Symbols by llvm-symbolizer for DWARF
# (clang/mingw) builds; a module without debug information is named so it can be rebuilt with it. The timeline in windows shows
# the start-up (the first windows) and the worst stretches without anything said about the program.
# Use: python tools/analysis/sampler.py (--cmd "line" [--hidden] | --pid N) [--seconds S] [--interval-ms M] [--window-s W]
#      [--symbolizer llvm-symbolizer.exe] [--json out.json]
import argparse
import ctypes as C
import ctypes.wintypes as W
import json
import os
import shutil
import struct
import subprocess
import time
from collections import defaultdict

from sampler_report import report

k32 = C.WinDLL("kernel32", use_last_error=True)
dbg = C.WinDLL("dbghelp", use_last_error=True)
psapi = C.WinDLL("psapi", use_last_error=True)
PROCESS_ALL = 0x1F0FFF
THREAD_ACCESS = 0x0002 | 0x0008 | 0x0010 | 0x0040  # suspend/resume, get context, query information
CONTEXT_FULL = 0x10000B
IMAGE_FILE_MACHINE_AMD64 = 0x8664
TH32CS_SNAPTHREAD = 0x4
MAX_FRAMES = 96
STILL_ACTIVE = 259
REFRESH_S = 0.5  # threads and modules listed again this often
DESKTOP = os.environ.get("SAMPLER_DESKTOP", "sampler")


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


def start_hidden(cmd):
    """CreateProcess on a desktop of its own (WinSta0\\sampler): the program's windows never show to the user."""
    u32 = C.WinDLL("user32", use_last_error=True)
    u32.CreateDesktopW.restype = C.c_void_p
    u32.CreateDesktopW(DESKTOP, None, None, 0, 0x10000000, None)

    class StartupInfo(C.Structure):
        _fields_ = [("cb", W.DWORD), ("r", W.LPWSTR), ("desk", W.LPWSTR), ("t", W.LPWSTR), ("x", W.DWORD * 8), ("f", W.DWORD),
                    ("sw", W.WORD), ("r2", W.WORD), ("r3", C.c_void_p), ("h", C.c_void_p * 3)]

    class ProcessInfo(C.Structure):
        _fields_ = [("hp", W.HANDLE), ("ht", W.HANDLE), ("pid", W.DWORD), ("tid", W.DWORD)]
    si, pi = StartupInfo(), ProcessInfo()
    si.cb = C.sizeof(StartupInfo)
    si.desk = "WinSta0\\" + DESKTOP
    if not k32.CreateProcessW(None, C.create_unicode_buffer(cmd), None, None, False, 0, None, None, C.byref(si), C.byref(pi)):
        raise OSError(C.get_last_error(), "CreateProcess")
    return pi.pid


class Sampler:
    def __init__(self, pid):
        self.pid = pid
        self.hp = k32.OpenProcess(PROCESS_ALL, False, pid)
        dbg.SymSetOptions(0x2 | 0x10)  # UNDNAME | DEFERRED_LOADS
        dbg.SymInitializeW(self.hp, None, True)
        self.table = C.cast(dbg.SymFunctionTableAccess64, C.c_void_p)
        self.base = C.cast(dbg.SymGetModuleBase64, C.c_void_p)
        self.opened = {}
        self.main = None  # the first thread seen: the program's main loop
        self.modules = {}  # base -> (end, path), gathered while sampling: the process may be gone at the end

    def stack(self, tid):
        """(the thread's stack, its CPU ns so far)."""
        h = self.opened.get(tid)
        if h is None:
            h = self.opened[tid] = k32.OpenThread(THREAD_ACCESS, False, tid)
        if not h or k32.SuspendThread(h) == 0xFFFFFFFF:
            return None, 0
        made, ended, kernel, user = W.FILETIME(), W.FILETIME(), W.FILETIME(), W.FILETIME()
        k32.GetThreadTimes(h, C.byref(made), C.byref(ended), C.byref(kernel), C.byref(user))
        cpu = sum((f.dwHighDateTime << 32 | f.dwLowDateTime) * 100 for f in (kernel, user))
        try:
            buf = (C.c_byte * (C.sizeof(ContextX64) + 16))()
            aligned = (C.addressof(buf) + 15) & ~15  # CONTEXT wants 16-byte alignment
            ctx = ContextX64.from_address(aligned)
            ctx.ContextFlags = CONTEXT_FULL
            if not k32.GetThreadContext(h, C.c_void_p(aligned)):
                return None, cpu
            f = Frame()
            f.AddrPC.Offset, f.AddrFrame.Offset, f.AddrStack.Offset = ctx.Rip, ctx.Rbp, ctx.Rsp
            f.AddrPC.Mode = f.AddrFrame.Mode = f.AddrStack.Mode = 3  # AddrModeFlat
            out = []
            while len(out) < MAX_FRAMES and dbg.StackWalk64(IMAGE_FILE_MACHINE_AMD64, self.hp, h, C.byref(f), C.c_void_p(aligned), None,
                                                            self.table, self.base, None):
                if not f.AddrPC.Offset:
                    break
                out.append(f.AddrPC.Offset)
            return tuple(out), cpu
        finally:
            k32.ResumeThread(h)

    def run(self, seconds, interval):
        samples = []  # (t, tid, stack, CPU ns since the thread's previous sample, wall ns since it)
        before, t0, listed, ids, code = {}, time.perf_counter(), 0.0, [], W.DWORD()
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
            for tid in ids:
                stack, cpu = self.stack(tid)
                prev = before.get(tid)
                before[tid] = (cpu, now)
                if stack and prev:
                    samples.append((now, tid, stack, cpu - prev[0], (now - prev[1]) * 1e9))
            time.sleep(interval)
        return samples


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
        with open(path, "rb") as f:
            head = f.read(4096)
        pe = struct.unpack_from("<I", head, 0x3C)[0]
        image_base = struct.unpack_from("<Q", head, pe + 24 + 24)[0]
        r = subprocess.run([symbolizer, f"--obj={path}", "--demangle", "--no-inlines"],
                           input="\n".join(f"0x{image_base + a - base:x}" for a in items), capture_output=True, text=True)
        trusted = 0
        for a, block in zip(items, (b.splitlines() for b in r.stdout.strip().split("\n\n"))):
            # with file and line only: without debug info a symbolizer gives the nearest export, which misleads
            if len(block) > 1 and block[0] not in ("??", "") and not block[1].startswith("??"):
                names[a] = f"{os.path.basename(path)}!{block[0]} ({os.path.basename(block[1])})"
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
    pid = a.pid or (start_hidden(a.cmd) if a.hidden else subprocess.Popen(a.cmd).pid)
    s = Sampler(pid)
    samples = s.run(a.seconds, a.interval_ms / 1000.0)
    names, undebugged = symbols(s, {e for _, _, st, _, _ in samples for e in st}, a.symbolizer)
    if undebugged:
        print("WARNING: no debug information (build with it to see functions and lines): " + ", ".join(sorted(undebugged)))
    data = report(samples, names, s.main, a.window_s, a.interval_ms)
    if a.json:
        with open(a.json, "w", encoding="utf-8") as f:
            json.dump(data, f, ensure_ascii=False, indent=1)
    if a.cmd:
        k32.TerminateProcess(s.hp, 0)


if __name__ == "__main__":
    main()
