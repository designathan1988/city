# The Windows process and thread access the sampler walks with (Win64 x64): kernel32, dbghelp and psapi with their 64-bit
# handle types declared, the structures StackWalk64 reads, a process's threads (and which is its main one) and modules.
import ctypes as C
import ctypes.wintypes as W

k32 = C.WinDLL("kernel32", use_last_error=True)
dbg = C.WinDLL("dbghelp", use_last_error=True)
psapi = C.WinDLL("psapi", use_last_error=True)
k32.GetCurrentThread.restype = C.c_void_p  # a pseudo-handle (-2): 64 bits, never an int
k32.QueryThreadCycleTime.argtypes = [C.c_void_p, C.POINTER(C.c_uint64)]
for _f in (k32.OpenProcess, k32.OpenThread, k32.CreateToolhelp32Snapshot):  # HANDLEs: 64 bits, never an int
    _f.restype = C.c_void_p
k32.CloseHandle.argtypes = [C.c_void_p]
k32.SuspendThread.argtypes = k32.ResumeThread.argtypes = [C.c_void_p]
k32.GetThreadTimes.argtypes = [C.c_void_p] + [C.POINTER(W.FILETIME)] * 4
dbg.SymCleanup.argtypes = [C.c_void_p]
THREAD_QUERY_LIMITED = 0x0800
PROCESS_ALL = 0x1F0FFF
THREAD_ACCESS = 0x0002 | 0x0008 | 0x0010 | 0x0040  # suspend/resume, get context, query information
CONTEXT_FULL = 0x10000B
IMAGE_FILE_MACHINE_AMD64 = 0x8664
TH32CS_SNAPTHREAD = 0x4
STILL_ACTIVE = 259  # GetExitCodeProcess of a process still running


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


def main_thread(ids):
    """The process's oldest thread, the one its entry point runs on: Toolhelp32 lists threads in no documented order, so
    the first one listed is not it by rule."""
    born = {}
    for tid in ids:
        h = k32.OpenThread(THREAD_QUERY_LIMITED, False, tid)
        if not h:
            continue
        times = [W.FILETIME() for _ in range(4)]
        if k32.GetThreadTimes(h, *(C.byref(t) for t in times)):
            born[tid] = (times[0].dwHighDateTime << 32) | times[0].dwLowDateTime
        k32.CloseHandle(h)
    return min(born, key=born.get) if born else ids[0]


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
