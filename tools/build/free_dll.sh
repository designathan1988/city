#!/usr/bin/env bash
# free_dll.sh: a wrapper of `python tools/tasks/cli.py free-dll` (tools/tasks/cache.py free_dlls, the one owner of dist/held):
# a game left open holds its DLL and the linker cannot write over it, so each DLL of project/bin goes aside under dist/held,
# where the open game keeps running from it, and a copy stands in its place for the link to replace; dist/held is held to its
# bound. Exit 1 when a move or a copy fails.
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
exec "${PY:-python}" "$ROOT/tools/tasks/cli.py" free-dll
