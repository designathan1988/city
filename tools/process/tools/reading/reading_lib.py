"""Biblioteca do inventário de leitura: o que foi lido, de onde, quanto, com prova gerada pelo programa.

O inventário (`records/reading/inventory.json`) guarda, por fonte (arquivo ou URL), os intervalos lidos
(linhas, para arquivos; caracteres, para páginas), quem registrou (hook de ferramenta, importação da
transcrição ou anotação manual), o hash do arquivo na hora da leitura e as conclusões por bloco. A cobertura
é calculada, nunca declarada: um bloco de BLOCO_LINHAS linhas conta como lido só quando a união dos intervalos
o cobre inteiro.
"""
import hashlib
import json
import os
import re
import time
from datetime import datetime, timezone

RAIZ = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
PASTA = os.path.join(RAIZ, "records", "reading")
INVENTARIO = os.path.join(PASTA, "inventory.json")
OBRIGATORIOS = os.path.join(PASTA, "required.json")
ESTADO = os.path.join(PASTA, "state.json")
BLOCO_LINHAS = 200
BLOCO_CHARS = 20000
LIMITE_READ = 2000  # linhas que a ferramenta Read devolve sem `limit`
LIMITE_WEBFETCH = 100000  # caracteres que o WebFetch lê por chamada
RAIZES_DO_PROJETO = (RAIZ.replace("\\", "/").lower(), "c:/users/jonathan/documents/projects/roadcraft-4.0-main",
                     "c:/users/jonathan/documents/labs/roadcraft-4.0-main", "c:/users/jonathan/.claude")
TEXTO = {".md", ".txt", ".py", ".ts", ".tsx", ".js", ".mjs", ".cjs", ".json", ".cpp", ".h", ".gd", ".gdshader",
         ".gdshaderinc", ".glsl", ".sh", ".ps1", ".csv", ".cfg", ".cmake", ".yaml", ".yml", ".toml", ".html", ".css",
         ".tscn", ".godot", ".jsonl", ".log", ".ini", ".xml"}


def agora():
    return datetime.now(timezone.utc).isoformat(timespec="seconds")


def normalizar(caminho, base=None):
    """Caminho absoluto, barras normais, letra do disco minúscula: a mesma chave para `C:\\x` e `/c/x`.
    Caminho relativo resolve contra `base` (o cwd do comando) ou, sem base, contra a raiz do projeto."""
    c = str(caminho).strip().strip('"').strip("'")
    if c.startswith("http"):
        return c
    m = re.match(r"^/([a-zA-Z])/(.*)$", c)
    if m:
        c = f"{m.group(1)}:/{m.group(2)}"
    if c.startswith("~/"):
        c = os.path.expanduser(c)
    if not os.path.isabs(c):
        c = os.path.join(base or RAIZ, c)
    c = os.path.abspath(c).replace("\\", "/")
    if re.match(r"^[A-Z]:", c):
        c = c[0].lower() + c[1:]
    return c


def no_projeto(caminho):
    c = normalizar(caminho).lower()
    return any(c.startswith(r) for r in RAIZES_DO_PROJETO)


def relativo(caminho):
    c = normalizar(caminho)
    raiz = RAIZ.replace("\\", "/")
    raiz = raiz[0].lower() + raiz[1:]
    if c.lower().startswith(raiz.lower() + "/"):
        return c[len(raiz) + 1:]
    return c


def contar_linhas(caminho):
    with open(caminho, "rb") as f:
        dados = f.read()
    n = dados.count(b"\n")
    if dados and not dados.endswith(b"\n"):
        n += 1
    return n, hashlib.sha256(dados).hexdigest()


def carregar(arquivo, padrao):
    try:
        with open(arquivo, encoding="utf8") as f:
            return json.load(f)
    except (OSError, ValueError):
        return padrao


def gravar(arquivo, dados):
    os.makedirs(os.path.dirname(arquivo), exist_ok=True)
    tmp = arquivo + ".tmp"
    with open(tmp, "w", encoding="utf8") as f:
        json.dump(dados, f, ensure_ascii=False, indent=1)
    os.replace(tmp, arquivo)


