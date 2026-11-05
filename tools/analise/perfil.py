# Amostrador de pilhas genérico (Windows x64): qualquer processo, qualquer linguagem compilada, nenhum nome do projeto.
# Congela cada thread do processo a cada intervalo, lê a pilha (dbghelp StackWalk64, tabelas de unwind do próprio binário),
# solta a thread e soma: onde cada thread gasta tempo (inclusivo e próprio) e onde ela fica parada esperando, atribuindo a
# espera ao primeiro quadro de código da aplicação acima do sistema. Símbolos por dbghelp (PDB/exports) e, para binários
# com DWARF (clang/mingw), pelo llvm-symbolizer. A linha do tempo em janelas mostra a abertura (as primeiras janelas) e
# os trechos piores sem que nada precise ser dito sobre o programa.
# Uso: python tools/analise/perfil.py (--cmd "linha" [--oculto] | --pid N) [--segundos S] [--intervalo-ms M] [--janela-s J]
#      [--simbolizador llvm-symbolizer.exe] [--json saida.json]
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

from perfil_relatorio import relatorio

k32 = C.WinDLL("kernel32", use_last_error=True)
dbg = C.WinDLL("dbghelp", use_last_error=True)
psapi = C.WinDLL("psapi", use_last_error=True)
PROCESS_ALL = 0x1F0FFF
THREAD_ACCESS = 0x0002 | 0x0008 | 0x0010 | 0x0040  # suspend/resume, get context, query information
CONTEXT_FULL = 0x10000B
IMAGE_FILE_MACHINE_AMD64 = 0x8664
TH32CS_SNAPTHREAD = 0x4
MAX_FRAMES = 96


class ContextoX64(C.Structure):  # CONTEXT (winnt.h, x64): só os campos que o StackWalk64 lê; o resto é espaço
    _fields_ = [("P", C.c_uint64 * 6), ("ContextFlags", W.DWORD), ("MxCsr", W.DWORD), ("Seg", W.WORD * 6), ("EFlags", W.DWORD),
                ("Dr", C.c_uint64 * 6), ("Rax", C.c_uint64), ("Rcx", C.c_uint64), ("Rdx", C.c_uint64), ("Rbx", C.c_uint64),
                ("Rsp", C.c_uint64), ("Rbp", C.c_uint64), ("Rsi", C.c_uint64), ("Rdi", C.c_uint64), ("R", C.c_uint64 * 8),
                ("Rip", C.c_uint64), ("Resto", C.c_byte * 976)]


class Endereco(C.Structure):
    _fields_ = [("Offset", C.c_uint64), ("Segment", W.WORD), ("Mode", C.c_int)]


class Quadro(C.Structure):  # STACKFRAME64
    _fields_ = [("AddrPC", Endereco), ("AddrReturn", Endereco), ("AddrFrame", Endereco), ("AddrStack", Endereco),
                ("AddrBStore", Endereco), ("FuncTableEntry", C.c_void_p), ("Params", C.c_uint64 * 4), ("Far", W.BOOL),
                ("Virtual", W.BOOL), ("Reserved", C.c_uint64 * 3), ("KdHelp", C.c_byte * 112)]


class Thread32(C.Structure):
    _fields_ = [("dwSize", W.DWORD), ("cntUsage", W.DWORD), ("th32ThreadID", W.DWORD), ("th32OwnerProcessID", W.DWORD),
                ("tpBasePri", C.c_long), ("tpDeltaPri", C.c_long), ("dwFlags", W.DWORD)]


def threads_de(pid):
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


def iniciar_oculto(cmd):
    """CreateProcess numa área de trabalho própria (WinSta0\\analise): as janelas do programa não aparecem para o usuário."""
    u32 = C.WinDLL("user32", use_last_error=True)
    u32.CreateDesktopW.restype = C.c_void_p
    u32.CreateDesktopW(os.environ.get("ANALISE_DESKTOP", "analise"), None, None, 0, 0x10000000, None)

    class SI(C.Structure):
        _fields_ = [("cb", W.DWORD), ("r", W.LPWSTR), ("desk", W.LPWSTR), ("t", W.LPWSTR), ("x", W.DWORD * 8), ("f", W.DWORD),
                    ("sw", W.WORD), ("r2", W.WORD), ("r3", C.c_void_p), ("h", C.c_void_p * 3)]

    class PI(C.Structure):
        _fields_ = [("hp", W.HANDLE), ("ht", W.HANDLE), ("pid", W.DWORD), ("tid", W.DWORD)]
    si, pi = SI(), PI()
    si.cb = C.sizeof(SI)
    si.desk = "WinSta0\\" + os.environ.get("ANALISE_DESKTOP", "analise")
    if not k32.CreateProcessW(None, C.create_unicode_buffer(cmd), None, None, False, 0, None, None, C.byref(si), C.byref(pi)):
        raise OSError(C.get_last_error(), "CreateProcess")
    return pi.pid


