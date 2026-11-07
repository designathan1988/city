# rename_english.py <repo> [--apply | --check]: every Portuguese file and folder name under the trees below put in English
# (one map, below), with `git mv` from the leaves up, every reference to an old name rewritten where it is a path (beside a
# slash or backslash, in quotes, or a file name with its extension), never in prose, and every Python `import` of a renamed
# module (a module's name is its file's stem) rewritten through the parser. Without a flag: the plan only. --check: every
# import of every Python file in the renamed trees resolves (exit 1 if one does not, or if there is no Python file).
import ast
import importlib.util
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(sys.argv[1])
APPLY = "--apply" in sys.argv
CHECK = "--check" in sys.argv
# The trees whose names are put in English, by their old names (the renamed trees are their names through english()).
TREES = ("tools/processo", "tools/portes")
# A git command of the rename (a listing, a move); past this its process is stopped.
GIT_TIMEOUT_S = 600
# Larger files are data (photos, recordings), never text with references.
MAX_TEXT_BYTES = 20_000_000
WORDS = {
    "processo": "process", "portes": "ports", "ferramentas": "tools", "registro": "records", "leitura": "reading",
    "mapa40": "map40", "revisado": "reviewed", "verificado": "verified", "ciclos": "cycles", "conversor": "converter",
    "pacote-extrator": "extractor-package", "vegetacao-terreno": "vegetation-terrain",
    "funcionalidades-4.0": "features-4.0", "arquivos-inteiros": "whole-files", "rastreadas": "traced",
    "sem-rastreamento": "untraced", "analise": "analysis", "analise-pessoas": "people-analysis",
    "converter-antigo": "converter-old", "portao": "gate", "portao-ver": "gate-check", "integracao": "integration",
    "elevacao": "elevation", "especificacoes": "specifications", "fontes": "sources", "fotos-jogo": "photos-game",
    "fotos-oclusao": "photos-occlusion", "marcos": "landmarks", "paridade": "parity",
    "roadcraft-leitura": "roadcraft-reading", "roadcraft-porte": "roadcraft-port",
}
FILES = {
    "01-arvore-do-4.0.txt": "01-tree-of-the-4.0.txt", "02-canais-dinamicos-reais.txt": "02-real-dynamic-channels.txt",
    "03-exemplos-reais-do-4.0.txt": "03-real-examples-of-the-4.0.txt", "04-extrator-atual.txt": "04-current-extractor.txt",
    "05-funcionalidades-e-grafo.txt": "05-features-and-graph.txt", "GUIA-PORTE-GODOT.md": "GODOT-PORT-GUIDE.md",
    "INDICE-fotos.md": "PHOTOS-INDEX.md", "LEIA-ME.txt": "READ-ME.txt", "adendo-final.md": "addendum-final.md",
    "adendo-onda2.md": "addendum-wave2.md", "adendo-onda3.md": "addendum-wave3.md", "analise.md": "analysis.md",
    "c2-sessao-real.log": "c2-real-session.log", "c2-stop-prova.log": "c2-stop-proof.log", "c2-testar.log": "c2-test.log",
    "calcada-analise.md": "sidewalk-analysis.md", "ciclo2-convert-road-elevation.log": "cycle2-convert-road-elevation.log",
    "ciclo2-testes.log": "cycle2-tests.log", "ciclo2-typecheck.log": "cycle2-typecheck.log", "cobertura.md": "coverage.md",
    "cobertura.py": "coverage.py", "cobertura_regras.json": "coverage_rules.json", "comandos.py": "commands.py",
    "conferencia.json": "check.json", "constantes.json": "constants.json", "contrato-porte6.md": "contract-port6.md",
    "conversas.py": "conversations.py", "coordenacao.md": "coordination.md", "despacho_gates.py": "gates_dispatch.py",
    "diagnostico-claude-code.md": "claude-code-diagnosis.md", "elementos.md": "elements.md", "estado.json": "state.json",
    "estado.py": "state.py", "fila.md": "queue.md", "fonte-settings.proposto.json": "source-settings.proposed.json",
    "fotos-4.0-lista-rodada-4.md": "photos-4.0-list-round-4.md",
    "fotos-4.0-lista-rodada-5.md": "photos-4.0-list-round-5.md", "fotos-4.0.md": "photos-4.0.md",
    "fotos-lista-par.md": "photos-list-pair.md", "fotos-lista.md": "photos-list.md", "fotos.md": "photos.md",
    "fotos_analise.md": "photos_analysis.md", "funcionalidades.json": "features.json",
    "funcionalidades.cjs": "features.cjs", "extrair_funcionalidade.cjs": "extract_feature.cjs",
    "extrair_funcionalidade_cpp.cjs": "extract_feature_cpp.cjs", "grafo.json": "graph.json",
    "hooks-e-travas.md": "hooks-and-locks.md", "indice.md": "index.md", "infraestrutura.md": "infrastructure.md",
    "inventario.json": "inventory.json", "inventario.py": "inventory.py", "leitura.md": "reading.md",
    "leitura_lib.py": "reading_lib.py", "levantamento.md": "survey.md", "lista.md": "list.md", "lista2.md": "list2.md",
    "look-noite.json": "look-night.json", "look-pessoas.json": "look-people.json",
    "look-pessoas2.json": "look-people2.json", "look-pessoas3.json": "look-people3.json",
    "look-tudo.json": "look-all.json", "look-vias.json": "look-roads.json", "look-vias2.json": "look-roads2.json",
    "look-vias3.json": "look-roads3.json", "look-vias4.json": "look-roads4.json", "medir_vista.py": "measure_view.py",
    "obrigatorios.json": "required.json", "paleta-fontes.md": "palette-sources.md",
    "pares-analise.md": "pairs-analysis.md", "pares-lista.md": "pairs-list.md",
    "paridade-defeitos.md": "parity-defects.md", "passos.py": "steps.py",
    "pendencias-coordenadora.md": "coordinator-pending.md", "pesquisa.md": "research.md", "pessoas.md": "people.md",
    "plano-mobiliario.md": "plan-furniture.md", "plano.md": "plan.md", "poses-nomes.json": "poses-names.json",
    "processo.py": "process.py", "prompt-coordenadora.md": "coordinator-prompt.md",
    "prompt-extrator-funcionalidade.md": "feature-extractor-prompt.md", "regras-sessoes.md": "session-rules.md",
    "relatorio.md": "report.md", "resposta-par.md": "pair-answer.md", "roads-luz.md": "roads-light.md",
    "rodada-final-lista.md": "final-round-list.md", "sessoes_conversor.txt": "converter_sessions.txt",
    "sessoes_coordenador.txt": "coordinator_sessions.txt", "simbolos.py": "symbols.py", "testar.py": "test.py",
    "testes.log": "tests.log", "vegetacao-terreno.md": "vegetation-terrain.md", "vigia_mensagens.py": "message_watch.py",
    "vigia_sessoes.py": "session_watch.py", "grafo.cjs": "graph.cjs", "01_campo_meia.jpg": "01_field_half.jpg",
    "02_borda_mato.jpg": "02_scrub_edge.jpg", "03_mato_perto.jpg": "03_scrub_near.jpg",
    "04_mato_perto_zoom.png": "04_scrub_near_zoom.png", "05_jardins.jpg": "05_gardens.jpg",
    "06_jardins_zoom.png": "06_gardens_zoom.png", "07_centro_arvores_rua.jpg": "07_downtown_street_trees.jpg",
    "08_parque_dia.jpg": "08_park_day.jpg", "09_parque_noite.jpg": "09_park_night.jpg",
    "10_parque_chuva.jpg": "10_park_rain.jpg", "11_cidade_longe.jpg": "11_city_far.jpg",
    "12_horizonte_20graus.jpg": "12_horizon_20degrees.jpg", "13_morro_lago.jpg": "13_hill_lake.jpg",
    "14_lago_perto.jpg": "14_lake_near.jpg", "15_penhasco.jpg": "15_cliff.jpg",
    "16_penhasco_noite.jpg": "16_cliff_night.jpg", "n_forward_plus_perto.png": "n_forward_plus_near.png",
    "n_gl_compatibility_perto.png": "n_gl_compatibility_near.png", "w2_forward_plus_perto.png": "w2_forward_plus_near.png",
    "w2_gl_compatibility_perto.png": "w2_gl_compatibility_near.png",
}
PLAN = {"arvores": "trees", "ceu": "sky", "cubo": "cube", "dinheiro": "money", "luz": "light", "luz2": "light2", "luz3": "light3",
        "mar": "sea", "noite": "night", "ordem": "order", "poses": "poses", "rampa": "ramp", "referencia": "reference",
        "referencia2": "reference2", "rio": "river", "soltas": "loose", "sombra": "shadow"}


