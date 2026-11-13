# What a run tells its listeners, never under the scheduler's lock (pool.py): the lines a running task writes to its log as
# they come (Follower, one thread a running task) and each finished result in the order it finished (Delivery, one thread a
# run). A slow listener (a printer that reads a whole log) then delays nobody's start, finish or deadline.
from __future__ import annotations

import queue
import threading
import traceback
from pathlib import Path
from typing import Callable

POLL_S = 0.2  # a running task's log is read again this often


class Follower:
    """The lines of a log another process is writing, handed to `on_line(ident, line)`: read from where the last read ended
    every POLL_S, complete lines only (a line still being written waits for its end), each decoded as UTF-8 with what is not
    UTF-8 replaced and its line ending left out. stop() reads what is left, the last line even without its end, and returns
    when the thread has ended: no line comes after it."""

    def __init__(self, ident: str, log: Path, on_line: Callable[[str, str], None]):
        self.ident, self.log, self.on_line = ident, Path(log), on_line
        self._pos = 0
        self._rest = b""
        self._stop = threading.Event()
        self._thread = threading.Thread(target=self._loop, name=f"follow {ident}", daemon=True)
        self._thread.start()

    def stop(self) -> None:
        self._stop.set()
        self._thread.join()

    def _loop(self) -> None:
        while True:
            stopping = self._stop.wait(POLL_S)
            self._read()
            if stopping:
                if self._rest:
                    self._say(self._rest)
                return

    def _read(self) -> None:
        try:
            with open(self.log, "rb") as fh:
                fh.seek(self._pos)
                data = fh.read()
        except FileNotFoundError:  # not created yet: the next turn reads it
            return
        self._pos += len(data)
        *lines, self._rest = (self._rest + data).split(b"\n")
        for raw in lines:
            self._say(raw)

    def _say(self, raw: bytes) -> None:
        try:
            self.on_line(self.ident, raw.decode("utf-8", errors="replace").rstrip("\r"))
        except Exception:
            traceback.print_exc()  # a broken listener is reported; the task and its log go on


class Delivery:
    """Items handed to `deliver(*item)` by a thread of its own, in the order they were put. drain() returns when everything
    put has been delivered; close() delivers what is left and ends the thread."""

    def __init__(self, deliver: Callable[..., None], name: str):
        self._deliver = deliver
        self._queue: queue.SimpleQueue = queue.SimpleQueue()
        self._cond = threading.Condition()
        self._pending = 0
        self._thread = threading.Thread(target=self._loop, name=name, daemon=True)
        self._thread.start()

    def put(self, *item) -> None:
        with self._cond:
            self._pending += 1
        self._queue.put(item)

    def drain(self) -> None:
        with self._cond:
            while self._pending:
                self._cond.wait()

    def close(self) -> None:
        self._queue.put(None)
        self._thread.join()

    def _loop(self) -> None:
        while True:
            item = self._queue.get()
            if item is None:
                return
            try:
                self._deliver(*item)
            except Exception:
                traceback.print_exc()  # a broken listener is reported; the run itself goes on
            finally:
                with self._cond:
                    self._pending -= 1
                    self._cond.notify_all()
