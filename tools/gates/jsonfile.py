"""The gates' one reader of JSON: a key written twice in one object is an error (json.load keeps the last one without a word,
so a table could say two things and the gates would see one)."""
import json


class RepeatedKey(ValueError):
    """A JSON object names the same key twice."""


def _unique(pairs):
    out = {}
    for key, value in pairs:
        if key in out:
            raise RepeatedKey(f"the key '{key}' is written twice in one object")
        out[key] = value
    return out


def loads(text):
    return json.loads(text, object_pairs_hook=_unique)


def load(path):
    """The JSON at `path`; json.JSONDecodeError for a malformed file, RepeatedKey for a key written twice."""
    with open(path, encoding="utf-8") as fh:
        return json.load(fh, object_pairs_hook=_unique)