def english(name: str) -> str:
    if name in WORDS:
        return WORDS[name]
    if name in FILES:
        return FILES[name]
    m = re.fullmatch(r"plano-([a-z0-9]+)\.json", name)
    if m and m.group(1) in PLAN:
        return f"plan-{PLAN[m.group(1)]}.json"
    m = re.fullmatch(r"ciclo2-passo(\d)\.json", name)
    if m:
        return f"cycle2-step{m.group(1)}.json"
    m = re.fullmatch(r"passo(\d)\.json", name)
    if m:
        return f"step{m.group(1)}.json"
    m = re.fullmatch(r"lista-rodada-(\d)\.md", name)
    if m:
        return f"list-round-{m.group(1)}.md"
    m = re.fullmatch(r"fotos-(par|luz|t|v)(\d+)(-.*)?", name)
    if m:
        kind = {"par": "pair", "luz": "light", "t": "t", "v": "v"}[m.group(1)]
        tail = {"-dll-antiga": "-old-dll", "-sem-efeito": "-no-effect", "-luz": "-light", None: ""}[m.group(3)]
        return f"photos-{kind}{m.group(2)}{tail}"
    m = re.fullmatch(r"look-par(\d+)\.json", name)
    if m:
        return f"look-pair{m.group(1)}.json"
    m = re.fullmatch(r"par(\d+)", name)
    if m:
        return f"pair{m.group(1)}"
    return name


