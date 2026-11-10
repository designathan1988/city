#!/usr/bin/env python3
"""Gera coverage.md: uma linha por arquivo e por declaracao propria de feature.md, no formato que o converter le
(src/convert/inventory.ts readCoverage): a primeira celula e o caminho entre crases (`src/x.ts` ou `src/x.ts#nome`), a linha de um
arquivo inteiro diz `whole` na segunda celula, o estado e uma das celulas seguintes e o dono e a celula entre crases.
O que a declaracao faz vem do comentario dela no rastreamento; estado, dono e motivo vem de coverage_rules.json."""
import collections
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(HERE, '..', '..', 'converter', 'scripts'))
from machine_config import machine  # noqa: E402

# the feature's extraction, in the converter's out/ (the converter's folder from the machine file)
ROOT = f"{machine()['converter']}/out/features/road-elevation"
b = json.load(open(f'{ROOT}/boundary.json', encoding='utf8'))
regras = json.load(open(f'{HERE}/coverage_rules.json', encoding='utf8'))
feat = open(f'{ROOT}/feature.md', encoding='utf8').read()
files = re.findall(r'^- (src/\S+) — (whole|part), (\d+) lines$', feat, re.M)


def doc_of(path, name):
    p = f'{ROOT}/source/{path}'
    if not os.path.exists(p):
        return ''
    lines = open(p, encoding='utf8').read().split('\n')
    leaf = name.split('.')[-1]
    pat = re.compile(r'(function|const|let|class|interface|type|enum|get|set)\s+' + re.escape(leaf) + r'\b|^\s*(private |public |protected |static |readonly |async )*' + re.escape(leaf) + r'\s*[(:<=?]')
    for i, l in enumerate(lines):
        if not pat.search(l):
            continue
        j = i - 1
        while j >= 0 and lines[j].strip().startswith('@'):
            j -= 1
        if j >= 0 and lines[j].strip().endswith('*/'):
            k = j
            while k >= 0 and '/**' not in lines[k] and j - k < 12:
                k -= 1
            txt = ' '.join(x.strip(' /*') for x in lines[k:j + 1])
            return re.sub(r'\s+', ' ', txt).strip()[:150]
        return ''
    return ''


def rule(path, name=None):
    r = regras['arquivos'].get(path)
    if r is None:
        for pre, v in regras['prefixos'].items():
            if path.startswith(pre):
                r = v
                break
    r = dict(r or regras['padrao'])
    if name and name in r.get('decl', {}):
        r.update(r['decl'][name])
    return r


def cell(text):
    return (text or '—').replace('|', '/')


def owner(d):
    return f"`{d['dono']}`" if d.get('dono') else '—'


own = collections.defaultdict(list)
for o in b['own']:
    f, n = o.split('#', 1)
    own[f].append(n)

cont, contd, rows = collections.Counter(), collections.Counter(), []
for path, kind, n in files:
    r = rule(path)
    cont[r['estado']] += 1
    rows.append(f"\n## `{path}` ({'inteiro' if kind == 'whole' else 'parte'}, {n} linhas)\n")
    rows.append('| Fonte | Alcance | Estado | Dono no jogo | Motivo |\n|---|---|---|---|---|')
    rows.append(f"| `{path}` | {kind} | {r['estado']} | {owner(r)} | {cell(r.get('motivo'))} |")
    if own.get(path):
        rows.append('\n| Declaração | O que faz | Estado | Dono no jogo |\n|---|---|---|---|')
        for name in sorted(own[path]):
            d = rule(path, name)
            contd[d['estado']] += 1
            rows.append(f"| `{path}#{name}` | {cell(d.get('faz') or doc_of(path, name))} | {d['estado']} | {owner(d)} |")
extra = sorted(set(own) - {p for p, _, _ in files})
out = ['# Cobertura do rastreamento: road-elevation', '',
       f'Uma linha por arquivo e por declaração própria de `feature.md` ({sum(len(v) for v in own.values())} declarações, {len(files)} arquivos), '
       'no formato que o converter lê (`src/convert/inventory.ts`). Gerado por `coverage.py` a partir de `coverage_rules.json`.',
       'Estados: **portado**, **já existia**, **não se aplica** (com o motivo), **não portado** (ainda falta, dito às claras) e **a portar**.', '',
       '| Estado | Arquivos | Declarações próprias |', '|---|---|---|']
for k in sorted(set(cont) | set(contd)):
    out.append(f'| {k} | {cont[k]} | {contd[k]} |')
out.append(f'| total | {sum(cont.values())} | {sum(contd.values())} |')
if extra:
    out.append(f'\nArquivos com declaração própria fora da lista de código: {extra}')
open(f'{HERE}/coverage.md', 'w', encoding='utf8').write('\n'.join(out + rows) + '\n')
print(dict(cont), dict(contd), 'extra', len(extra))
