"""Answers made to be looked at: one HTML file that opens with no server (its data written in it), holding the graph
(functions, state, outputs; calls, writes, reads, handlers registered and fired, each with its condition) to go
through, the timeline of a sequence, the bands of a sweep (where each output happens, in two programs: the gaps and
overlaps seen), the values of outputs on a plane (the axes chosen among them), sheets and costs side by side; and the
graph in the DOT format.
"""
from __future__ import annotations

import html
import json

from .decide import render
from .vocab import FIRES, HANDED, OUTCALL, READS, WRITE, WRITES, CHANGES

GRAPH, TIMELINE, BANDS, PLANE, SIDE = "the graph", "the timeline", "the bands", "the plane", "side by side"
WORDS = "as text"                # a view that is the answer as it is written


def graph(result, units=None) -> dict:
    """The graph: nodes (functions, state, outputs), edges (calls, writes, reads, handlers), with conditions."""
    p, nodes, edges = result.program, {}, []

    def node(key, label, kind, where=""):
        nodes.setdefault(key, {"id": key, "label": label, "kind": kind, "where": where})
    for src, links in result.out.items():
        if units is not None and src not in units:
            continue
        node(f"f{src}", p.label(src), "function", p.where(src))
        for link in links:
            if units is not None and link.dst not in units:
                continue
            node(f"f{link.dst}", p.label(link.dst), "function", p.where(link.dst))
            edges.append({"from": f"f{src}", "to": f"f{link.dst}", "kind": link.kind,
                          "where": f"{p.files[link.file].display}:{link.line}"})
    for unit, touched in (result.state_out or {}).items():
        if units is not None and unit not in units:
            continue
        node(f"f{unit}", p.label(unit), "function", p.where(unit))
        for d, how in touched.items():
            node(f"s{d}", p.label(d), "state", p.where(d))
            if how in (WRITES, CHANGES):
                edges.append({"from": f"f{unit}", "to": f"s{d}", "kind": how, "where": ""})
            elif how == READS:
                edges.append({"from": f"s{d}", "to": f"f{unit}", "kind": how, "where": ""})
    for e in (result.cases.effects if result.cases else []):
        if e.kind != OUTCALL or (units is not None and e.unit not in units):
            continue
        key = f"o{e.name}"
        node(key, e.name, "output")
        edges.append({"from": f"f{e.unit}", "to": key, "kind": "calls out",
                      "where": f"{p.files[e.file].display}:{e.line}",
                      "when": render(e.guard)[:160] if e.guard is not True else ""})
    return {"nodes": list(nodes.values()), "edges": edges}


def dot(view: dict) -> str:
    """The graph in the DOT format."""
    def q(s) -> str:
        return "\"" + str(s).replace("\\", "\\\\").replace("\"", "\\\"") + "\""
    lines = ["digraph tracer {"]
    for n in view["nodes"]:
        lines.append(f"  {q(n['id'])} [label={q(n['label'])}, shape={q('box' if n['kind'] == 'function' else 'ellipse')}];")
    for e in view["edges"]:
        label = e["kind"] + (f" when {e['when']}" if e.get("when") else "")
        lines.append(f"  {q(e['from'])} -> {q(e['to'])} [label={q(label)}];")
    lines.append("}")
    return "\n".join(lines)


def timeline(run) -> list:
    """A sequence's steps: each start, its outputs in order with their values."""
    from .evaluate import plain
    return [{"start": label, "outputs": [{"where": o.where, "callee": o.callee,
                                           "values": [plain(a) for a in o.args]} for o in outputs]}
            for label, outputs, _ in run.steps]


def bands(done) -> dict:
    """A sweep compared: for each output of each program, the values of the variable where it happens."""
    from .compare import outputs_of
    one, two = done.sides
    name = done.sweep[0] if done.sweep else ""
    rows = {}
    for situation, _, run_one, run_two in done.rows:
        at = situation.get(name, 0)
        for tag, s, run in (("first", one, run_one), ("second", two, run_two)):
            for callee, values, _ in outputs_of(s, run):
                rows.setdefault(f"{tag}: {callee}", []).append(at)
    xs = [situation.get(name, 0) for situation, *_ in done.rows]
    return {"over": name, "xs": xs, "rows": rows}