def git(*args: str) -> str:
    return subprocess.run(["git", "-C", str(ROOT), *args], capture_output=True, text=True, check=True, timeout=GIT_TIMEOUT_S).stdout


def renamed_path(p: str) -> str:
    return "/".join(english(part) for part in p.split("/"))


# ---- Python modules: a module's name is its file's stem, so a renamed .py file renames every `import` of it. Imports are
# found by the parser (ast), never by text: `import comandos as C`, `from comandos import x`, `from . import comandos`.

def module_renames(renamed: dict) -> dict:
    return {Path(old).stem: Path(new).stem for old, new in renamed.items() if old.endswith(".py") and Path(old).stem != Path(new).stem}


def _from_module_span(lines: list, node: ast.ImportFrom) -> tuple:
    """Where `from <module>` names its module's first part: (line index, start column, end column)."""
    line = lines[node.lineno - 1]
    m = re.compile(r"from\s+\.*").match(line, node.col_offset)
    first = node.module.split(".")[0]
    return node.lineno - 1, m.end(), m.end() + len(first)


def rename_imports(text: str, modules: dict) -> str:
    """The text with every import of a renamed module naming its new name; what the parser cannot read is left as it is."""
    try:
        tree = ast.parse(text)
    except SyntaxError:
        return text
    lines = text.split("\n")
    spans = []
    for node in ast.walk(tree):
        if isinstance(node, ast.Import):
            for alias in node.names:
                first = alias.name.split(".")[0]
                if first in modules:
                    spans.append((alias.lineno - 1, alias.col_offset, alias.col_offset + len(first), modules[first]))
        elif isinstance(node, ast.ImportFrom):
            if node.module and node.module.split(".")[0] in modules:
                row, start, end = _from_module_span(lines, node)
                spans.append((row, start, end, modules[node.module.split(".")[0]]))
            if not node.module and node.level > 0:
                for alias in node.names:
                    if alias.name in modules:
                        spans.append((alias.lineno - 1, alias.col_offset, alias.col_offset + len(alias.name), modules[alias.name]))
    for row, start, end, new in sorted(spans, reverse=True):
        lines[row] = lines[row][:start] + new + lines[row][end:]
    return "\n".join(lines)


