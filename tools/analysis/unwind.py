# The x64 unwind tables of any module, read from the target process's own memory (the PE exception directory, .pdata), for
# dbghelp's StackWalk64. Without them dbghelp only knows the modules it has symbols for (PDB): in a module built by clang or gcc
# (DWARF) it falls back to the frame pointer, which optimised code does not keep, and the walk is lost above the first frame.
# Every x64 PE carries these tables (the OS unwinds exceptions with them), so this works for any module of any compiler.
import bisect
import ctypes as C
import struct

k32 = C.WinDLL("kernel32", use_last_error=True)
dbg = C.WinDLL("dbghelp", use_last_error=True)
k32.ReadProcessMemory.argtypes = [C.c_void_p, C.c_void_p, C.c_void_p, C.c_size_t, C.POINTER(C.c_size_t)]
dbg.SymFunctionTableAccess64.restype = C.c_void_p
dbg.SymFunctionTableAccess64.argtypes = [C.c_void_p, C.c_uint64]
dbg.SymGetModuleBase64.restype = C.c_uint64
dbg.SymGetModuleBase64.argtypes = [C.c_void_p, C.c_uint64]

E_LFANEW = 0x3C
OPTIONAL_HEADER = 24  # after the PE signature and the file header
EXCEPTION_DIRECTORY = 112 + 3 * 8  # PE32+ data directories start at 112; the exception one is the fourth
ENTRY = struct.Struct("<III")  # IMAGE_RUNTIME_FUNCTION_ENTRY: begin, end, unwind data (RVAs)
TABLE_ACCESS = C.WINFUNCTYPE(C.c_void_p, C.c_void_p, C.c_uint64)
READ_MEMORY = C.WINFUNCTYPE(C.c_int, C.c_void_p, C.c_uint64, C.c_void_p, C.c_uint32, C.POINTER(C.c_uint32))
MODULE_BASE = C.WINFUNCTYPE(C.c_uint64, C.c_void_p, C.c_uint64)


class MemoryInfo(C.Structure):  # MEMORY_BASIC_INFORMATION (x64)
    _fields_ = [("base", C.c_uint64), ("allocation_base", C.c_uint64), ("allocation_protect", C.c_uint32), ("partition", C.c_uint16),
                ("region_size", C.c_uint64), ("state", C.c_uint32), ("protect", C.c_uint32), ("type", C.c_uint32)]


k32.VirtualQueryEx.argtypes = [C.c_void_p, C.c_void_p, C.POINTER(MemoryInfo), C.c_size_t]


class RuntimeFunction(C.Structure):
    _fields_ = [("begin", C.c_uint32), ("end", C.c_uint32), ("unwind", C.c_uint32)]


class UnwindTables:
    """StackWalk64's FunctionTableAccess and GetModuleBase routines over the modules the sampler lists (base -> (end, path))."""

    def __init__(self, hp, modules):
        self.hp, self.modules = hp, modules
        self.tables = {}  # base -> (sorted begins, entries)
        self.bases, self.ends, self.listed = [], [], -1  # the modules sorted by base, made again when the list grows
        self.entries = {}  # address -> the entry found for it (a sampled program returns to the same places)
        self.found = RuntimeFunction()  # what the last lookup returned: dbghelp reads it before the next call
        self.table_access = TABLE_ACCESS(self._table_access)
        self.module_base = MODULE_BASE(self._module_base)
        self.read_memory = READ_MEMORY(self._read_memory)
        self.code = {}  # (address, size) -> bytes read outside the stack copy
        self.stack_at, self.stack = 0, b""  # the frozen thread's stack, copied: the walk reads it after the thread runs again

    def copy_stack(self, rsp, most):
        """While the thread is frozen: its stack from rsp to the end of the stack's region (at most `most` bytes)."""
        info = MemoryInfo()
        size = most
        if k32.VirtualQueryEx(self.hp, C.c_void_p(rsp), C.byref(info), C.sizeof(info)):
            size = min(most, info.base + info.region_size - rsp)
        self.stack_at, self.stack = rsp, self._read(rsp, size) or b""

    def _read_memory(self, hp, address, buf, size, got):
        """The stack from the copy (as it was when frozen); code and unwind data from the process (they do not change)."""
        off = address - self.stack_at
        if 0 <= off and off + size <= len(self.stack):
            C.memmove(buf, self.stack[off:off + size], size)
            if got:
                got[0] = size
            return 1
        key = (address, size)
        data = self.code.get(key, False)
        if data is False:  # code and unwind data never change while the program runs: each read once
            data = self.code[key] = self._read(address, size)
        if data is None:
            return 0
        C.memmove(buf, data, size)
        if got:
            got[0] = size
        return 1

    def _read(self, address, size):
        buf, got = (C.c_ubyte * size)(), C.c_size_t()
        if not k32.ReadProcessMemory(self.hp, C.c_void_p(address), buf, size, C.byref(got)) or got.value != size:
            return None
        return bytes(buf)

    def _base_of(self, address):
        if self.listed != len(self.modules):
            self.bases = sorted(self.modules)
            self.ends = [self.modules[b][0] for b in self.bases]
            self.listed = len(self.modules)
        i = bisect.bisect_right(self.bases, address) - 1
        return self.bases[i] if i >= 0 and address < self.ends[i] else 0

    def _table(self, base):
        if base not in self.tables:
            begins, entries = [], []
            head = self._read(base, 0x400)
            if head:
                pe = struct.unpack_from("<I", head, E_LFANEW)[0]
                d = self._read(base + pe + OPTIONAL_HEADER + EXCEPTION_DIRECTORY, 8)
                rva, size = struct.unpack("<II", d) if d else (0, 0)
                data = self._read(base + rva, size) if rva and size else None
                if data:
                    entries = list(ENTRY.iter_unpack(data[:size - size % ENTRY.size]))
                    entries.sort()
                    begins = [e[0] for e in entries]
            self.tables[base] = (begins, entries)
        return self.tables[base]

    def _table_access(self, hp, address):
        base = self._base_of(address)
        if not base:
            return dbg.SymFunctionTableAccess64(hp, address)
        entry = self.entries.get(address, False)
        if entry is False:
            begins, entries = self._table(base)
            i = bisect.bisect_right(begins, address - base) - 1
            entry = self.entries[address] = entries[i] if i >= 0 and entries[i][0] <= address - base < entries[i][1] else None
        if entry is None:
            return None  # a leaf function: no entry, the return address is on top of the stack
        self.found.begin, self.found.end, self.found.unwind = entry
        return C.addressof(self.found)

    def _module_base(self, hp, address):
        return self._base_of(address) or dbg.SymGetModuleBase64(hp, address)
