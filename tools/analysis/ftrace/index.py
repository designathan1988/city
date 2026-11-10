"""Indexes a source tree: picks a grammar per extension, then extracts every text file, in parallel, reusing results by content.

Parsing runs native grammar code. A worker that dies (a crash inside a grammar) fails the whole run at once, naming
the files it was given; a phase that overruns its deadline has its workers killed. Nothing is waited on blindly."""
import concurrent.futures
import hashlib
import multiprocessing
import os
import pickle
from collections import defaultdict, namedtuple

import tree_sitter

from . import extract, grammars, sources

# Bump when extraction output changes: cached facts from older extractors are then ignored.
EXTRACTOR_VERSION = 17
# Bump when the per-grammar statistics change.
STATS_VERSION = 2
# Below this many files, starting worker processes costs more than doing the work in this process.
PARALLEL_MIN_FILES = 64
# Chunks per worker: small enough to balance uneven files, large enough to keep inter-process traffic low.
CHUNKS_PER_WORKER = 4
# No indexing phase may run longer than this; a slower phase is reported as a failure, never waited on.
PHASE_DEADLINE_S = 50

Index = namedtuple('Index', 'root facts choice unusable')

_GRAMMARS = None
_KEYWORDS = None


def build_index(root, cache_dir=None, workers=None, include=None):
    """include: path prefixes (relative to root) to index; None indexes the whole tree."""
    global _GRAMMARS
    root = os.path.abspath(root)
    _GRAMMARS, unusable = grammars.load_grammars()
    rels = sources.list_files(root)
    if include:
        prefixes = tuple(p.rstrip('/') + '/' for p in include)
        rels = [r for r in rels if r.startswith(prefixes)]
    groups = defaultdict(list)
    for rel in rels:
        groups[grammars.extension_key(rel)].append(rel)
    probe = [rel for members in groups.values() for rel in grammars.sample(members)]
    stats = defaultdict(lambda: defaultdict(list))
    for rel, per_grammar in _run(_stats_job, root, probe, cache_dir, workers):
        for name, row in per_grammar.items():
            stats[grammars.extension_key(rel)][name].append(row)
    choice = grammars.choose_grammars(stats)
    items = [(rel, choice.get(grammars.extension_key(rel))) for rel in rels]
    facts = sorted(_run(_extract_job, root, items, cache_dir, workers), key=lambda f: f['path'])
    return Index(root, facts, choice, unusable)


def _run(job, root, items, cache_dir, workers):
    if not items:
        return []
    workers = workers or os.cpu_count() or 1
    if workers == 1 or len(items) < PARALLEL_MIN_FILES:
        return job((root, items, cache_dir))
    size = max(1, -(-len(items) // (workers * CHUNKS_PER_WORKER)))
    chunks = [items[i:i + size] for i in range(0, len(items), size)]
    pool = concurrent.futures.ProcessPoolExecutor(max_workers=min(workers, len(chunks)),
                                                  mp_context=multiprocessing.get_context('spawn'))
    futures = {pool.submit(job, (root, chunk, cache_dir)): chunk for chunk in chunks}
    results = []
    try:
        for future in concurrent.futures.as_completed(futures, timeout=PHASE_DEADLINE_S):
            try:
                results.extend(future.result())
            except concurrent.futures.process.BrokenProcessPool:
                # A dead worker breaks every pending future, so the failing chunk cannot be named from here.
                raise RuntimeError(f'{job.__name__}: a worker process died (its output above says why; with no output '
                                   f'it was a native crash: rerun with --workers 1 under python -X faulthandler)')
    except concurrent.futures.TimeoutError:
        raise RuntimeError(f'{job.__name__} exceeded {PHASE_DEADLINE_S}s on {len(items)} files; workers killed')
    finally:
        for child in multiprocessing.active_children():
            child.kill()
        pool.shutdown(wait=False, cancel_futures=True)
    return results


def _loaded():
    global _GRAMMARS, _KEYWORDS
    if _GRAMMARS is None:
        _GRAMMARS = grammars.load_grammars()[0]
    if _KEYWORDS is None:
        _KEYWORDS = {name: grammars.keywords(g.language) for name, g in _GRAMMARS.items()}
    return _GRAMMARS, _KEYWORDS


def _stats_job(payload):
    root, rels, cache_dir = payload
    known, reserved = _loaded()
    out = []
    for rel in rels:
        data = sources.read_text(root, rel)
        if data is None:
            continue
        digest = hashlib.sha1(data).hexdigest()
        per_grammar = {}
        for name, grammar in known.items():
            key = f'stats{STATS_VERSION}-{digest}-{grammar.ident}'
            row = _cache_get(cache_dir, key)
            if row is None:
                tree = tree_sitter.Parser(grammar.language).parse(data)
                row = (grammars.error_share(tree, len(data)),) + grammars.recognition(tree, data, reserved[name])
                _cache_put(cache_dir, key, row)
            per_grammar[name] = row
        out.append((rel, per_grammar))
    return out


def _extract_job(payload):
    root, items, cache_dir = payload
    known, _ = _loaded()
    out = []
    for rel, name in items:
        data = sources.read_text(root, rel)
        if data is None:
            continue
        grammar = known.get(name) if name else None
        ident = grammar.ident if grammar else 'lexical'
        key = 'facts-' + hashlib.sha1(f'{rel}\0{ident}\0{EXTRACTOR_VERSION}'.encode() + data).hexdigest()
        facts = _cache_get(cache_dir, key)
        if facts is None:
            facts = extract.extract_source(rel, data, grammar.language if grammar else None, name)
            _cache_put(cache_dir, key, facts)
        out.append(facts)
    return out


def _cache_path(cache_dir, key):
    name = hashlib.sha1(key.encode()).hexdigest()
    return os.path.join(cache_dir, name[:2], name + '.pkl')


def _cache_get(cache_dir, key):
    if not cache_dir:
        return None
    try:
        with open(_cache_path(cache_dir, key), 'rb') as handle:
            return pickle.load(handle)
    except FileNotFoundError:
        return None


def _cache_put(cache_dir, key, value):
    if not cache_dir:
        return
    path = _cache_path(cache_dir, key)
    os.makedirs(os.path.dirname(path), exist_ok=True)
    temp = f'{path}.{os.getpid()}.tmp'
    with open(temp, 'wb') as handle:
        pickle.dump(value, handle, protocol=pickle.HIGHEST_PROTOCOL)
    try:
        os.replace(temp, path)
    except PermissionError:
        # Identical files in two places give two workers the same key at once; Windows refuses to replace a file
        # another process has open. The key is the content, so the entry already there holds this same value.
        if not os.path.exists(path):
            raise
        os.remove(temp)