# ---- the check: every import of every Python file in the renamed trees resolves, without running any of them.

def _static_value(node, env: dict, file: Path):
    """The value of a path expression a script builds for sys.path (constants, __file__, os.path and Path calls), or None."""
    if isinstance(node, ast.Constant) and isinstance(node.value, str):
        return node.value
    if isinstance(node, ast.Name):
        return str(file) if node.id == "__file__" else env.get(node.id)
    if isinstance(node, ast.JoinedStr):
        parts = [_static_value(v.value if isinstance(v, ast.FormattedValue) else v, env, file) for v in node.values]
        return None if None in parts else "".join(parts)
    if isinstance(node, ast.BinOp) and isinstance(node.op, (ast.Div, ast.Add)):
        left, right = _static_value(node.left, env, file), _static_value(node.right, env, file)
        if left is None or right is None:
            return None
        return os.path.join(left, right) if isinstance(node.op, ast.Div) else left + right
    if isinstance(node, ast.Attribute) and node.attr == "parent":
        inner = _static_value(node.value, env, file)
        return None if inner is None else os.path.dirname(inner)
    if isinstance(node, ast.Call):
        name = ast.unparse(node.func)
        args = [_static_value(a, env, file) for a in node.args]
        if None in args:
            return None
        if name in ("os.path.join",):
            return os.path.join(*args)
        if name in ("os.path.dirname",):
            return os.path.dirname(args[0])
        if name == "str" and len(args) == 1:
            return args[0]
        if name in ("os.path.abspath", "os.path.realpath", "os.path.normpath", "Path", "pathlib.Path"):
            return os.path.abspath(args[0]) if args else None
        if isinstance(node.func, ast.Attribute) and node.func.attr in ("resolve", "absolute") and not node.args:
            inner = _static_value(node.func.value, env, file)
            return None if inner is None else os.path.abspath(inner)
    return None


def _search_path(tree: ast.Module, file: Path) -> list:
    """The folders a file's imports are looked up in: its own, then each it puts on sys.path (insert or append)."""
    env, dirs = {}, [str(file.parent)]
    for node in ast.walk(tree):
        if isinstance(node, ast.Assign) and len(node.targets) == 1 and isinstance(node.targets[0], ast.Name):
            value = _static_value(node.value, env, file)
            if value is not None:
                env[node.targets[0].id] = value
        if isinstance(node, ast.Call) and ast.unparse(node.func) in ("sys.path.insert", "sys.path.append") and node.args:
            value = _static_value(node.args[-1], env, file)
            if value is not None:
                dirs.append(os.path.abspath(value))
    return dirs


def _resolves(name: str, dirs: list) -> bool:
    if name in sys.stdlib_module_names or name in sys.builtin_module_names:
        return True
    for d in dirs:
        if os.path.isfile(os.path.join(d, f"{name}.py")) or os.path.isfile(os.path.join(d, name, "__init__.py")):
            return True
    return importlib.util.find_spec(name) is not None


def check(trees: list) -> int:
    """Exit code 0 only when the trees hold Python files and every import in them resolves."""
    files = [ROOT / f for f in git("ls-files", "--", *trees).splitlines() if f.endswith(".py")]
    files += [ROOT / f for f in git("ls-files", "--others", "--exclude-standard", "--", *trees).splitlines() if f.endswith(".py")]
    files = [f for f in files if f.is_file()]  # a tracked file deleted in the working tree is no longer the tree's
    if not files:
        print(f"check: no Python file in {', '.join(trees)}")
        return 1
    broken = []
    for file in files:
        tree = ast.parse(file.read_text(encoding="utf-8"), str(file))
        dirs = _search_path(tree, file)
        for node in ast.walk(tree):
            if isinstance(node, ast.Import):
                names = [a.name.split(".")[0] for a in node.names]
            elif isinstance(node, ast.ImportFrom) and node.level == 0 and node.module:
                names = [node.module.split(".")[0]]
            elif isinstance(node, ast.ImportFrom) and node.level > 0:
                base = file.parent
                for _ in range(node.level - 1):
                    base = base.parent
                names = [] if node.module and _resolves(node.module.split(".")[0], [str(base)]) else \
                    [] if not node.module and all(_resolves(a.name, [str(base)]) for a in node.names) else [f".{node.module or ''}"]
                if names:
                    broken.append(f"{file.relative_to(ROOT).as_posix()}:{node.lineno}: relative import {names[0]}")
                continue
            else:
                continue
            for name in names:
                if not _resolves(name, dirs):
                    broken.append(f"{file.relative_to(ROOT).as_posix()}:{node.lineno}: import {name}")
    for b in broken:
        print(f"  {b}")
    print(f"check: {len(files)} Python files, {len(broken)} imports that do not resolve")
    return 1 if broken else 0


