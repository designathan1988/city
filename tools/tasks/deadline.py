# The deadline of a whole run of the orchestrator (`cli.py run --seconds N`): every process a session starts ends by itself,
# in seconds (CLAUDE.md, Working). Each task has its own deadline already (execute.py); a run of several, with their waits,
# has this one. Nothing outside the tools ends a process: Claude Code applies no time limit to a background command of a local
# session, and moves a foreground one to the background at its timeout instead of stopping it (code.claude.com/docs/en/
# tools-reference, "Time limit for background commands"). Past the deadline the run says what was still going, kills every
# tree it started (proc.kill_live), records each unfinished task TIMEOUT for the block rule (so a run that hung counts as
# the verification's run and is not started again as it is) and ends with EXPIRED.
from __future__ import annotations

import os
import sys
import threading
from typing import Callable

from . import proc
from .task import TIMEOUT

EXPIRED = 3  # the exit code of a run ended at its own deadline


class RunDeadline:
    """A run's own deadline, held while its tasks run (a context manager); `seconds` None or 0 holds none. `record` is
    given every task's status when the deadline passes; `leave` ends the process (the tests give their own)."""

    def __init__(self, seconds: float | None, names: list[str], record: Callable[[dict], None],
                 leave: Callable[[int], None] = os._exit):
        self.seconds, self.names, self.record, self.leave = seconds, list(names), record, leave
        self.statuses: dict[str, str] = {}
        self.timer = threading.Timer(seconds, self.expire) if seconds and seconds > 0 else None
        if self.timer:
            self.timer.daemon = True

    def done(self, result) -> None:
        """A task's result, as the run delivers it: a finished task is not one the deadline ends."""
        self.statuses[result.name] = result.status

    def expire(self) -> None:
        late = [n for n in self.names if n not in self.statuses]
        killed = proc.kill_live()
        print(f"the run passed its deadline of {self.seconds:g} s and was ended: {len(late)} task(s) not finished "
              f"({', '.join(late) or 'none'}), each recorded TIMEOUT; {len(killed)} process tree(s) killed. A run that "
              "needs longer is a defect of its own: read what the last lines of its log say, in the code.",
              file=sys.stderr, flush=True)
        self.record({**self.statuses, **{n: TIMEOUT for n in late}})
        sys.stdout.flush()
        self.leave(EXPIRED)

    def __enter__(self) -> "RunDeadline":
        if self.timer:
            self.timer.start()
        return self

    def __exit__(self, *exc) -> None:
        if self.timer:
            self.timer.cancel()
