# A machine tool's version (suites.json `versions`): the whole output of its command, line endings normalised. A task whose result
# follows from a tool is keyed by it, so another tool, or none, is never taken for the result of the same sources.
from __future__ import annotations

import tempfile
from pathlib import Path

from . import proc

VERSION_TIMEOUT_S = 60


def version(name: str, argv: list[str]) -> str:
    """The output of `argv`, the version command of the tool `name`. A tool that is missing or fails stops the selection with
    the reason (ValueError, the CLI's usage exit): nothing runs and nothing is cached on a machine without it."""
    with tempfile.TemporaryDirectory() as tmp:
        log = Path(tmp) / "version.txt"
        try:
            code, timed_out = proc.run(argv, VERSION_TIMEOUT_S, log=log)
        except OSError as e:
            raise ValueError(f"{name}: {' '.join(argv)} could not start: {e}") from e
        text = log.read_text(encoding="utf-8", errors="replace").strip()
    if timed_out or code != 0 or not text:
        raise ValueError(f"{name}: {' '.join(argv)} gave exit {code}{' (timed out)' if timed_out else ''}: {text}")
    return "\n".join(line.rstrip() for line in text.splitlines())
