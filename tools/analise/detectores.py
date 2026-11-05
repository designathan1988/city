# Detectores de causa-raiz por padrão de código, sem nomes nem versões fixas: cada um procura a forma de um defeito
# (quem trava o quê, quem aloca por quadro, quem lê com efeito colateral, quem nunca poda) em qualquer cópia do projeto.
# Cada achado diz a causa, onde está e o que a prova; o efeito que ela produz vai em `efeito`.
import os
import re

CPP_EXT = (".cpp", ".h", ".hpp")
GD_EXT = (".gd",)
SKIP_DIRS = {".git", ".godot", "godot-cpp", "addons", "build-win", "build-win-debug", "build-linux", "__pycache__",
             "oracle-4.0", "converter", "converter-slice", "portes", "processo", "dist", "bin"}
FUNC_HEAD = re.compile(r"^\s*[A-Za-z_][\w:<>,\s\*&~]*?\b([A-Za-z_]\w*(?:::~?\w+)*)\s*\(([^;{}]*)\)\s*(const)?[^;{}=]*\{\s*$")
SIGNATURE_LINES = 4 # the longest signature the project wraps
# Constantes de matemática e de formato: repeti-las não duplica regra de negócio.
TRIVIAL = re.compile(r"^(HALF\w*|BYTE\w*|PI\w*|TAU\w*|RGBA\w*|CHANNELS|CORNERS|DEG_TO_RAD|RAD_TO_DEG|GOLDEN|NS_PER_\w+|QUARTER|"
                     r"\w*EPS\w*|DEGENERATE|FULL_TURN|\w*_SHIFT|\w*_BITS|\w*_MASK)$")


def arquivos(raiz, ext):
    for base, dirs, nomes in os.walk(raiz):
        dirs[:] = [d for d in dirs if d not in SKIP_DIRS]
        for n in nomes:
            if n.endswith(ext):
                yield os.path.join(base, n)


def ler(caminho):
    with open(caminho, encoding="utf-8", errors="replace") as f:
        return f.read()


def funcoes(texto):
    """(nome, linha inicial, corpo, const) de cada função definida com a chave na mesma linha do cabeçalho."""
    linhas = texto.splitlines()
    i = 0
    while i < len(linhas):
        cab, k = linhas[i], i # a signature may run over several lines: joined until its parentheses close
        while cab.count("(") > cab.count(")") and k + 1 < len(linhas) and k - i < SIGNATURE_LINES:
            k += 1
            cab += " " + linhas[k].strip()
        m = FUNC_HEAD.match(cab)
        if not m or m.group(1) in ("if", "for", "while", "switch", "catch"):
            i += 1
            continue
        nivel, j = 0, i
        while j < len(linhas):
            nivel += linhas[j].count("{") - linhas[j].count("}")
            if nivel <= 0 and j > i or (nivel == 0 and j == i and "}" in linhas[j]):
                break
            j += 1
        yield m.group(1), i + 1, "\n".join(linhas[i:j + 1]), bool(m.group(3))
        i = j + 1


def achado(regra, causa, efeito, onde, prova):
    return {"regra": regra, "causa": causa, "efeito": efeito, "onde": onde, "prova": prova}


def rel(raiz, p):
    return os.path.relpath(p, raiz).replace("\\", "/")


def travas_de_trabalho(raiz, fontes):
    """Uma leitura feita pela thread principal (api/) toma um mutex que uma thread de trabalho segura num laço."""
    out, laco = [], {}
    for p, t in fontes.items():
        if re.search(r"std::thread|while\s*\(\s*running", t):
            for m in re.finditer(r"lock_guard<std::mutex>\s+\w+\(\s*([\w.>-]+)\s*\)", t):
                laco.setdefault(m.group(1).split(".")[-1].split(">")[-1], set()).add(rel(raiz, p))
    for p, t in fontes.items():
        if "/api/" not in p.replace("\\", "/"):
            continue
        for nome, linha, corpo, _ in funcoes(t):
            for m in re.finditer(r"(?:lock_guard<std::mutex>\s+\w+\(|std::mutex\s*&\s*\w+)\s*([\w.>-]*)", corpo):
                mutex = m.group(1).split(".")[-1].split(">")[-1] or "(parâmetro)"
                if mutex in laco or (mutex == "(parâmetro)" and laco):
                    out.append(achado("TRAVA_DE_TRABALHO_NA_LEITURA",
                                      f"{nome} toma '{mutex}', que {', '.join(sorted(laco.get(mutex, laco[next(iter(laco))])))} segura no laço de trabalho",
                                      "a thread principal espera o passo inteiro da outra thread (picos de quadro, cliques lentos)",
                                      f"{rel(raiz, p)}:{linha}", "publicar o que a leitura precisa (atômico/ponteiro imutável) em vez de ler sob a trava"))
                    break
    return out