def plane_view(x: tuple, y: tuple, runs: list) -> dict:
    """The plane's view: the axes' names and the points."""
    return {"x": f"{x[0]}[{x[1]}]", "y": f"{y[0]}[{y[1]}]", "points": plane(runs, x, y)}


def plane(runs: list, x: tuple, y: tuple) -> list:
    """Points of output values: each run, the value of argument x[1] of output x[0] against y's."""
    from .evaluate import plain
    points = []
    for run in runs:
        got = {}
        for o in run.outputs:
            got.setdefault(o.callee, [plain(a) for a in o.args])
        try:
            points.append([float(got[x[0]][x[1]]), float(got[y[0]][y[1]])])
        except (KeyError, IndexError, TypeError, ValueError):
            continue
    return points


PAGE = """<!doctype html>
<html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>%TITLE%</title>
<style>
:root { --bg:#fff; --fg:#1d1d1f; --muted:#6e6e73; --line:#d2d2d7; --a:#0a64d8; --b:#c2410c; --c:#15803d; }
@media (prefers-color-scheme: dark) { :root { --bg:#111214; --fg:#ececf1; --muted:#9a9aa3; --line:#2c2d31;
  --a:#5aa2ff; --b:#fb923c; --c:#4ade80; } }
body { background:var(--bg); color:var(--fg); font:14px/1.45 system-ui, sans-serif; margin:0 auto; max-width:1100px;
  padding:16px; }
h1 { font-size:20px; } h2 { font-size:16px; margin-top:28px; border-bottom:1px solid var(--line); }
.muted { color:var(--muted); } input { padding:4px 8px; width:280px; }
table { border-collapse:collapse; width:100%; } td, th { border-bottom:1px solid var(--line); padding:4px 6px;
  text-align:left; vertical-align:top; font-size:13px; }
.node { cursor:pointer; color:var(--a); } .cols { display:grid; grid-template-columns:1fr 1fr; gap:16px; }
pre { white-space:pre-wrap; font-size:12px; } svg text { fill:var(--fg); font-size:11px; }
</style></head><body>
<h1>%TITLE%</h1><div id="views"></div>
<script id="data" type="application/json">%DATA%</script>
<script>
const D = JSON.parse(document.getElementById('data').textContent);
const root = document.getElementById('views');
const esc = s => String(s).replace(/[&<>"]/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;'}[c]));
function section(title) { const h = document.createElement('h2'); h.textContent = title; root.appendChild(h);
  const d = document.createElement('div'); root.appendChild(d); return d; }
function graphView(g, box) {
  box.innerHTML = '<input placeholder="find a node" id="find"> <span class="muted">click a node: what it touches</span>' +
    '<div id="list"></div><div id="detail"></div>';
  const byId = Object.fromEntries(g.nodes.map(n => [n.id, n]));
  const show = () => { const q = box.querySelector('#find').value.toLowerCase();
    box.querySelector('#list').innerHTML = g.nodes.filter(n => n.label.toLowerCase().includes(q)).slice(0, 300)
      .map(n => `<span class="node" data-id="${esc(n.id)}">${esc(n.label)}</span> <span class="muted">${esc(n.kind)}</span>`)
      .join('<br>');
    box.querySelectorAll('.node').forEach(el => el.onclick = () => detail(el.dataset.id)); };
  const detail = id => { const n = byId[id];
    const out = g.edges.filter(e => e.from === id), into = g.edges.filter(e => e.to === id);
    const row = (e, other) => `<tr><td>${esc(e.kind)}</td><td><span class="node" data-id="${esc(other)}">` +
      `${esc((byId[other]||{}).label||other)}</span></td><td>${esc(e.where||'')}</td><td>${esc(e.when||'')}</td></tr>`;
    box.querySelector('#detail').innerHTML = `<h3>${esc(n.label)} <span class="muted">${esc(n.where||'')}</span></h3>` +
      '<table><tr><th>out</th><th></th><th>where</th><th>when</th></tr>' + out.map(e => row(e, e.to)).join('') +
      '<tr><th>in</th><th></th><th></th><th></th></tr>' + into.map(e => row(e, e.from)).join('') + '</table>';
    box.querySelectorAll('#detail .node').forEach(el => el.onclick = () => detail(el.dataset.id)); };
  box.querySelector('#find').oninput = show; show(); }
function timelineView(steps, box) {
  box.innerHTML = steps.map((s, i) => `<h3>${i + 1}. ${esc(s.start)}</h3><table>` + s.outputs.map(o =>
    `<tr><td>${esc(o.where)}</td><td>${esc(o.callee)}</td><td>${esc(JSON.stringify(o.values))}</td></tr>`).join('') +
    '</table>').join(''); }
function bandsView(b, box) {
  const names = Object.keys(b.rows), w = 900, h = 22 * names.length + 30, lo = Math.min(...b.xs), hi = Math.max(...b.xs);
  const X = v => 220 + (hi === lo ? 0 : (v - lo) / (hi - lo)) * (w - 240), step = (w - 240) / Math.max(1, b.xs.length - 1);
  let s = `<svg viewBox="0 0 ${w} ${h}" width="100%">`;
  names.forEach((name, i) => { const y = 10 + i * 22;
    s += `<text x="4" y="${y + 13}">${esc(name)}</text>`;
    b.rows[name].forEach(v => { s += `<rect x="${X(v) - step / 2}" y="${y}" width="${Math.max(step, 2)}" height="16"
      fill="${name.startsWith('first') ? 'var(--a)' : 'var(--b)'}" opacity="0.75"></rect>`; }); });
  s += `<text x="220" y="${h - 4}">${esc(b.over)} = ${lo}</text><text x="${w - 120}" y="${h - 4}">${hi}</text></svg>`;
  box.innerHTML = s; }
function planeView(p, box) {
  const xs = p.points.map(q => q[0]), ys = p.points.map(q => q[1]);
  const [x0, x1, y0, y1] = [Math.min(...xs), Math.max(...xs), Math.min(...ys), Math.max(...ys)], w = 600, h = 400;
  const X = v => 40 + (x1 === x0 ? 0.5 : (v - x0) / (x1 - x0)) * (w - 60), Y = v => h - 30 - (y1 === y0 ? 0.5 :
    (v - y0) / (y1 - y0)) * (h - 50);
  box.innerHTML = `<svg viewBox="0 0 ${w} ${h}" width="100%">` + p.points.map(q => `<circle cx="${X(q[0])}"
    cy="${Y(q[1])}" r="3" fill="var(--c)"></circle>`).join('') + `<text x="40" y="${h - 8}">${esc(p.x)}</text>` +
    `<text x="4" y="14">${esc(p.y)}</text></svg>`; }
function sideView(s, box) {
  box.innerHTML = '<div class="cols">' + s.map(col => `<div><h3>${esc(col.title)}</h3><pre>${esc(col.text)}</pre></div>`)
    .join('') + '</div>'; }
for (const v of D.views) { const box = section(v.title);
  if (v.kind === 'the graph') graphView(v.data, box); else if (v.kind === 'the timeline') timelineView(v.data, box);
  else if (v.kind === 'the bands') bandsView(v.data, box); else if (v.kind === 'the plane') planeView(v.data, box);
  else if (v.kind === 'side by side') sideView(v.data, box);
  else box.innerHTML = `<pre>${esc(typeof v.data === 'string' ? v.data : JSON.stringify(v.data, null, 1))}</pre>`; }
</script></body></html>
"""


def page(title: str, views: list) -> str:
    """One HTML file: each view (kind, title, its data); a text view where the kind is none of the drawn ones."""
    data = json.dumps({"views": [{"kind": kind, "title": name, "data": held} for kind, name, held in views]},
                      default=str).replace("</", "<\\/")
    return PAGE.replace("%TITLE%", html.escape(title)).replace("%DATA%", data)
