"""O que um caminho e um comando de shell são para o portão: código, arquivo protegido, escrita, execução do jogo, compilação.

A leitura do comando é heurística e o portão sabe disso: o que escapar daqui (um script que escreve por dentro) é pego depois pela
varredura do git (`varrer`), que olha o disco e não o texto do comando. O guia oficial de hooks recomenda exatamente isso, porque um
matcher `Edit|Write` não vê o que o shell escreve.
"""
import hashlib
import os
import re

import state as E

L = E.L
EXT_CODIGO = {".cpp", ".h", ".hpp", ".c", ".gd", ".gdshader", ".gdshaderinc", ".glsl", ".py", ".cjs", ".mjs", ".js", ".ts", ".ps1",
              ".sh", ".cmake", ".tscn", ".json", ".csv"}
RAIZES_CODIGO = re.compile(r"^(?:fonte/|ports/[^/]+/workspace/)(?:native|project|tools)/|^tools/|^converter/(?:scripts|src|test|configs)/")
FORA = re.compile(r"(?:^|/)(?:node_modules|out|\.godot|build-[\w-]+|godot-cpp|__pycache__)/")
# Where the gate lives, found from this file: its code and its state under its own root (tools/process, L.RAIZ) and the
# repository's `.claude/`; only this project's and the user's settings are the gate's concern.
REPO = os.path.abspath(os.path.join(L.RAIZ, "..", ".."))


def _path_pattern(root, *parts):
    """A path as a regex: its segments escaped, either slash between them, a drive written `C:` or `/c`."""
    segments = [p for p in os.path.abspath(root).replace("\\", "/").split("/") if p] + list(parts)
    drive = segments[0]
    start = rf"(?:{re.escape(drive)}|/{re.escape(drive[0])})" if re.fullmatch(r"[A-Za-z]:", drive) else re.escape(drive)
    return start + "".join(r"[/\\]" + re.escape(s) for s in segments[1:])


ABSOLUTOS = re.compile("(?:" + "|".join([_path_pattern(L.RAIZ, "tools", "process"), _path_pattern(L.RAIZ, "records", "process"),
                                         _path_pattern(REPO, ".claude") + r"[/\\]"])
                       + r"|Jonathan[/\\]\.claude[/\\](?:settings|skills[/\\]roadcraft)|Jonathan[/\\]bin[/\\]portao|~[/\\]\.claude[/\\]settings)", re.I)
PROTEGIDOS = re.compile(ABSOLUTOS.pattern + r"|tools[/\\]process[/\\]tools[/\\]process|tools[/\\]process[/\\]records[/\\]process|\.claude[/\\]settings", re.I)
SEGMENTO = re.compile(r"\|\||&&|\||;|\n")
# Proteger é negar escrita, não listar o que pode ler: uma lista de leitura negou grep, cd e Get-Content, e nem os juízes conseguiam ler.
ALTERA = re.compile(r"(?:^|[\s;&|(])(?:rm|rmdir|mv|cp|tee|truncate|chmod|sed\s+-i\S*|Remove-Item|Move-Item|Copy-Item|Rename-Item|Set-Content|Add-Content|Out-File|Clear-Content|git\s+(?:checkout|restore|rm|mv|reset|clean))(?:\s|$)", re.I)
LEITOR = re.compile(r"^\s*(?:cd\s+\S+\s*$|cat|head|tail|less|more|wc|grep|rg|egrep|fgrep|findstr|diff|cmp|file|stat|ls|dir|sha\w*sum|echo|printf|"
                    r"Get-Content|gc|Select-String|sls|Test-Path|sed\s+-n|awk|cut|sort|uniq|"
                    r"git(?:\s+-C\s+\S+)?\s+(?:diff|log|show|status|blame|ls-files|grep|add|commit|check-ignore|rev-parse))\b")
# The game or the tests run: Godot itself, the orchestration owner's runs (tools/tasks/cli.py run|serve), the build's game stages.
JOGO = re.compile(r"(?:^|[\s/\\\"'])godot(?:_console)?(?:\.exe)?\s+-|tools[/\\]tasks[/\\]cli\.py[\"']?\s+(?:run|serve)\b|build\.sh\s+(?:e2e|capture|export|package)|\.tscn\b|testar\.py|pytest|node\s+--test")
COMPILA = re.compile(r"build\.sh\s+(?:core|core-windows)\b|run_gates\.py|(?:^|\s)cmake\s|(?:^|\s)ninja\b")
HEREDOC = re.compile(r"<<-?\s*(['\"]?)(\w+)\1")
ASPAS = re.compile(r"\"(?:\\.|[^\"\\])*\"|'[^']*'")
REDIRECAO = re.compile(r"(?<![0-9&])>>?\s*([^\s|;&>]+)")
ESCRITORES = (re.compile(r"\btee\s+(?:-a\s+)?([^\s|;&]+)"), re.compile(r"\bsed\s+-i\S*\s+.*?\s([^\s|;&]+)\s*$"),
              re.compile(r"\b(?:cp|mv)\s+(?:-\w+\s+)*\S+\s+([^\s|;&]+)"),
              re.compile(r"\b(?:Set-Content|Add-Content|Out-File)\s+(?:-(?:Path|FilePath|LiteralPath)\s+)?([^\s|;&]+)", re.I))
