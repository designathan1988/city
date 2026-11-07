"""Small shared rules: stable hashes, splitting identifiers into words, and cuts taken from a distribution."""
import functools
import hashlib
import json
import math
import os
import re

SPLIT_CASE = re.compile(r"[A-Z]+(?![a-z])|[A-Z]?[a-z]+|[0-9]+")
IDENT = re.compile(r"(?<![0-9A-Za-z_\\])[A-Za-z_][A-Za-z0-9_]*")


def digest(text):
    """A short stable hash of a string or bytes."""
    data = text.encode("utf8", "surrogatepass") if isinstance(text, str) else text
    return hashlib.blake2b(data, digest_size=10).hexdigest()


@functools.lru_cache(maxsize=None)
def words_of(name):
    """The lower-case words an identifier is made of, whatever its case style."""
    return [w.lower() for part in name.split("_") for w in SPLIT_CASE.findall(part)]


def natural_break(values):
    """The value that best splits a sample in two (Otsu's between-class variance): no constant, only the sample. One sort, one pass of sums."""
    import numpy  # here, not at the top: a worker process that never cuts anything must not load it
    data = numpy.sort(numpy.fromiter(values, dtype=float))
    if data.size < 2 or data[0] == data[-1]:
        return math.inf
    sums = numpy.cumsum(data)
    low = numpy.arange(1, data.size, dtype=float)
    high = data.size - low
    left = sums[:-1]
    spread = low * high * (left / low - (sums[-1] - left) / high) ** 2
    spread[data[:-1] == data[1:]] = -1.0
    i = int(numpy.argmax(spread))
    return float((data[i] + data[i + 1]) / 2)


def noisy_or(confidences):
    """Independent sources that agree are worth more than any one of them."""
    miss = 1.0
    for c in confidences:
        miss *= 1.0 - c
    return 1.0 - miss


def write_json(path, value):
    """A file with sorted keys and no time in it: two runs give the same bytes."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf8", newline="\n") as f:
        json.dump(value, f, ensure_ascii=False, sort_keys=True, indent=0, separators=(",", ":"))
        f.write("\n")


def read_json(path, default=None):
    if not os.path.exists(path):
        return default
    with open(path, encoding="utf8") as f:
        return json.load(f)


def text_of(data):
    """Bytes as text, line ends normalised, or None for what is not text."""
    if b"\0" in data[:8192]:
        return None
    return data.decode("utf8", "replace").replace("\r\n", "\n")


def _here():
    """The program's own folder, the real one also when a selfcheck runs from a copy of it."""
    return os.environ.get("ARCH_SELF") or os.path.dirname(os.path.abspath(__file__))


def own_prefixes(root):
    """What the program is, inside the repository it reads: itself, its entry and its answer keys, as paths from that repository's top."""
    rel = inside(_here(), root)
    return (rel + "/", rel + ".py", rel + "_eval/") if rel else ()


def inside(path, root):
    """A path as seen from a folder, or None when it lies outside it, on another drive included."""
    try:
        rel = os.path.relpath(path, root).replace("\\", "/")
    except ValueError:
        return None
    return None if rel.startswith("..") else rel


def program_root():
    """Where this program lives: outputs go under the repository holding it."""
    return os.path.dirname(os.path.dirname(os.path.dirname(_here())))
