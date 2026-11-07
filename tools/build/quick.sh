#!/usr/bin/env bash
# quick.sh [--ready|--all|--bless] [task,task...]: the check a change asks for, the `quick` suite of tools/tasks/suites.json
# (scripts, debug DLL, gates of the changed files, every scene whose inputs moved, the digest and the frozen photos against
# their goldens). --ready adds the release DLL and leaves dist/quick/passed when all of it holds; --all takes nothing from the
# cache; --bless makes this run's digest and photos the goldens. Exit 0 only when every task passed.
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
exec "${PY:-python}" "$ROOT/tools/tasks/cli.py" run quick "$@"
