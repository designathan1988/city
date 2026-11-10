"""Renders a trace: entry chains, the feature's units with how each was reached, the state it mutates and who reads it."""
import json

KIND_ARROW = {'call': '->', 'write': '=>', 'read': '~>'}


def render_text(graph, result, top=None):
    label = graph.label
    lines = ['SEEDS'] + [f'  {label(s)}' for s in result.seeds]
    lines.append(f'\nUPSTREAM  {len(result.upstream)} callers, {len(result.entries)} entry points')
    lines.append(f'  ({len(result.hubs)} hubs where the walk up stops: they call far more than the graph norm)')
    for entry in (result.entries + result.hubs)[:top]:
        tag = '' if entry in result.entries else '[hub] '
        lines.append('  ' + tag + ' -> '.join(label(n) for n in result.chain_up(entry)))
    members = sorted(result.feature - set(result.seeds), key=lambda n: (len(result.chain_down(n)), n))
    lines.append(f'\nFEATURE  {len(members)} units  (-> calls, => writes, ~> reads what was written)')
    for node in members[:top]:
        steps = result.chain_down(node)
        text = label(steps[0][0])
        for unit, kind in steps[1:]:
            text += f' {KIND_ARROW.get(kind, "->")} {label(unit)}'
        lines.append('  ' + text)
    lines.append(f'\nMUTATED STATE  {len(result.mutated)}')
    lines += [f'  {label(d)}' for d in sorted(result.mutated)[:top]]
    lines.append(f'\nDEPENDENTS (read the mutated state)  {len(result.dependents)}')
    lines += [f'  {label(d)}' for d in sorted(result.dependents)[:top]]
    lines.append(f'\nCONSUMERS (outside the run, read the most mutated state; the entry that runs each)  {len(result.consumers)}')
    for consumer in result.consumers[:top]:
        read = sorted(graph.nodes[d].name for d in result.consumer_reads[consumer])
        lines.append('  ' + ' -> '.join(label(n) for n in result.consumer_up[consumer]))
        lines.append(f'      reads {len(read)}: ' + ', '.join(read[:12]) + (' ...' if len(read) > 12 else ''))
    lines.append(f'\nINFRASTRUCTURE (called from far more places than the graph norm)  {len(result.shared)}')
    lines += [f'  {label(d)}  callers {result.prevalence.get(d, 0)}' for d in sorted(result.shared)[:top]]
    lines.append(f'\nEXECUTED THROUGH INFRASTRUCTURE (runs in the calls; its writes are mutated state)  {len(result.executed)}')
    lines += [f'  {label(d)}' for d in sorted(result.executed)[:top]]
    return '\n'.join(lines)


def to_json(graph, result):
    def node(n):
        info = graph.nodes[n]
        return {'id': n, 'label': graph.label(n), 'kind': info.kind, 'path': info.path, 'line': info.start}
    return {
        'seeds': [node(s) for s in result.seeds],
        'entries': [{'entry': node(e), 'chain': [node(n) for n in result.chain_up(e)]} for e in result.entries],
        'feature': [{'node': node(n), 'chain': [[node(u), k] for u, k in result.chain_down(n)]} for n in sorted(result.feature)],
        'mutated': [node(n) for n in sorted(result.mutated)],
        'dependents': [node(n) for n in sorted(result.dependents)],
        'inputs': [node(n) for n in sorted(result.inputs)],
        'shared': [dict(node(n), prevalence=result.prevalence.get(n, 0)) for n in sorted(result.shared)],
        'executed': [node(n) for n in sorted(result.executed)],
        'consumers': [{'consumer': node(c), 'reads': [node(d) for d in sorted(result.consumer_reads[c])],
                       'entry_chain': [node(n) for n in result.consumer_up[c]]} for c in result.consumers],
    }


def dump(graph, result, path):
    with open(path, 'w', encoding='utf-8') as handle:
        json.dump(to_json(graph, result), handle, indent=1)
