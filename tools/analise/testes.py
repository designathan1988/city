# Executor paralelo da suíte: as mesmas cenas, renderizadores, tempo-limite e regra de falha de tools/build/run_tests.sh
# (lidos de lá: uma lista só), mas todas ao mesmo tempo, cada uma num processo do Godot na área de trabalho oculta
# (tools/build/hidden_run.ps1). Grava os mesmos logs em dist/tests que o hook de commit confere. O tempo total é o do teste
# mais longo, não a soma: o rigor é o mesmo, a espera não. --so=a,b roda só as cenas cujo nome contém a ou b.
# Uso: python tools/analise/testes.py [--godot EXE] [--paralelos N] [--so=nomes] [--timeout S]
import argparse
import os
import re
import shutil
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor

RAIZ = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..")) # --raiz troca (outra cópia do projeto)
FALHA = re.compile(r"SCRIPT ERROR|^FAIL|FALHAS: [1-9]", re.M)


def lista(nome, texto):
    m = re.search(r"^" + nome + r"=\((.*?)\)", texto, re.M)
    return m.group(1).split() if m else []


def casos():
    """(cena, renderizador) na ordem de run_tests.sh: todas em gl_compatibility, as BOTH_RENDERERS também em forward_plus."""
    texto = open(os.path.join(RAIZ, "tools", "build", "run_tests.sh"), encoding="utf-8").read()
    ambos = set(lista("BOTH_RENDERERS", texto))
    out = []
    for t in lista("TESTS", texto):
        out.append((t, "gl_compatibility"))
        if t in ambos:
            out.append((t, "forward_plus"))
    return out


def rodar(godot, cena, metodo, timeout):
    logs = os.path.join(RAIZ, "dist", "tests")
    log = os.path.join(logs, f"{os.path.basename(cena)}_{metodo}.log")
    linha = f'"{godot}" --path "{os.path.join(RAIZ, "project")}" --rendering-method {metodo} "res://{cena}.tscn" -- --no-flow > "{log}" 2>&1'
    t0 = time.time()
    r = subprocess.run(["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", os.path.join(RAIZ, "tools", "build", "hidden_run.ps1"),
                        "-Command", linha, "-TimeoutS", str(timeout), "-WorkDir", RAIZ], capture_output=True, text=True)
    codigo = r.returncode
    texto = open(log, encoding="utf-8", errors="replace").read() if os.path.exists(log) else ""
    falhou = codigo != 0 or bool(FALHA.search(texto))
    motivo = "; ".join(l for l in texto.splitlines() if FALHA.search(l))[:300]
    return cena, metodo, falhou, codigo, time.time() - t0, motivo


def main():
    global RAIZ
    ap = argparse.ArgumentParser()
    ap.add_argument("--godot", default=shutil.which("godot") or "godot")
    ap.add_argument("--paralelos", type=int, default=max(2, (os.cpu_count() or 4) // 3))
    ap.add_argument("--so", default="")
    ap.add_argument("--timeout", type=int, default=900)
    ap.add_argument("--raiz", default=RAIZ)
    a = ap.parse_args()
    RAIZ = os.path.abspath(a.raiz)
    os.makedirs(os.path.join(RAIZ, "dist", "tests"), exist_ok=True)
    filtro = [f for f in a.so.split(",") if f]
    todos = [c for c in casos() if not filtro or any(f in c[0] for f in filtro)]
    t0 = time.time()
    with ThreadPoolExecutor(max_workers=a.paralelos) as ex:
        resultados = list(ex.map(lambda c: rodar(a.godot, c[0], c[1], a.timeout), todos))
    # Uma falha sob carga é repetida sozinha: se passa, é INSTÁVEL (dependente de tempo, relatada, nunca escondida); se falha de
    # novo, é falha. O log que fica em dist/tests é o da execução sozinha.
    instaveis, falhas = [], 0
    for i, (cena, metodo, falhou, codigo, dur, motivo) in enumerate(resultados):
        if falhou:
            de_novo = rodar(a.godot, cena, metodo, a.timeout)
            if not de_novo[2]:
                instaveis.append(f"{cena} ({metodo}): {motivo}")
                resultados[i] = de_novo
    for cena, metodo, falhou, codigo, dur, motivo in resultados:
        falhas += falhou
        print(f"{'FALHOU' if falhou else 'ok'} {cena} ({metodo}) {dur:.0f} s" + (f" saída {codigo}: {motivo}" if falhou else ""))
    for x in instaveis:
        print(f"INSTÁVEL sob carga (passou sozinho): {x}")
    print(f"{len(resultados) - falhas}/{len(resultados)} ok, {len(instaveis)} instáveis, em {time.time() - t0:.0f} s ({a.paralelos} em paralelo)")
    sys.exit(1 if falhas else 0)


if __name__ == "__main__":
    main()
