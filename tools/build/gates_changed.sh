#!/usr/bin/env bash
# gates_changed.sh <workspace>: the gates on the files the workspace changed (git status), nothing else
cd "$1" || exit 2
args=()
while read -r f; do args+=(--file "$f"); done < <(git status --short | awk '{print $2}' | grep -E '\.(cpp|h|gd|json|md|sh|tscn|py|ps1)$' | grep -v '^log/')
exec "C:/Users/Jonathan/AppData/Local/Programs/Python/Python312/python.exe" tools/gates/run_gates.py "${args[@]}" --summary