class Amostrador:
    def __init__(self, pid):
        self.pid = pid
        self.hp = k32.OpenProcess(PROCESS_ALL, False, pid)
        dbg.SymSetOptions(0x2 | 0x10)  # UNDNAME | DEFERRED_LOADS
        dbg.SymInitializeW(self.hp, None, True)
        self.tabela = C.cast(dbg.SymFunctionTableAccess64, C.c_void_p)
        self.base = C.cast(dbg.SymGetModuleBase64, C.c_void_p)
        dbg.SymGetModuleBase64.restype = C.c_uint64
        dbg.SymGetModuleBase64.argtypes = [W.HANDLE, C.c_uint64]
        self.abertos = {}
        self.principal = None  # a primeira thread vista: a do laço principal do programa

    def pilha(self, tid):
        h = self.abertos.get(tid)
        if h is None:
            h = self.abertos[tid] = k32.OpenThread(THREAD_ACCESS, False, tid)
        if not h or k32.SuspendThread(h) == 0xFFFFFFFF:
            return None, 0
        cria, sai, kern, user = W.FILETIME(), W.FILETIME(), W.FILETIME(), W.FILETIME()
        k32.GetThreadTimes(h, C.byref(cria), C.byref(sai), C.byref(kern), C.byref(user))
        cpu = sum((f.dwHighDateTime << 32 | f.dwLowDateTime) * 100 for f in (kern, user))  # ns de CPU da thread até agora
        try:
            ctx = ContextoX64()
            ctx.ContextFlags = CONTEXT_FULL
            buf = (C.c_byte * (C.sizeof(ContextoX64) + 16))()
            al = (C.addressof(buf) + 15) & ~15  # CONTEXT exige 16 bytes de alinhamento
            C.memmove(al, C.byref(ctx), C.sizeof(ctx))
            if not k32.GetThreadContext(h, C.c_void_p(al)):
                return None, cpu
            ctx = ContextoX64.from_address(al)
            q = Quadro()
            q.AddrPC.Offset, q.AddrFrame.Offset, q.AddrStack.Offset = ctx.Rip, ctx.Rbp, ctx.Rsp
            q.AddrPC.Mode = q.AddrFrame.Mode = q.AddrStack.Mode = 3  # AddrModeFlat
            out = []
            while len(out) < MAX_FRAMES and dbg.StackWalk64(IMAGE_FILE_MACHINE_AMD64, self.hp, h, C.byref(q), C.c_void_p(al), None,
                                                            self.tabela, self.base, None):
                if not q.AddrPC.Offset:
                    break
                out.append(q.AddrPC.Offset)
            return tuple(out), cpu
        finally:
            k32.ResumeThread(h)

    def rodar(self, segundos, intervalo):
        amostras = []  # (t, tid, pilha, ns de CPU desde a amostra anterior da thread, ns de relógio desde ela)
        antes = {}
        t0, listados, ids = time.perf_counter(), 0.0, []
        codigo = W.DWORD()
        while time.perf_counter() - t0 < segundos:
            agora = time.perf_counter() - t0
            if agora - listados > 0.5 or not ids:
                ids, listados = threads_de(self.pid), agora
                dbg.SymRefreshModuleList(self.hp)  # DLLs carregadas depois do início: sem elas a pilha se perde nelas
                if ids and self.principal is None:
                    self.principal = ids[0]
            if not ids or (k32.GetExitCodeProcess(self.hp, C.byref(codigo)) and codigo.value != 259):
                break
            for tid in ids:
                p, cpu = self.pilha(tid)
                anterior = antes.get(tid)
                antes[tid] = (cpu, agora)
                if p and anterior:
                    amostras.append((agora, tid, p, cpu - anterior[0], (agora - anterior[1]) * 1e9))
            time.sleep(intervalo)
        return amostras