def realocacao_por_quadro(raiz, fontes):
    """Quem aloca o buffer de instâncias toda vez que a contagem muda, sem folga."""
    out = []
    for p, t in fontes.items():
        for nome, linha, corpo, _ in funcoes(t):
            if "allocate_data" not in corpo:
                continue
            exato = re.search(r"\b(\w*count\w*)\s*!=\s*(\w*capacity\w*)|\b(\w*capacity\w*)\s*!=\s*(\w*count\w*)", corpo)
            folga = re.search(r">\s*\w*capacity|capacity\w*\s*<|ROOM|room", corpo)
            if exato and not folga:
                out.append(achado("REALOCACAO_POR_QUADRO", f"{nome} realoca o buffer sempre que a contagem difere da capacidade",
                                  "buffer da GPU destruído e recriado a cada quadro em que a contagem muda (picos de GPU)",
                                  f"{rel(raiz, p)}:{linha}", f"'{exato.group(0)}' sem regra de folga no dono do buffer"))
    return out


def mapas_sem_poda(raiz, fontes):
    """Membro mapa/conjunto que recebe entradas e nunca as apaga: cresce com a sessão."""
    out = []
    for p, t in fontes.items():
        if not p.endswith((".h", ".hpp")):
            continue
        par = p[:-2] + ".cpp" if p.endswith(".h") else p[:-4] + ".cpp"
        corpo = t + (fontes.get(par, ""))
        for m in re.finditer(r"std::(?:unordered_map|map|unordered_set|set)<[^;]*>\s+(\w+_)\s*;", t):
            n = m.group(1)
            insere = re.search(rf"\b{n}\s*(\[|\.emplace|\.insert|\.try_emplace)", corpo)
            poda = re.search(rf"\b{n}\s*\.(erase|clear)\s*\(|\b{n}\s*=\s*", corpo)
            if insere and not poda:
                linha = t[:m.start()].count("\n") + 1
                out.append(achado("MAPA_SEM_PODA", f"'{n}' recebe entradas e nunca é podado", "memória que cresce sem limite durante a sessão",
                                  f"{rel(raiz, p)}:{linha}", "há inserção e nenhum erase/clear/atribuição"))
    return out


def threads_que_acordam(raiz, fontes):
    """Laço de thread que dorme por tempo mesmo sem trabalho (em pausa acorda à toa)."""
    out = []
    for p, t in fontes.items():
        for nome, linha, corpo, _ in funcoes(t):
            if re.search(r"while\s*\(", corpo) and re.search(r"sleep_for|sleep_until", corpo) and not re.search(r"\.wait\s*\(|wait_for|wait_until", corpo):
                out.append(achado("THREAD_ACORDA_SEM_TRABALHO", f"{nome} dorme por tempo num laço, sem esperar um evento",
                                  "acordadas periódicas sem trabalho (CPU e energia em pausa)", f"{rel(raiz, p)}:{linha}",
                                  "sleep_for/sleep_until num while, nenhuma condition_variable no laço"))
    return out


def constantes_duplicadas(raiz, fontes):
    """A mesma constante nomeada em vários arquivos: a regra tem mais de um dono."""
    donos = {}
    for p, t in fontes.items():
        for m in re.finditer(r"constexpr\s+[\w:<>]+\s+([A-Z][A-Z0-9_]{3,})\s*=\s*([^,;]+)", t):
            donos.setdefault(m.group(1), set()).add((rel(raiz, p), m.group(2).strip()))
    out = []
    for nome, lugares in sorted(donos.items()):
        arqs = sorted({a for a, _ in lugares})
        if len(arqs) < 2 or TRIVIAL.match(nome):
            continue
        valores = sorted({v for _, v in lugares})
        regra = "REGRA_DIVERGENTE" if len(valores) > 1 else "REGRA_SEM_DONO_UNICO"
        efeito = ("o mesmo nome vale coisas diferentes em cada arquivo" if len(valores) > 1
                  else "a regra copiada: mudar uma cópia deixa as outras para trás")
        out.append(achado(regra, f"'{nome}' definido em {len(arqs)} arquivos ({', '.join(valores)})", efeito, ", ".join(arqs), "constexpr repetido"))
    return out
