#!/usr/bin/env python3
"""Gera coverage.md: uma linha por arquivo e por declaracao propria de feature.md.
O que a declaracao faz vem do comentario dela no rastreamento; estado, dono e motivo vem de coverage_rules.json."""
import json, re, os, collections
ROOT = 'C:/Projects/Roadcraft-windows/converter/out/features/roads'
HERE = os.path.dirname(os.path.abspath(__file__))
b = json.load(open(f'{ROOT}/boundary.json', encoding='utf8'))
regras = json.load(open(f'{HERE}/coverage_rules.json', encoding='utf8'))
feat = open(f'{ROOT}/feature.md', encoding='utf8').read()
files = re.findall(r'^- (src/\S+) — (whole|part), (\d+) lines$', feat, re.M)

def doc_of(path, name):
    p = f'{ROOT}/source/{path}'
    if not os.path.exists(p): return ''
    lines = open(p, encoding='utf8').read().split('\n')
    leaf = name.split('.')[-1]
    pat = re.compile(r'(function|const|let|class|interface|type|enum|get|set)\s+' + re.escape(leaf) + r'\b|^\s*(private |public |protected |static |readonly |async )*' + re.escape(leaf) + r'\s*[(:<=?]')
    for i, l in enumerate(lines):
        if not pat.search(l): continue
        j = i - 1
        while j >= 0 and lines[j].strip().startswith('@'): j -= 1
        if j >= 0 and lines[j].strip().endswith('*/'):
            k = j
            while k >= 0 and '/**' not in lines[k] and j - k < 12: k -= 1
            txt = ' '.join(x.strip(' /*') for x in lines[k:j + 1])
            return re.sub(r'\s+', ' ', txt).strip()[:150]
        return ''
    return ''

def rule(path, name=None):
    r = regras['arquivos'].get(path)
    if r is None:
        for pre, v in regras['prefixos'].items():
            if path.startswith(pre): r = v; break
    r = dict(r or regras['padrao'])
    if name and name in r.get('decl', {}): r.update(r['decl'][name])
    return r

own = collections.defaultdict(list)
for o in b['own']:
    f, n = o.split('#', 1)
    own[f].append(n)

out = ['# Cobertura do rastreamento: roads', '',
       'Uma linha por arquivo e por declaração própria de `feature.md` (2.504 declarações, 280 arquivos). Gerado por `coverage.py` a partir de `coverage_rules.json`.',
       'Estados: **portado**, **já existia**, **não se aplica** (com o motivo), **não portado** (ainda falta, dito às claras) e **a portar** (antes de codificar).', '']
cont = collections.Counter(); contd = collections.Counter()
rows = []
for path, kind, n in files:
    r = rule(path); cont[r['estado']] += 1
    rows.append(f"\n## `{path}` ({'inteiro' if kind == 'whole' else 'parte'}, {n} linhas) — **{r['estado']}**\n\nDono: {r['dono']}. {r['motivo']}\n")
    if own.get(path):
        rows.append('| Declaração | O que faz | Estado | Dono no jogo |\n|---|---|---|---|')
        for name in sorted(own[path]):
            d = rule(path, name); contd[d['estado']] += 1
            txt = (d.get('faz') or doc_of(path, name) or '—').replace('|', '/')
            rows.append(f"| `{name}` | {txt} | {d['estado']} | {d['dono']} |")
extra = sorted(set(own) - {p for p, _, _ in files})
out.append('| Estado | Arquivos | Declarações próprias |\n|---|---|---|')
for k in sorted(set(cont) | set(contd)):
    out.append(f'| {k} | {cont[k]} | {contd[k]} |')
out.append(f'| total | {sum(cont.values())} | {sum(contd.values())} |')
if extra: out.append(f'\nArquivos com declaração própria fora da lista de código: {extra}')
open(f'{HERE}/coverage.md', 'w', encoding='utf8').write('\n'.join(out + rows) + '\n')
print(dict(cont), dict(contd), 'extra', len(extra))
