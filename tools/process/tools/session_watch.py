"""Vigia das sessões de porte: a cada 2 minutos lê o histórico de todas as filhas e o estado dos clones.

Cada ciclo imprime um bloco (uma linha por sessão) e os alertas: sessão parada, regra quebrada, memória no limite.
A primeira leitura vai do início de cada histórico, então também audita o que já foi feito.
Uso: python -u session_watch.py [--uma-vez]
"""
import json
import os
import re
import subprocess
import sys
import time
from datetime import datetime, timezone
from pathlib import Path

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'converter', 'scripts'))
from machine_config import machine  # noqa: E402

# The game (this repository), the converter and the port folders, from the converter's machine file.
GAME_ROOT = Path(machine()['game'])
CONVERTER = Path(machine()['converter'])
PORTS = Path(machine()['ports'])
PROJETOS = Path.home() / '.claude' / 'projects'
LOG = GAME_ROOT / 'dist' / 'process' / 'session_watch.log'
INTERVALO_S = 120
PARADA_S = 600
MEMORIA_LIMITE = 92
RESUMO = 90

# Nome da sessão -> (clone onde ela abriu, funcionalidades dela na ordem).
SESSOES = {
    'roads': ('roads', ['roads']),
    'build': ('build', ['build']),
    'landscape': ('landscape', ['landscape']),
    'A': ('interface', ['interface', 'demolition', 'economy']),
    'B': ('traffic', ['traffic', 'street-furniture', 'people']),
    'C': ('road-elevation', ['road-elevation', 'transit', 'destruction', 'landmarks']),
}

REGRAS_COMANDO = [
    (re.compile(r'--map=[1-4]\b'), 'mapa proibido (só o mapa 0)'),
    (re.compile(r'\bgit\s+push\b'), 'push (filha não dá push)'),
    (re.compile(r'(?:\s-j\s*|JOBS=)([5-9]|\d\d)\b'), 'compilação acima de -j 4'),
]
# Only what runs counts: reading a script or naming the game in a text opens no window. The game runs hidden only
# through the repository's orchestration owner (tools/tasks/cli.py run|serve), which puts every task on its private desktop.
JOGO = re.compile(r'(godot(_console)?(\.exe)?|\$\{?GODOT\}?)["\']?\s+(--path|--rendering|res://|-e\b)'
                  r'|build\.sh["\']?\s+(e2e|capture)\b', re.I)
OCULTO = re.compile(r'tools[\\/]tasks[\\/]cli\.py["\']?\s+(run|serve)\b', re.I)


def _key(path):
    """A path as one comparable text: absolute, forward slashes, lower case (Windows paths ignore case)."""
    return os.path.abspath(str(path)).replace('\\', '/').lower().rstrip('/') + '/'


def outside_clone(path):
    """An edit of the game or the converter themselves, not of a port's clone (<ports>/<feature>/workspace)."""
    if not path:
        return False
    key = _key(path)
    in_clone = re.match(re.escape(_key(PORTS)) + r'[^/]+/workspace/', key) is not None
    return not in_clone and (key.startswith(_key(GAME_ROOT)) or key.startswith(_key(CONVERTER)))


SUBAGENTES = {'Agent', 'Task', 'Workflow'}


def rodar(args, cwd=None):
    r = subprocess.run(args, cwd=cwd, capture_output=True, text=True, encoding='utf8', errors='replace')
    return r.stdout.strip()


def historico(clone):
    # Claude Code names a project's history folder after its path, every ':', backslash and slash a '-'
    pasta = PROJETOS / re.sub(r'[:\\/]', '-', str(PORTS / clone / 'workspace'))
    arquivos = list(pasta.glob('*.jsonl')) if pasta.is_dir() else []
    return max(arquivos, key=lambda p: p.stat().st_mtime) if arquivos else None


def resumo_acao(nome, entrada):
    if nome in ('Bash', 'PowerShell'):
        texto = entrada.get('description') or entrada.get('command') or ''
    elif 'file_path' in entrada:
        texto = '/'.join(re.split(r'[\\/]', entrada['file_path'])[-3:])
    else:
        texto = ''
    return f'{nome}: {" ".join(str(texto).split())[:RESUMO]}'


def violacoes(nome, entrada):
    achadas = []
    if nome in SUBAGENTES:
        achadas.append(f'subagente ({nome})')
    if nome == 'AskUserQuestion':
        achadas.append('perguntou ao usuário')
    if nome in ('Edit', 'Write') and outside_clone(entrada.get('file_path', '')):
        achadas.append(f'editou fora do clone: {entrada["file_path"]}')
    if nome in ('Bash', 'PowerShell'):
        comando = entrada.get('command', '')
        achadas += [texto for regra, texto in REGRAS_COMANDO if regra.search(comando)]
        if JOGO.search(comando) and not OCULTO.search(comando) and not re.search(r'--headless|--version|--help', comando):
            achadas.append('game run outside tools/tasks')
    return achadas