class InfoModulo(C.Structure):
    _fields_ = [("base", C.c_void_p), ("tamanho", W.DWORD), ("entrada", C.c_void_p)]


def modulos_de(hp):
    hs = (C.c_void_p * 2048)()
    preciso = W.DWORD()
    psapi.EnumProcessModulesEx(hp, hs, C.sizeof(hs), C.byref(preciso), 3)  # LIST_MODULES_ALL
    out, caminho, info = [], C.create_unicode_buffer(1024), InfoModulo()
    for h in hs[:preciso.value // C.sizeof(C.c_void_p)]:
        if h and psapi.GetModuleInformation(hp, C.c_void_p(h), C.byref(info), C.sizeof(info)):
            psapi.GetModuleFileNameExW(hp, C.c_void_p(h), caminho, 1024)
            out.append((info.base, info.base + info.tamanho, caminho.value))
    return out


def simbolos(amost, enderecos, simbolizador):
    """endereço -> 'módulo!função' (dbghelp; com DWARF, llvm-symbolizer no módulo pela sua base preferida)."""
    nomes, por_modulo, sem_depuracao = {}, defaultdict(list), set()
    modulos = modulos_de(amost.hp)  # a lista real do processo (base, fim, caminho): não depende do que o dbghelp viu
    for a in enderecos:
        achado = next(((b, m) for b, fim, m in modulos if b <= a < fim), None)
        if achado is None:
            nomes[a] = f"?!0x{a:x}"  # fora de qualquer módulo: quadro perdido (código gerado ou pilha sem unwind)
            continue
        base, mod = achado
        por_modulo[(base, mod)].append(a)
        nomes[a] = f"{os.path.basename(mod)}!+0x{a - base:x}"
    for (base, mod), lista in por_modulo.items():
        if not mod or not simbolizador or not os.path.exists(mod):
            continue
        with open(mod, "rb") as f:
            cab = f.read(4096)
        pe = struct.unpack_from("<I", cab, 0x3C)[0]
        img_base = struct.unpack_from("<Q", cab, pe + 24 + 24)[0]
        entrada = "\n".join(f"0x{img_base + a - base:x}" for a in lista)
        r = subprocess.run([simbolizador, f"--obj={mod}", "--demangle", "--no-inlines"], input=entrada, capture_output=True, text=True)
        blocos = [b.splitlines() for b in r.stdout.strip().split("\n\n")]
        confiaveis = 0
        for a, b in zip(lista, blocos):
            # só com arquivo e linha (informação de depuração): sem ela o simbolizador dá o export mais próximo, que engana
            if len(b) > 1 and b[0] not in ("??", "") and not b[1].startswith("??"):
                nomes[a] = f"{os.path.basename(mod)}!{b[0]} ({os.path.basename(b[1])})"
                confiaveis += 1
        if not confiaveis and not mod.lower().startswith(os.environ.get("WINDIR", "C:\\Windows").lower()):
            sem_depuracao.add(os.path.basename(mod))
    return nomes, sem_depuracao


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cmd")
    ap.add_argument("--pid", type=int)
    ap.add_argument("--oculto", action="store_true")
    ap.add_argument("--segundos", type=float, default=30)
    ap.add_argument("--intervalo-ms", type=float, default=5)
    ap.add_argument("--janela-s", type=float, default=2)
    ap.add_argument("--simbolizador", default=shutil.which("llvm-symbolizer") or "")
    ap.add_argument("--json")
    a = ap.parse_args()
    pid = a.pid or (iniciar_oculto(a.cmd) if a.oculto else subprocess.Popen(a.cmd).pid)
    time.sleep(0.05)
    am = Amostrador(pid)
    amostras = am.rodar(a.segundos, a.intervalo_ms / 1000.0)
    nomes, sem_depuracao = simbolos(am, {e for _, _, p, _, _ in amostras for e in p}, a.simbolizador)
    if sem_depuracao:
        print("AVISO: sem informação de depuração (compile com -g para ver funções e linhas): " + ", ".join(sorted(sem_depuracao)))
    dados = relatorio(amostras, nomes, am.principal, a.janela_s, a.intervalo_ms)
    if a.json:
        with open(a.json, "w", encoding="utf-8") as f:
            json.dump(dados, f, ensure_ascii=False, indent=1)
    if a.cmd:
        k32.TerminateProcess(am.hp, 0)


if __name__ == "__main__":
    main()
