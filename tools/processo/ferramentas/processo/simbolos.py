"""Quem declara o que o código novo chama. A falha que motivou isto: chamar `rig.jump(...)` tendo lido só as 8 linhas da função, sem
ler `_apply`, que descarta a altura passada. Antes de uma escrita, cada nome chamado pela primeira vez no arquivo precisa ter um
declarador lido inteiro. O índice vem de expressões regulares por linguagem, guardado por data de arquivo; nome declarado em muitos
arquivos (`size`, `get`) é ambíguo demais para exigir leitura e fica de fora.
"""
import json
import os
import re

import estado as E

L = E.L
EXTS = {".gd", ".cpp", ".h", ".gdshader", ".gdshaderinc", ".py", ".cjs", ".mjs"}
PASTAS = ("native/src", "project", "tools", "ferramentas", "converter/scripts")
FORA = ("godot-cpp", ".godot", "addons", "node_modules", "__pycache__")
MAX_DECLARADORES = 6
PALAVRAS = set("if for while switch return sizeof catch func print range len int float str bool vec2 vec3 vec4 mat3 mat4 assert static_cast "
               "constexpr decltype defined".split())
DECL = {
    ".gd": re.compile(r"^\s*(?:static\s+)?func\s+(\w+)", re.M),
    ".py": re.compile(r"^\s*def\s+(\w+)", re.M),
    ".cjs": re.compile(r"\bfunction\s+(\w+)", re.M),
    ".mjs": re.compile(r"\bfunction\s+(\w+)", re.M),
    ".gdshader": re.compile(r"^[a-z]\w*\s+(\w+)\s*\(", re.M),
    ".gdshaderinc": re.compile(r"^[a-z]\w*\s+(\w+)\s*\(", re.M),
    ".cpp": re.compile(r"^[\w:<>,&*~ \t]*?\b(?:\w+::)?(\w+)\s*\([^;{}]*\)\s*(?:const\s*)?(?:noexcept\s*)?(?:override\s*)?\{", re.M),
    ".h": re.compile(r"^[ \t]*(?:[\w:<>,&*~]+[ \t]+)+[&*]?(\w+)\s*\([^;{}]*\)\s*(?:const\s*)?(?:noexcept\s*)?(?:override\s*)?(?:;|\{)", re.M),
}
CHAMADA = re.compile(r"\b([A-Za-z_]\w*)\s*\(")


def _arquivos(repo):
    for pasta in PASTAS:
        for base, dirs, nomes in os.walk(os.path.join(repo, pasta)):
            dirs[:] = [d for d in dirs if d not in FORA and not d.startswith("build")]
            for n in nomes:
                if os.path.splitext(n)[1].lower() in EXTS:
                    yield os.path.join(base, n)


def indice(repo):
    """nome -> arquivos que o declaram, reaproveitando do cache o que não mudou."""
    os.makedirs(E.PASTA, exist_ok=True)
    cache_arq = os.path.join(E.PASTA, "indice-" + re.sub(r"\W+", "_", L.relativo(repo)) + ".json")
    cache = L.carregar(cache_arq, {})
    novo, mudou = {}, False
    for c in _arquivos(repo):
        chave = L.normalizar(c)
        data = os.path.getmtime(c)
        item = cache.get(chave)
        if not item or item[0] != data:
            with open(c, encoding="utf8", errors="replace") as f:
                texto = f.read()
            item = [data, sorted(set(DECL[os.path.splitext(c)[1].lower()].findall(texto)))]
            mudou = True
        novo[chave] = item
    if mudou or len(novo) != len(cache):
        L.gravar(cache_arq, novo)
    nomes = {}
    for c, (_, declarados) in novo.items():
        for n in declarados:
            nomes.setdefault(n, []).append(c)
    return nomes


def repo_de(caminho):
    r = L.relativo(L.normalizar(caminho))
    m = re.match(r"^(fonte|ports/[^/]+/workspace)/", r)
    return os.path.join(L.RAIZ, m.group(1)) if m else L.RAIZ


def faltam(est, caminho, novo_texto, texto_antigo):
    """{nome: declaradores} do que o texto novo chama pela primeira vez sem declarador lido inteiro nem no plano."""
    ext = os.path.splitext(caminho)[1].lower()
    if ext not in EXTS:
        return {}
    alvo = L.normalizar(caminho)
    ja = set(CHAMADA.findall(texto_antigo)) | PALAVRAS
    proprio = set(DECL[ext].findall(novo_texto)) | set(DECL[ext].findall(texto_antigo))
    nomes = indice(repo_de(caminho))
    plano = set(est.get("plano", []))
    saida = {}
    for n in sorted(set(CHAMADA.findall(novo_texto)) - ja - proprio):
        declaradores = [c for c in nomes.get(n, []) if c != alvo]
        if not declaradores or len(declaradores) > MAX_DECLARADORES:
            continue
        if not any(c in plano or E.cobre(est, c) for c in declaradores):
            saida[n] = [L.relativo(c) for c in declaradores]
    return saida


def texto_antigo(caminho):
    try:
        with open(caminho, encoding="utf8", errors="replace") as f:
            return f.read()
    except OSError:
        return ""


__all__ = ["faltam", "indice", "texto_antigo", "json"]
