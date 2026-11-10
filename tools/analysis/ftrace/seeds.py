"""Chooses trace seeds: from explicit specs, or from a free-text query ranked by tf-idf and cut at the largest gap.

Ranking follows the textual half of hybrid feature location (Dora; SNIAFL): each unit is two documents, its name and
its body (identifiers, strings, comments, split into words), each compared with the query by tf-idf cosine. The name
document is the qualified name (scope and name; a file's path), since a method is known by its container as much as
by its own name, with its ordered word pairs, so that word order counts ('config get' is Config::get, not getConfig).
The two fields are not added: what a unit is (its name) outranks what it uses (its body), the principle behind field
weighting in retrieval, applied without a tuned weight as a lexicographic order: name similarity first, body
similarity only to break ties. The seed set is everything above the largest drop between consecutive scores (SNIAFL's
gap threshold) on the deciding field, so no tuned cut-off decides it."""
import math
from collections import Counter

from .words import split_words

BEHAVIOUR = ('callable', 'container')


def gap_cut(values):
    """How many leading values of a best-first list to keep: everything before the largest jump."""
    if len(values) < 2:
        return len(values)
    jumps = [abs(values[i] - values[i + 1]) for i in range(len(values) - 1)]
    return jumps.index(max(jumps)) + 1


def rank_by_query(graph, query):
    """[(node id, name similarity, body similarity)] for units sharing a word with the query, best first."""
    terms = Counter(_with_bigrams(split_words(query)))
    names = [Counter(_with_bigrams(_qualified_words(n))) for n in graph.nodes]
    bodies = []
    for node in graph.nodes:
        body = Counter()
        for _, text, _ in node.tokens:
            body.update(split_words(text))
        bodies.append(body)
    name_scores, body_scores = _cosines(names, terms), _cosines(bodies, terms)
    ranked = [(i, name_scores[i], body_scores[i]) for i in range(len(graph.nodes))
              if name_scores[i] > 0 or body_scores[i] > 0]
    return sorted(ranked, key=lambda r: (-r[1], -r[2], r[0]))


def select_by_query(graph, query):
    """Seeds: among units that relate to something (a seed with no edge cannot be traced) and mention every query
    word in their qualified name or body (boolean AND, when any unit does), preferring behaviour (callables and
    containers: a feature is something the program does) over data, those above the largest gap of the deciding
    field: the name when any candidate's name matches, else the body."""
    terms = set(split_words(query))
    ranked = [r for r in rank_by_query(graph, query) if graph.out[r[0]] or graph.inn[r[0]]]
    pool = [r for r in ranked if terms <= _words(graph.nodes[r[0]])] or ranked
    pool = [r for r in pool if graph.nodes[r[0]].kind in BEHAVIOUR] or pool
    named = [r for r in pool if r[1] > 0]
    if named:
        return [r[0] for r in named[:gap_cut([r[1] for r in named])]]
    pool = sorted(pool, key=lambda r: (-r[2], r[0]))
    return [r[0] for r in pool[:gap_cut([r[2] for r in pool])]]


def select_by_specs(graph, specs):
    found = []
    for spec in specs:
        found.extend(graph.find(spec))
    return list(dict.fromkeys(found))


def _cosines(docs, terms):
    frequency = Counter()
    for doc in docs:
        frequency.update(doc.keys())
    idf = {t: math.log(len(docs) / f) for t, f in frequency.items()}
    query_vector = {t: c * idf[t] for t, c in terms.items() if idf.get(t)}
    query_norm = math.sqrt(sum(v * v for v in query_vector.values()))
    scores = [0.0] * len(docs)
    if not query_norm:
        return scores
    for i, doc in enumerate(docs):
        dot = sum(w * doc[t] * idf[t] for t, w in query_vector.items() if t in doc)
        if dot:
            scores[i] = dot / (query_norm * math.sqrt(sum((c * idf[t]) ** 2 for t, c in doc.items())))
    return scores


def _qualified_words(node):
    return split_words(node.path if node.kind == 'file' else ' '.join(node.scope + (node.name,)))


def _with_bigrams(words):
    return words + [f'{a} {b}' for a, b in zip(words, words[1:])]


def _words(node):
    found = set(_qualified_words(node))
    for _, text, _ in node.tokens:
        found.update(split_words(text))
    return found
