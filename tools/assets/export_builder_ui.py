#!/usr/bin/env python3
"""The Build tab's icons and texts, from the Roadcraft 4.0 source: project/ui/icons/b_<tool>.svg and data/i18n/strings_d.csv and strings_e.csv.

usage: export_builder_ui.py <roadcraft 4.0 root>
Icons are the glyphs of src/ui/builder/icons.ts, one per tool of src/ui/builder/catalog.ts. Texts are the keys under PREFIXES
in src/ui/i18n, in both languages, that the other parts of the catalogue do not hold yet.
"""
import csv
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ICONS = os.path.join(ROOT, 'project', 'ui', 'icons')
I18N = os.path.join(ROOT, 'project', 'data', 'i18n')
PREFIXES = ('builder.', 'building.finish.', 'building.material.', 'building.problem.', 'creator.dock.scope.', 'creator.pattern.',
            'hint.builder.')
SVG = ('<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24" fill="none" stroke="#ffffff" '
       'stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">%s</svg>\n')
ENTRY = re.compile(r"^\s*'?([A-Za-z_.]+)'?:\s*'((?:[^'\\]|\\.)*)',\s*$", re.M)
KEY = re.compile(r"^\s*'([A-Za-z_.0-9]+)':\s*(['\"`])((?:\\.|(?!\2).)*)\2,\s*$", re.M)


def block(text, start):
    a = text.index(start)
    return text[a:text.index('\n};', a)]


def export_icons(source):
    text = open(os.path.join(source, 'src/ui/builder/icons.ts'), encoding='utf8').read()
    glyphs = dict(ENTRY.findall(block(text, 'const P = {')))
    aliases = dict(ENTRY.findall(block(text, 'const TAB_ICON')))
    catalog = open(os.path.join(source, 'src/ui/builder/catalog.ts'), encoding='utf8').read()
    tools = sorted(set(re.findall(r"(?:mode|action)\('([A-Za-z_]+)'\)", catalog)))
    missing = []
    for tool in tools:
        glyph = glyphs.get(aliases.get(tool, tool))
        if glyph is None:
            missing.append(tool)
            continue
        with open(os.path.join(ICONS, 'b_%s.svg' % tool), 'w', encoding='utf8', newline='\n') as f:
            f.write(SVG % glyph)
    print('icons', len(tools) - len(missing), 'without a glyph of their own:', missing)


def texts(path):
    text = open(path, encoding='utf8').read()
    return {k: v.replace("\\'", "'") for k, _, v in KEY.findall(text) if k.startswith(PREFIXES)}


def export_strings(source):
    pt = texts(os.path.join(source, 'src/ui/i18n/pt-BR.ts'))
    en = texts(os.path.join(source, 'src/ui/i18n/en.ts'))
    held = set()
    for part in ('strings.csv', 'strings_b.csv', 'strings_c.csv'):
        with open(os.path.join(I18N, part), encoding='utf8', newline='') as f:
            held.update(row[0] for row in csv.reader(f) if row)
    with open(os.path.join(I18N, 'strings.csv'), encoding='utf8', newline='') as f:
        header = next(csv.reader(f))
    keys = sorted(k for k in pt if k not in held)
    half = (len(keys) + 1) // 2 # two parts: a file of the catalogue stays under the size limit
    for part, chunk in (('strings_d.csv', keys[:half]), ('strings_e.csv', keys[half:])):
        with open(os.path.join(I18N, part), 'w', encoding='utf8', newline='') as f:
            w = csv.writer(f, lineterminator='\n')
            w.writerow(header)
            for k in chunk:
                w.writerow([k, pt[k], en.get(k, pt[k])])
    print('texts', len(keys), 'without English:', [k for k in keys if k not in en][:8])


if __name__ == '__main__':
    export_icons(sys.argv[1])
    export_strings(sys.argv[1])
