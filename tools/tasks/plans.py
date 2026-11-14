# The photo plans of a suites file, their one reading: what the block rule (block_review.py) and the hook
# (.claude/hooks/port_looks.py) ask before a block closes or is committed. A functionality is photographed by a plan of its own;
# FAR_VIEWS, the generated town from six distances, is the same for every functionality and is none. Stands alone (no import of
# the package), so the hook loads it by its path.
from __future__ import annotations

import json
from pathlib import Path

FAR_VIEWS = "zoom_ladder"


def photo_plans(suites: Path) -> dict:
    """Each task of the suites file that has a `plan`, by name, with its plan's file in the project."""
    def walk(doc) -> dict:
        out = {}
        for part in doc.values() if isinstance(doc, dict) else doc if isinstance(doc, list) else ():
            if isinstance(part, dict) and isinstance(part.get("name"), str) and isinstance(part.get("plan"), str):
                out[part["name"]] = "project/" + part["plan"].removeprefix("res://")
            elif isinstance(part, (dict, list)):
                out.update(walk(part))
        return out
    try:
        return walk(json.loads(Path(suites).read_text(encoding="utf-8")))
    except (OSError, json.JSONDecodeError):
        return {}
