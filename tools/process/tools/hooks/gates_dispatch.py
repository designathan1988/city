"""Despacho dos hooks dos gates para o repositório certo.

Os hooks do jogo (`tools/gates/hook_protect.py` em PreToolUse, `tools/gates/hook_stop_gates.py` em Stop) vivem em
`fonte/.claude/settings.json` e só carregam numa sessão aberta em `fonte/`. Uma sessão aberta na raiz do projeto ou
num clone (`ports/<feature>/workspace`) não os tinha: este despacho, ligado no settings da raiz e no do usuário, acha o
repositório que a chamada toca (pelo arquivo editado, pelos caminhos do comando ou pelo cwd) e roda o hook daquele
repositório com `CLAUDE_PROJECT_DIR` apontando para ele. Fora de um repositório com gates, sai em silêncio.
Uso: gates_dispatch.py pre|stop   (lê o JSON do hook em stdin, devolve o stdout e o código do hook despachado)
"""
import json
import os
import re
import subprocess
import sys

RAIZ = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
HOOKS = {"pre": "hook_protect.py", "stop": "hook_stop_gates.py"}
CAMINHO = re.compile(r"[\w:./\\~-]+")


def normalizar(c):
    c = str(c).strip().strip("\"'")
    m = re.match(r"^/([a-zA-Z])/(.*)$", c)
    if m:
        c = f"{m.group(1)}:/{m.group(2)}"
    return os.path.abspath(c).replace("\\", "/")


def repositorios():
    """fonte e todo clone de ports/*/workspace que tenha os gates."""
    saida = []
    for cand in [os.path.join(RAIZ, "fonte")] + [os.path.join(RAIZ, "ports", p, "workspace") for p in (os.listdir(os.path.join(RAIZ, "ports")) if os.path.isdir(os.path.join(RAIZ, "ports")) else [])]:
        if os.path.isfile(os.path.join(cand, "tools", "gates", "hook_protect.py")):
            saida.append(normalizar(cand))
    return saida


def repositorio_de(caminho, repos):
    c = normalizar(caminho).lower()
    for r in repos:
        if c.startswith(r.lower() + "/") or c == r.lower():
            return r
    return None


def alvos(dados, repos, qual):
    tool, inp, cwd = dados.get("tool_name", ""), dados.get("tool_input") or {}, dados.get("cwd") or os.getcwd()
    achados = []
    if qual == "stop":
        r = repositorio_de(cwd, repos)
        return [r] if r else []
    if tool in ("Edit", "Write", "MultiEdit", "NotebookEdit"):
        r = repositorio_de(inp.get("file_path", ""), repos)
        return [r] if r else []
    if tool in ("Bash", "PowerShell"):
        comando = inp.get("command", "")
        base = normalizar(cwd)
        r = repositorio_de(base, repos)
        if r:
            achados.append(r)
        for seg in re.split(r"\|\||&&|;|\n", comando):
            m = re.match(r"^\s*cd\s+(\S+)", seg)
            if m:
                alvo = m.group(1).strip("\"'")
                alvo = alvo if os.path.isabs(alvo) or re.match(r"^/[a-z]/", alvo) else os.path.join(base, alvo)
                if os.path.isdir(normalizar(alvo)):
                    base = normalizar(alvo)
                    rr = repositorio_de(base, repos)
                    if rr and rr not in achados:
                        achados.append(rr)
            for tok in CAMINHO.findall(seg):
                if "/" in tok or "\\" in tok:
                    rr = repositorio_de(tok if os.path.isabs(tok) or re.match(r"^/[a-z]/", tok) else os.path.join(base, tok), repos)
                    if rr and rr not in achados:
                        achados.append(rr)
    return achados


def main():
    qual = sys.argv[1] if len(sys.argv) > 1 else "pre"
    try:
        dados = json.load(sys.stdin)
    except ValueError:
        return 0
    repos = repositorios()
    bruto = json.dumps(dados)
    # A sessão aberta dentro de um repositório com gates já roda os hooks dele pelo próprio settings: não repetir.
    proprio = normalizar(os.environ.get("CLAUDE_PROJECT_DIR", "")).lower() if os.environ.get("CLAUDE_PROJECT_DIR") else ""
    for repo in alvos(dados, repos, qual):
        if repo.lower() == proprio:
            continue
        env = dict(os.environ, CLAUDE_PROJECT_DIR=repo)
        r = subprocess.run([sys.executable, os.path.join(repo, "tools", "gates", HOOKS[qual])], input=bruto, capture_output=True,
                           text=True, encoding="utf8", errors="replace", env=env, cwd=repo, timeout=120)
        if r.stdout.strip():
            sys.stdout.reconfigure(encoding="utf-8")
            print(r.stdout.strip())
            return r.returncode
        if r.returncode == 2:
            sys.stderr.write(r.stderr)
            return 2
    return 0


if __name__ == "__main__":
    sys.exit(main())
