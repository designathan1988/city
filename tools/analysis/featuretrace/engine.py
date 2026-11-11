"""The native engine run from Python (log/trace-engine-plan.md): built by CMake where its sources are (incremental: the
build tool redoes only what changed; the compiler is CMake's choice, the CXX environment variable by its convention),
run as a process of its own whose lines are handed on as they come, and stopped with every process it started, by its
PID, when its deadline passes."""
from __future__ import annotations

import os
import signal
import subprocess
import threading
from pathlib import Path

ENGINE = Path(__file__).resolve().parents[1] / "engine"
BUILD = ENGINE / "build"
PROGRAM = "ftengine"
GENERATOR = "Ninja"
BUILD_SECONDS = 300              # a whole first build; an incremental one takes seconds
WINDOWS = os.name == "nt"


class EngineError(RuntimeError):
    pass


def build(progress=print) -> Path:
    """The engine's program, built or brought up to date."""
    if not (BUILD / "CMakeCache.txt").is_file():
        watched(["cmake", "-S", str(ENGINE), "-B", str(BUILD), "-G", GENERATOR], BUILD_SECONDS, progress)
    watched(["cmake", "--build", str(BUILD)], BUILD_SECONDS, progress)
    return program(PROGRAM)


def program(name: str) -> Path:
    path = BUILD / (name + (".exe" if WINDOWS else ""))
    if not path.is_file():
        raise EngineError(f"{path} was not built")
    return path


def run(arguments: list[str], deadline: float, progress=print) -> list[str]:
    """The engine run with these arguments: its lines, each handed to `progress` as it comes."""
    return watched([str(build(progress))] + arguments, deadline, progress)


def watched(command: list[str], deadline: float, progress) -> list[str]:
    """A process run with its output read line by line as it comes; at the deadline it is stopped with every process
    it started (Windows: `taskkill /T` on its PID; elsewhere its own process group), and that is an error."""
    late, lines = threading.Event(), []
    with subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding="utf-8",
                          errors="replace", bufsize=1, start_new_session=not WINDOWS) as process:
        def stop():
            late.set()
            if WINDOWS:
                subprocess.run(["taskkill", "/T", "/F", "/PID", str(process.pid)], capture_output=True)
            else:
                os.killpg(process.pid, signal.SIGKILL)

        timer = threading.Timer(deadline, stop)
        timer.start()
        try:
            for line in process.stdout:
                lines.append(line.rstrip("\n"))
                progress(lines[-1])
            code = process.wait()
        finally:
            timer.cancel()
    if late.is_set():
        raise EngineError(f"{command[0]} passed its deadline of {deadline} s and was stopped")
    if code != 0:
        raise EngineError(f"{command[0]} exited {code}: " + " | ".join(lines[-5:]))
    return lines