def inventario():
    return carregar(INVENTARIO, {"versao": 1, "bloco_linhas": BLOCO_LINHAS, "bloco_chars": BLOCO_CHARS, "sources": {}})


def unir(intervalos):
    """União de intervalos fechados [a, b] (inteiros), em ordem."""
    saida = []
    for a, b in sorted((int(a), int(b)) for a, b in intervalos if int(b) >= int(a)):
        if saida and a <= saida[-1][1] + 1:
            saida[-1][1] = max(saida[-1][1], b)
        else:
            saida.append([a, b])
    return saida


def blocos_de(total, tamanho):
    return max(1, (total + tamanho - 1) // tamanho) if total > 0 else 0


def blocos_lidos(uniao, total, tamanho):
    lidos = []
    for k in range(blocos_de(total, tamanho)):
        a, b = k * tamanho + 1, min(total, (k + 1) * tamanho)
        if any(ia <= a and ib >= b for ia, ib in uniao):
            lidos.append(k + 1)
    return lidos


def fonte_arquivo(inv, caminho):
    chave = normalizar(caminho)
    f = inv["sources"].setdefault(chave, {"tipo": "arquivo", "intervalos": [], "registros": [], "conclusoes": []})
    if os.path.isfile(chave):
        linhas, sha = contar_linhas(chave)
        f["linhas"], f["sha256_atual"] = linhas, sha
    return chave, f


def registrar_arquivo(inv, caminho, a, b, por, nota="", sessao="", comando=""):
    """Registra as linhas [a, b] lidas de `caminho`; devolve a chave ou None se o arquivo não existe/não é texto."""
    chave = normalizar(caminho)
    if not os.path.isfile(chave) or os.path.splitext(chave)[1].lower() not in TEXTO:
        return None
    chave, f = fonte_arquivo(inv, chave)
    total = f["linhas"]
    a, b = max(1, int(a)), min(total, int(b))
    if total == 0 or b < a:
        return chave
    f["intervalos"] = unir(f["intervalos"] + [[a, b]])
    f["sha256_lido"] = f["sha256_atual"]
    f["registros"].append({"quando": agora(), "por": por, "linhas": [a, b], "sessao": sessao, "nota": nota,
                           "comando": comando[:160]})
    return chave


def registrar_url(inv, url, a, b, total, por, nota="", sessao=""):
    f = inv["sources"].setdefault(url, {"tipo": "url", "intervalos": [], "registros": [], "conclusoes": []})
    if total:
        f["chars"] = int(total)
    total = f.get("chars") or int(b)
    a, b = max(0, int(a)), min(total - 1, int(b))
    if b < a:
        return url
    f["intervalos"] = unir(f["intervalos"] + [[a, b]])
    f["registros"].append({"quando": agora(), "por": por, "chars": [a, b], "sessao": sessao, "nota": nota})
    return url


def cobertura_fonte(inv, chave, f):
    if f["tipo"] == "arquivo":
        if os.path.isfile(chave):
            total, sha = contar_linhas(chave)
        else:
            total, sha = f.get("linhas", 0), None
        tamanho = inv.get("bloco_linhas", BLOCO_LINHAS)
        uniao = unir(f["intervalos"])
        lidos = blocos_lidos(uniao, total, tamanho)
        n = blocos_de(total, tamanho)
        cobertas = sum(b - a + 1 for a, b in uniao if a <= total)
        return {"total": total, "unidade": "linhas", "blocos": n, "lidos": lidos, "cobertura": (cobertas / total) if total else 1.0,
                "desatualizado": bool(sha and f.get("sha256_lido") and sha != f["sha256_lido"]), "ausente": sha is None,
                "faltam": [k for k in range(1, n + 1) if k not in lidos]}
    total = f.get("chars", 0)
    tamanho = inv.get("bloco_chars", BLOCO_CHARS)
    uniao = unir(f["intervalos"])
    lidos = blocos_lidos([[a + 1, b + 1] for a, b in uniao], total, tamanho)
    n = blocos_de(total, tamanho)
    cobertas = sum(b - a + 1 for a, b in uniao)
    return {"total": total, "unidade": "chars", "blocos": n, "lidos": lidos, "cobertura": (cobertas / total) if total else 0.0,
            "desatualizado": False, "ausente": False, "faltam": [k for k in range(1, n + 1) if k not in lidos]}


def obrigatorios():
    """Lista de fontes obrigatórias: globs relativos à raiz ou URLs, com o motivo."""
    return carregar(OBRIGATORIOS, {"sources": []})


def expandir_obrigatorios(lista):
    import glob
    saida = []
    for item in lista.get("sources", []):
        padrao = item["padrao"]
        if padrao.startswith("http"):
            saida.append((padrao, item.get("motivo", "")))
            continue
        base = padrao if os.path.isabs(padrao) or re.match(r"^/[a-z]/", padrao) else os.path.join(RAIZ, padrao)
        base = normalizar(base)
        for c in sorted(glob.glob(base, recursive=True)):
            if os.path.isfile(c) and os.path.splitext(c)[1].lower() in TEXTO:
                saida.append((normalizar(c), item.get("motivo", "")))
    return saida


# --- leitura de comandos de shell: que arquivos e que linhas um comando leu ------------------------------------
SEGMENTO = re.compile(r"\|\||&&|;|\n")
RANGE_SED = re.compile(r"sed\s+-n\s+['\"]?(\d+),(\d+)p['\"]?")
HEAD = re.compile(r"head\s+(?:-n\s*|-)(\d+)")
TAIL = re.compile(r"tail\s+(?:-n\s*|-)(\d+)")
LEITORES = ("cat", "sed", "head", "tail", "less", "more", "type", "Get-Content", "gc")
TOKEN = re.compile(r"""(?:"[^"]+"|'[^']+'|[^\s|;&<>()]+)""")


VARIAVEL = re.compile(r"\$\{?(\w+)\}?")
ATRIBUICAO = re.compile(r"^\s*(\w+)=(\S+)\s*$")
LACO_FOR = re.compile(r"\bfor\s+(\w+)\s+in\s+([^;\n]+?)\s*;\s*do\b")


def _substituir(texto, variaveis):
    return VARIAVEL.sub(lambda m: variaveis.get(m.group(1), m.group(0)), texto)


def _candidatos(texto, variaveis, base):
    """Caminhos de arquivo que um trecho de comando menciona, com as variáveis conhecidas resolvidas."""
    saida = []
    for t in TOKEN.findall(_substituir(texto, variaveis)):
        t = t.strip("\"'")
        if "$" in t or not re.search(r"[/\\]", t) and "." not in t:
            continue
        if re.match(r"^-", t):
            continue
        c = normalizar(t, base)
        if os.path.isfile(c) and no_projeto(c):
            saida.append(c)
    return saida


def _expandir_lacos(comando, base):
    """`for f in a b c; do X; done` vira as cópias de X com `$f` trocado por cada item (globs expandidos)."""
    import glob
    saida = comando
    for m in list(LACO_FOR.finditer(comando)):
        var, lista = m.group(1), m.group(2)
        itens = []
        for t in TOKEN.findall(lista):
            t = t.strip("\"'")
            achados = glob.glob(os.path.join(base, t)) if not os.path.isabs(t) and not re.match(r"^/[a-z]/", t) else glob.glob(normalizar(t, base))
            itens += achados or [t]
        fim = comando.find("done", m.end())
        corpo = comando[m.end():fim if fim >= 0 else len(comando)]
        copias = ";".join(corpo.replace("${" + var + "}", item).replace("$" + var, item) for item in itens)
        saida = saida.replace(comando[m.start():(fim + 4) if fim >= 0 else len(comando)], copias, 1)
    return saida


def leituras_do_comando(comando, cwd):
    """[(caminho, a, b)] que um comando de shell leu inteiro ou por trecho. Heurística auditável: registra o
    comando junto, e só arquivos que existem no projeto."""
    saida = []
    base = normalizar(cwd) if cwd and os.path.isdir(normalizar(cwd)) else RAIZ
    corpo = comando
    # `cd X && cat ...`: o cd muda a base dos caminhos relativos do resto do comando
    m = re.match(r"^\s*cd\s+(\S+)\s*(?:&&|;)", corpo)
    if m:
        alvo = normalizar(m.group(1), base)
        if os.path.isdir(alvo):
            base = alvo
    variaveis = {}
    for seg in SEGMENTO.split(corpo):
        ma = ATRIBUICAO.match(seg)
        if ma:
            variaveis[ma.group(1)] = _substituir(ma.group(2).strip("\"'"), variaveis)
    corpo = _expandir_lacos(_substituir(corpo, variaveis), base)
    if True:
        for seg in SEGMENTO.split(corpo):
            partes = seg.split("|")
            primeiro = partes[0].strip()
            if not re.match(r"^\s*(?:" + "|".join(LEITORES) + r")\b", primeiro):
                continue
            faixa = None
            for p in partes[1:] + [primeiro]:
                ms = RANGE_SED.search(p)
                if ms:
                    faixa = ("sed", int(ms.group(1)), int(ms.group(2)))
                    break
                mh = HEAD.search(p)
                if mh and p is not primeiro or (mh and primeiro.strip().startswith("head")):
                    faixa = ("head", 1, int(mh.group(1)))
                    break
                mt = TAIL.search(p)
                if mt and (p is not primeiro or primeiro.strip().startswith("tail")):
                    faixa = ("tail", int(mt.group(1)), 0)
                    break
            for c in _candidatos(primeiro, variaveis, base):
                total, _ = contar_linhas(c)
                if faixa is None:
                    saida.append((c, 1, total))
                elif faixa[0] == "tail":
                    saida.append((c, max(1, total - faixa[1] + 1), total))
                else:
                    saida.append((c, faixa[1], min(total, faixa[2])))
    return saida


def leituras_da_ferramenta(nome, entrada, resposta, cwd):
    """O que uma chamada de ferramenta leu: lista de ("arquivo", caminho, a, b) ou ("url", url, a, b, total)."""
    entrada = entrada or {}
    if nome == "Read":
        caminho = entrada.get("file_path", "")
        if not caminho or not os.path.isfile(normalizar(caminho)):
            return []
        total, _ = contar_linhas(normalizar(caminho))
        a = int(entrada.get("offset") or 1)
        limite = int(entrada.get("limit") or LIMITE_READ)
        return [("arquivo", caminho, a, min(total, a + limite - 1))]
    if nome in ("Bash", "PowerShell"):
        return [("arquivo", c, a, b) for c, a, b in leituras_do_comando(entrada.get("command", ""), cwd)]
    if nome == "WebFetch":
        url = entrada.get("url", "")
        if not url:
            return []
        texto = resposta if isinstance(resposta, str) else json.dumps(resposta, ensure_ascii=False)
        total = None
        m = re.search(r"text is (\d+) characters long", texto)
        if m:
            total = int(m.group(1))
        m = re.search(r"covers only characters (\d+) to (\d+)", texto)
        if m:
            a, b = int(m.group(1)), int(m.group(2)) - 1
        else:
            a = int(entrada.get("offset") or 0)
            b = (total - 1) if total else a + LIMITE_WEBFETCH - 1
            if total is None:
                total = b + 1
        return [("url", url, a, b, total)]
    return []


def estado():
    return carregar(ESTADO, {})


def git(args, cwd):
    import subprocess
    try:
        r = subprocess.run(["git", *args], cwd=cwd, capture_output=True, text=True, encoding="utf8", errors="replace",
                           timeout=20)
        return r.stdout.strip() if r.returncode == 0 else ""
    except (OSError, subprocess.SubprocessError):
        return ""


def resumo_git(cwd):
    if not git(["rev-parse", "--show-toplevel"], cwd):
        return {}
    sujos = [l for l in git(["status", "--porcelain"], cwd).splitlines() if not l.endswith(".uid")]
    return {"raiz": git(["rev-parse", "--show-toplevel"], cwd), "branch": git(["branch", "--show-current"], cwd),
            "head": git(["log", "-1", "--format=%h %s"], cwd), "sujos": len(sujos), "arquivos_sujos": sujos[:20]}