class Sessao:
    def __init__(self, nome, clone, funcionalidades):
        self.nome, self.clone, self.funcionalidades = nome, clone, funcionalidades
        self.arquivo, self.lido = None, 0
        self.ultima, self.quando, self.novas = 'sem histórico', None, 0
        self.avisados = set()

    def ler(self):
        """Lê o que o histórico ganhou desde o ciclo anterior; devolve os alertas novos."""
        atual = historico(self.clone)
        if atual is None:
            return []
        if atual != self.arquivo:
            self.arquivo, self.lido = atual, 0
        with open(atual, 'rb') as f:
            f.seek(self.lido)
            dados = f.read()
        fim = dados.rfind(b'\n') + 1
        self.lido += fim
        self.novas = 0
        alertas = []
        for linha in dados[:fim].splitlines():
            try:
                item = json.loads(linha)
            except ValueError:
                continue
            carimbo = item.get('timestamp')
            if carimbo:
                self.quando = datetime.fromisoformat(carimbo.replace('Z', '+00:00'))
            if item.get('type') != 'assistant':
                continue
            conteudo = (item.get('message') or {}).get('content')
            for bloco in conteudo if isinstance(conteudo, list) else []:
                if bloco.get('type') != 'tool_use':
                    continue
                nome, entrada = bloco.get('name', ''), bloco.get('input') or {}
                self.novas += 1
                self.ultima = resumo_acao(nome, entrada)
                for texto in violacoes(nome, entrada):
                    chave = (texto, json.dumps(entrada, sort_keys=True)[:200])
                    if chave not in self.avisados:
                        self.avisados.add(chave)
                        alertas.append(f'ALERTA {self.nome}: {texto} | {self.ultima}')
        return alertas

    def parada_ha(self):
        if self.quando is None:
            return None
        return (datetime.now(timezone.utc) - self.quando).total_seconds()


def estado_clone(funcionalidade):
    porta = PORTS / funcionalidade
    clone = porta / 'workspace'
    if not (clone / '.git').exists():
        return f'{funcionalidade} sem clone', []
    base = json.loads((porta / 'workspace.json').read_text(encoding='utf8'))['base']
    commits = rodar(['git', 'rev-list', '--count', f'{base}..HEAD'], clone)
    sujos = len(rodar(['git', 'status', '--short'], clone).splitlines())
    relatorio = 'relatório' if (porta / 'report.md').exists() else ''
    ramo = rodar(['git', 'branch', '--show-current'], clone)
    alertas = [] if ramo == f'convert/{funcionalidade}' else [f'ALERTA {funcionalidade}: branch {ramo}']
    return f'{funcionalidade} +{commits}c {sujos}s {relatorio}'.strip(), alertas


def maquina():
    script = ("$o=Get-CimInstance Win32_OperatingSystem;"
              "$m=[math]::Round(100*(1-$o.FreePhysicalMemory/$o.TotalVisibleMemorySize));"
              "$c=(Get-CimInstance Win32_Processor|Measure-Object LoadPercentage -Average).Average;"
              "$g=@(Get-Process godot* -ErrorAction SilentlyContinue).Count;\"$m $c $g\"")
    partes = rodar(['powershell', '-NoProfile', '-Command', script]).split()
    return [int(float(p)) for p in partes] if len(partes) == 3 else [0, 0, 0]


def ciclo(sessoes):
    alertas = []
    linhas = []
    for s in sessoes:
        alertas += s.ler()
        clones = []
        for funcionalidade in s.funcionalidades:
            texto, erros = estado_clone(funcionalidade)
            clones.append(texto)
            alertas += erros
        parada = s.parada_ha()
        if parada is None:
            situacao = 'sem atividade'
        else:
            situacao = f'última atividade há {int(parada)} s'
            if parada >= PARADA_S:
                alertas.append(f'ALERTA {s.nome}: parada há {int(parada // 60)} min')
        linhas.append(f'{s.nome}: {situacao}, +{s.novas} ações | {s.ultima} | {"; ".join(clones)}')
    memoria, cpu, jogos = maquina()
    if memoria >= MEMORIA_LIMITE:
        alertas.append(f'ALERTA memória em {memoria} %')
    fonte = [l for l in rodar(['git', 'status', '--short'], GAME_ROOT).splitlines() if not l.endswith('.uid')]
    if fonte:
        alertas.append(f'ALERTA fonte sujo: {len(fonte)} arquivos')
    if rodar(['git', 'status', '--short'], CONVERTER):
        alertas.append('ALERTA converter sujo')
    topo = f'[{datetime.now():%H:%M}] memória {memoria} %, CPU {cpu} %, jogos abertos {jogos}'
    return '\n'.join([topo, *linhas, *alertas])


def main():
    sys.stdout.reconfigure(encoding='utf-8')
    LOG.parent.mkdir(parents=True, exist_ok=True)
    sessoes = [Sessao(nome, clone, funcs) for nome, (clone, funcs) in SESSOES.items()]
    while True:
        bloco = ciclo(sessoes)
        print(bloco, flush=True)
        with open(LOG, 'a', encoding='utf8') as f:
            f.write(bloco + '\n')
        if '--uma-vez' in sys.argv:
            return
        time.sleep(INTERVALO_S)


if __name__ == '__main__':
    main()
