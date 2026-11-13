#!/usr/bin/env bash
# free_dll.sh: a wrapper of `python tools/tasks/cli.py free-dll` (tools/tasks/cache.py free_dlls, the one owner of dist/held):
# a game left open holds its DLL and the linker cannot write over it, so each DLL of project/bin goes aside under dist/held,
# where the open game keeps running from it, and a copy stands in its place for the link to replace; dist/held is held to its
# bound. Exit 1 when a move or a copy fails.
usage() { awk 'NR > 1 && !/^#/ { exit } NR > 1 { sub(/^# ?/, ""); print }' "$0"; echo "usage: bash tools/build/free_dll.sh [-h|--help]"; }
case "${1:-}" in "") ;; -h|--help) usage; exit 0;; *) usage >&2; exit 2;; esac # help, or a wrong argument, moves nothing
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
exec "${PY:-python}" "$ROOT/tools/tasks/cli.py" free-dll