CD = re.compile(r"^\s*(?:cd|Set-Location)\s+(\S+)\s*$")
ARQUIVO_CMD = re.compile(r"[\w:./\\~-]+\.(?:cmd|bat|sh|ps1)\b")
TOKEN_URL = re.compile(r"https?://[^\s\"'|;&<>)]+")
BAIXA = re.compile(r"(?:^|[\s;&|(])(?:curl|wget|Invoke-WebRequest|iwr)\b")


def relativo(caminho):
    return L.relativo(L.normalizar(caminho))


def eh_codigo(caminho):
    r = relativo(caminho)
    return bool(RAIZES_CODIGO.search(r)) and not FORA.search(r) and os.path.splitext(r)[1].lower() in EXT_CODIGO


def protegido(caminho):
    return bool(PROTEGIDOS.search(L.normalizar(caminho)))


def sem_heredocs(comando):
    """O corpo de um heredoc é dado, a menos que alimente um interpretador; a varredura pega o que um interpretador escrever."""
    saida, linhas, i = [], comando.split("\n"), 0
    while i < len(linhas):
        saida.append(linhas[i])
        m = HEREDOC.search(linhas[i])
        i += 1
        if m:
            while i < len(linhas) and linhas[i].strip() != m.group(2):
                i += 1
            i += 1
    return "\n".join(saida)


def segmentos(comando):
    return [s for s in SEGMENTO.split(ASPAS.sub('""', sem_heredocs(comando))) if s.strip()]


def partir(comando):
    """Os segmentos de um comando, sem cortar dentro de aspas: `grep "a\\|b" arquivo` é um segmento só."""
    saida, atual, aspa, i = [], [], "", 0
    while i < len(comando):
        c = comando[i]
        if aspa:
            aspa = "" if c == aspa else aspa
        elif c in "\"'":
            aspa = c
        elif c in ";\n|&":
            saida.append("".join(atual))
            atual = []
            i += 1
            continue
        atual.append(c)
        i += 1
    saida.append("".join(atual))
    return [s for s in saida if s.strip()]


def viola_protecao(comando):
    """Segmento que escreve num arquivo do portão. Ler nunca é negado; um programa que escreve por dentro cai na varredura."""
    for seg in partir(sem_heredocs(comando)):
        if not PROTEGIDOS.search(seg):
            continue
        if ALTERA.search(seg) or any(PROTEGIDOS.search(t) for t in REDIRECAO.findall(seg)):
            return seg.strip()[:160]
    return None


def escritas(comando, cwd):
    """Os arquivos que o comando escreve, com os `cd` dele resolvidos."""
    base, alvos = L.normalizar(cwd or L.RAIZ), []
    for seg in segmentos(comando):
        m = CD.match(seg)
        if m:
            base = L.normalizar(m.group(1), base)
            continue
        for rx in (REDIRECAO,) + ESCRITORES:
            for t in rx.findall(seg):
                if t.lower() not in ("/dev/null", "$null", "nul"):
                    alvos.append(L.normalizar(t, base))
    return alvos


def classe(comando):
    """"jogo" (executa o jogo ou os testes), "compila" (build, gates) ou None; segmento de leitura não conta."""
    achou = None
    for seg in segmentos(comando):
        if LEITOR.match(seg):
            continue
        if JOGO.search(seg):
            return "jogo"
        if COMPILA.search(seg):
            achou = "compila"
    return achou


def chave(comando, cwd):
    """O que torna uma execução a mesma: o comando sem espaços a mais e o conteúdo dos scripts que ele chama."""
    partes = [" ".join(comando.split())]
    for t in ARQUIVO_CMD.findall(comando):
        c = L.normalizar(t, L.normalizar(cwd or L.RAIZ))
        if os.path.isfile(c):
            partes.append(E.sha(c))
    return hashlib.sha256("\n".join(partes).encode("utf8")).hexdigest()[:20]


def urls_baixadas(comando):
    return TOKEN_URL.findall(comando) if BAIXA.search(comando) else []


def raiz_git(caminho):
    """O repositório de um caminho do projeto, subindo até achar `.git`: `converter/` e todo clone sem lista à mão."""
    c = L.normalizar(caminho)
    raiz = L.RAIZ.replace("\\", "/").lower()
    if not c.lower().startswith(raiz):
        return None
    while len(c) >= len(raiz):
        if os.path.exists(c + "/.git"):
            return c
        c = os.path.dirname(c)
    return None


def repos_de(est, cwd):
    """Os repositórios que a varredura olha: a raiz, o do cwd e os dos arquivos do plano."""
    achados = {L.RAIZ.replace("\\", "/")}
    for c in [cwd or ""] + est.get("plano", []):
        r = raiz_git(c) if c else None
        if r:
            achados.add(r)
    return sorted(achados)


def varrer(est, cwd):
    """Assinatura (data e tamanho) de todo arquivo de código alterado segundo o git, nos repositórios da sessão."""
    assinaturas = {}
    for repo in repos_de(est, cwd):
        for linha in E.git(["status", "--porcelain", "-uall"], repo).splitlines():
            if len(linha) < 4:
                continue
            caminho = L.normalizar(repo + "/" + linha[3:].split(" -> ")[-1].strip().strip('"'))
            if not eh_codigo(caminho):
                continue
            try:
                s = os.stat(caminho)
                assinaturas[caminho] = f"{s.st_mtime_ns}:{s.st_size}"
            except OSError:
                assinaturas[caminho] = "apagado"
    return assinaturas


def novidades(est, assinaturas):
    """O que mudou desde a última varredura; um arquivo que saiu do status foi commitado e não conta."""
    base = est.get("linha_base", {})
    return sorted(c for c, s in assinaturas.items() if base.get(c) != s)
