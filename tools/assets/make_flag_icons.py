#!/usr/bin/env python3
"""The flag cards of the Build > Roof tab: project/ui/icons/b_flag_<pattern>.svg, one per design of data/ui/builder.json
(the Roadcraft's flags.ts FLAG_PATTERNS, drawn by building_parts.gdshader flag_colour), plus none and the three colour
fields. Each is the cloth seen flat, the second colour's regions filled white, in the glyph style of export_builder_ui.py.

usage: make_flag_icons.py
"""
import os

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
ICONS = os.path.join(ROOT, 'project', 'ui', 'icons')
SVG = ('<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24" fill="none" stroke="#ffffff" '
       'stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">%s</svg>\n')
# The cloth: hoist at x = 6, top at y = 5, 15 wide, 11 tall.
X0, Y0, W, H = 6, 5, 15, 11
X1, Y1 = X0 + W, Y0 + H
MAST = '<path d="M4 2.5v19"/>'
CLOTH = '<rect x="%d" y="%d" width="%d" height="%d"/>' % (X0, Y0, W, H)
FILL = '<rect x="%.1f" y="%.1f" width="%.1f" height="%.1f" fill="#ffffff" stroke="none"/>'


def box(x, y, w, h):
    return FILL % (x, y, w, h)


def poly(points):
    return '<polygon points="%s" fill="#ffffff" stroke="none"/>' % ' '.join('%.1f,%.1f' % p for p in points)


PATTERNS = {
    'none': '<path d="M%d %d %d %d"/>' % (X0, Y1, X1, Y0),
    'solid': box(X0, Y0, W, H),
    'bicolourH': box(X0, Y0 + H / 2, W, H / 2),
    'bicolourV': box(X0 + W / 2, Y0, W / 2, H),
    'tricolourH': box(X0, Y0 + H / 3, W, H / 3),
    'tricolourV': box(X0 + W / 3, Y0, W / 3, H),
    'cross': box(X0 + W * 0.41, Y0, W * 0.18, H) + box(X0, Y0 + H * 0.38, W, H * 0.24),
    'nordic': box(X0 + W * 0.29, Y0, W * 0.14, H) + box(X0, Y0 + H * 0.4, W, H * 0.2),
    'diagonal': poly([(X0, Y0), (X1, Y0), (X0, Y1)]),
    'saltire': poly([(X0, Y0), (X0 + 2, Y0), (X1, Y1 - 2), (X1, Y1), (X1 - 2, Y1), (X0, Y0 + 2)]) +
               poly([(X1, Y0), (X1, Y0 + 2), (X0 + 2, Y1), (X0, Y1), (X0, Y1 - 2), (X1 - 2, Y0)]),
    'canton': box(X0, Y0, W * 0.42, H / 2),
    'stripes': box(X0, Y0, W * 0.42, H * 0.54) + box(X0 + W * 0.42, Y0 + H * 0.15, W * 0.58, H * 0.15) +
               box(X0 + W * 0.42, Y0 + H * 0.46, W * 0.58, H * 0.15) + box(X0, Y0 + H * 0.77, W, H * 0.15),
    'border': box(X0 + W * 0.1, Y0 + H * 0.15, W * 0.8, H * 0.7),
    'disc': '<ellipse cx="%.1f" cy="%.1f" rx="%.1f" ry="%.1f" fill="#ffffff" stroke="none"/>' % (X0 + W / 2, Y0 + H / 2, W * 0.2, H * 0.3),
    'chevron': poly([(X0, Y0), (X0 + W / 2, Y0 + H / 2), (X0, Y1)]),
}
# The colour fields: the whole cloth, the right half, the lower third (where a tricolour's second and third colours go).
FIELDS = [box(X0, Y0, W, H), box(X0 + W / 2, Y0, W / 2, H), box(X0, Y0 + H * 2 / 3, W, H / 3)]


def write(name, glyph):
    with open(os.path.join(ICONS, 'b_flag_%s.svg' % name), 'w', encoding='utf8', newline='\n') as fh:
        fh.write(SVG % (MAST + glyph + CLOTH))


def main():
    for name, glyph in PATTERNS.items():
        write(name, glyph)
    for k, glyph in enumerate(FIELDS):
        write('field%d' % k, glyph)
    print('%d ícones em %s' % (len(PATTERNS) + len(FIELDS), ICONS))


if __name__ == '__main__':
    main()
