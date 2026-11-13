"""How far the questions may go: a time and a memory the process may use, where the one running it asks for a limit
(none by default: the analysis and the questions go to their end). Past either, a question stops and what was not
answered is said (never answered silently in part).
"""
from __future__ import annotations

import ctypes
import math
import struct
import sys
import time

SECONDS = math.inf               # the longest the questions of one run may take: no limit unless asked (--seconds)
ANALYSIS_SECONDS = math.inf      # the longest the analysis may take: no limit
MEGABYTES = math.inf             # the most memory the process may hold: no limit unless asked (--memory)
LOOK_SECONDS = 0.25              # how often the memory is looked at (a look costs; past the ceiling by so little)


def held_megabytes() -> float | None:
    """The memory the process holds now (its working set), or None where the system does not say."""
    try:
        if sys.platform.startswith("win"):
            buffer = ctypes.create_string_buffer(80)
            struct.pack_into("<L", buffer, 0, 80)
            api, kernel = ctypes.windll.psapi, ctypes.windll.kernel32
            kernel.GetCurrentProcess.restype = ctypes.c_void_p
            api.GetProcessMemoryInfo.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_ulong]
            if not api.GetProcessMemoryInfo(kernel.GetCurrentProcess(), buffer, 80):
                return None
            return struct.unpack_from("<Q", buffer, 16)[0] / 2 ** 20
        import resource
        peak = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
        return peak / (2 ** 20 if sys.platform.startswith("darwin") else 2 ** 10)
    except (OSError, AttributeError, ValueError, ImportError):
        return None


class Ceiling:
    """The time and the memory left to the questions; what had to be left unanswered."""
    def __init__(self, seconds: float = SECONDS, megabytes: float = MEGABYTES):
        self.until = time.monotonic() + seconds
        self.megabytes = megabytes
        self.looked = 0.0
        self.why = ""
        self.left_out = []       # what was not answered: (what, why)

    def over(self) -> bool:
        """Whether the questions must stop now (and why, kept)."""
        if self.why:
            return True
        now = time.monotonic()
        if now > self.until:
            self.why = "the time"
        elif now - self.looked >= LOOK_SECONDS:
            self.looked = now
            held = held_megabytes()
            if held is not None and held > self.megabytes:
                self.why = "the memory"
        return bool(self.why)

    def skip(self, what: str) -> None:
        self.left_out.append((what, self.why))

    def text(self) -> str:
        if not self.left_out:
            return ""
        lines = [f"Not answered ({len(self.left_out)}), the ceiling reached ({self.why}):"]
        lines += [f"  {what}" for what, _ in self.left_out[:50]]
        if len(self.left_out) > 50:
            lines.append(f"  ... {len(self.left_out) - 50} more")
        return "\n".join(lines)


NONE = Ceiling(math.inf, math.inf)       # no ceiling (tests, a caller that sets none)
