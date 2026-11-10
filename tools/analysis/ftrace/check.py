"""Properties every trace must hold, checked against the graph and the raw source text; no names are expected.

1. grounding: each step of a chain is an edge of the graph of the kind the step claims, and the edge's name is written
   on the edge's line of the source file;
2. roles: a name recorded as called is followed by '(' on its line (or handed as an argument); a name recorded as
   written is followed by an assignment or increment operator on its line;
3. data closure: every unit reading data the feature writes is a dependent;
4. control closure: every unit a member calls is a member or shared, and every unit called by a unit that runs (member,
   shared or executed) is shared or executed itself;
5. mutation: every datum a running unit writes is in the mutated state;
6. entries: no entry point has a caller, and every caller's chain ends at a seed;
7. consumers: each reads mutated state, runs outside the feature, and its entry chain is made of `before` steps from a
   unit nothing makes run."""
import os
import re

from .roles import ASSIGNMENT_OPERATORS, INCREMENT_OPERATORS
from .trace import FLOW, READ, WRITE, _before

ASSIGNED = '|'.join(re.escape(o) for o in sorted(ASSIGNMENT_OPERATORS | INCREMENT_OPERATORS, key=len, reverse=True))
# What may stand between a written name and its assignment operator: members (.x, ->x), subscripts ([i]), a declared
# size or a type annotation (p[4], x: int), closing brackets and further targets (a, b = ...); never a comparison.
SUBSCRIPT = r'\[(?:[^\[\]]|\[[^\[\]]*\])*\]'
TARGET_TAIL = r'\b(?:\s*' + SUBSCRIPT + r'|[\)\w\s.:,*&-])*?'


def violations(graph, result, root):
    lines_of = _source_lines(root)
    found = []
    for node in result.feature:
        steps = result.chain_down(node)
        for (unit, _), (following, kind) in zip(steps, steps[1:]):
            if kind == READ:
                edge = graph.out[following].get(unit)
                ok = edge is not None and READ in edge.kinds
                owner = following
            else:
                edge = graph.out[unit].get(following)
                ok = edge is not None and (edge.kinds & FLOW if kind == 'call' else WRITE in edge.kinds)
                owner = unit
            if not ok:
                found.append(f'grounding: no {kind} edge for {graph.label(unit)} -> {graph.label(following)}')
            elif not _written(lines_of, edge.where or graph.nodes[owner].path, edge.line, edge.name):
                found.append(f'grounding: {edge.name!r} not on {edge.where or graph.nodes[owner].path}:{edge.line}')
    for node in result.feature:
        for role, name, line in graph.nodes[node].tokens:
            text = _line(lines_of, graph.nodes[node].path, line)
            if text is None:
                continue
            if role == 'call' and not re.search(r'\b' + re.escape(name) + r'\b\s*(<[^()]*>)?\s*\(', text):
                found.append(f'roles: {name!r} recorded as called, no call on {graph.nodes[node].path}:{line}')
            if role == 'write' and not re.search(r'\b' + re.escape(name) + TARGET_TAIL + r'(' + ASSIGNED + r')(?!=)', text) \
                    and not re.search(r'(\+\+|--)\s*' + re.escape(name), text):
                found.append(f'roles: {name!r} recorded as written, no assignment on {graph.nodes[node].path}:{line}')
    for datum in result.mutated:
        for reader, edge in graph.inn[datum].items():
            if READ in edge.kinds and reader not in result.dependents:
                found.append(f'data closure: {graph.label(reader)} reads {graph.label(datum)}, not a dependent')
    running = result.feature | result.shared | result.executed
    for node in result.shared | result.executed:
        for target, edge in graph.out[node].items():
            if 'call' in edge.kinds and graph.nodes[target].kind != 'data' and target not in running:
                found.append(f'control closure: {graph.label(node)} runs {graph.label(target)}, which is not executed')
    for node in running:
        for target, edge in graph.out[node].items():
            if node not in result.feature:
                if WRITE in edge.kinds and graph.nodes[target].kind == 'data' and target not in result.mutated:
                    found.append(f'mutation: {graph.label(node)} writes {graph.label(target)}, not in the mutated state')
                continue
            if edge.kinds & FLOW and graph.nodes[target].kind != 'data' and target not in result.feature \
                    and target not in result.shared:
                found.append(f'control closure: {graph.label(node)} calls {graph.label(target)}, neither member nor shared')
            if WRITE in edge.kinds and graph.nodes[target].kind == 'data' and target not in result.mutated:
                found.append(f'mutation: {graph.label(node)} writes {graph.label(target)}, not in the mutated state')
    for entry in result.entries:
        if _before(graph, entry):
            found.append(f'entries: {graph.label(entry)} has a caller')
    running = result.feature | result.shared | result.executed
    for consumer in result.consumers:
        if consumer in running or not result.consumer_reads.get(consumer) \
                or not result.consumer_reads[consumer] <= result.mutated:
            found.append(f'consumers: {graph.label(consumer)} is not an outside reader of mutated state')
        chain = result.consumer_up.get(consumer, [])
        if not chain or chain[-1] != consumer:
            found.append(f'consumers: the entry chain of {graph.label(consumer)} does not end at it')
        elif len(chain) > 1 and _before(graph, chain[0]):
            found.append(f'consumers: the chain of {graph.label(consumer)} starts at {graph.label(chain[0])}, which has a caller')
        for unit, following in zip(chain, chain[1:]):
            if unit not in _before(graph, following):
                found.append(f'consumers: {graph.label(unit)} does not make {graph.label(following)} run')
    for caller in result.upstream:
        if result.chain_up(caller)[-1] not in result.seeds:
            found.append(f'entries: chain of {graph.label(caller)} does not end at a seed')
    return found


def _source_lines(root):
    cache = {}

    def lines(path):
        if path not in cache:
            try:
                with open(os.path.join(root, path), encoding='utf-8', errors='replace', newline='') as handle:
                    cache[path] = handle.read().split('\n')
            except OSError:
                cache[path] = None
        return cache[path]
    return lines


def _line(lines_of, path, line):
    lines = lines_of(path)
    if lines is None or not 1 <= line <= len(lines):
        return None
    return lines[line - 1]


def _written(lines_of, path, line, name):
    text = _line(lines_of, path, line)
    return text is not None and name in text