if CHECK:
    sys.exit(check(sorted({renamed_path(t) for t in TREES} | {t for t in TREES if (ROOT / t).exists()})))

files = git("ls-files", *TREES).splitlines()
# Every path (files and their folders) whose own name changes, deepest first.
paths = set()
for f in files:
    parts = f.split("/")
    for i in range(1, len(parts) + 1):
        paths.add("/".join(parts[:i]))
moves = sorted((p for p in paths if english(p.split("/")[-1]) != p.split("/")[-1]), key=lambda p: -p.count("/"))
# The references are rewritten for every old name the last commit holds, not only for what is still to move: a run that
# moved part of the tree before stopping leaves the rest of its names to rewrite.
committed = git("ls-tree", "-r", "--name-only", "HEAD", *TREES).splitlines()
renamed = {c: english(c) for f in committed for c in f.split("/") if english(c) != c}
modules = module_renames(renamed)
print(f"{len(moves)} paths renamed, {len(renamed)} distinct names, {len(modules)} Python modules")
for old, new in sorted(renamed.items()):
    print(f"  {old} -> {new}")
if not APPLY:
    sys.exit(0)
for p in moves:
    parent, name = p.rsplit("/", 1) if "/" in p else ("", p)
    target = f"{parent}/{english(name)}" if parent else english(name)
    git("mv", p, target)
# References: a name where it is a path, never in prose.
TEXT = {".md", ".py", ".mjs", ".cjs", ".js", ".ts", ".json", ".jsonl", ".sh", ".ps1", ".txt", ".gd", ".cfg", ".toml", ".yml", ".yaml", ""}
# One pass a file: every name in one alternation (longest first), the replacement looked up. A folder name counts beside a
# slash or backslash or alone in quotes; a file name (it has its extension) wherever it stands as a whole name.
by_length = lambda names: "|".join(re.escape(n) for n in sorted(names, key=len, reverse=True))
folders = by_length(n for n in renamed if "." not in n)
files_ = by_length(n for n in renamed if "." in n)
rx = re.compile(rf"(?<=[/\\])(?P<a>{folders})(?![\w.-])|(?<![\w.-])(?P<b>{folders})(?=[/\\])|(?<=['\"`])(?P<c>{folders})(?=['\"`])"
                rf"|(?<![\w.-])(?P<d>{files_})(?![\w-])")


def swap(m: re.Match) -> str:
    name = m.group(m.lastgroup)
    # tools/analise is another session's folder, not moved here: its references are left alone.
    if name == "analise" and m.string[max(0, m.start() - 6):m.start()] in ("tools/", "tools\\"):
        return name
    return renamed[name]


# Other sessions' folders are not read: they are theirs to rename. This script is not read either: its maps are the old
# names, and rewriting them made every pair an identity (the run of ad200882).
OTHERS = ("tools/converter/", "tools/oracle-4.0/", "tools/analise/", "tools/analysis/")
SELF = Path(__file__).resolve()
changed = 0
for f in git("ls-files").splitlines():
    path = ROOT / f
    if f.startswith(OTHERS) or path.suffix not in TEXT or not path.is_file() or path.stat().st_size > MAX_TEXT_BYTES or path.resolve() == SELF:
        continue
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        continue
    new_text = rename_imports(text, modules) if path.suffix == ".py" else text
    new_text = rx.sub(swap, new_text)
    if new_text != text:
        path.write_text(new_text, encoding="utf-8", newline="")
        changed += 1
print(f"{changed} files with references rewritten")
sys.exit(check([renamed_path(t) for t in TREES]))
