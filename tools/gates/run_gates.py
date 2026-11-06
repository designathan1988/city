#!/usr/bin/env python3
"""Project gates. Run before finishing any change: python3 tools/gates/run_gates.py
Exit 2 + report on stderr when something fails.
Flags: --summary (one-line result), --file PATH (fast check of one edited file)."""
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gate_clones  # noqa: E402
import gate_code  # noqa: E402
import gate_data  # noqa: E402
import gate_defs  # noqa: E402
import gate_docs  # noqa: E402
import gate_files  # noqa: E402
import gate_tuning  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))


def run(only_file=None):
    with open(os.path.join(ROOT, "tools", "gates", "rules.json"), encoding="utf-8") as fh:
        rules = json.load(fh)
    problems = []

    def err(gate, where, msg):
        problems.append(f"[{gate}] {where}: {msg}")

    files = gate_files.walk(ROOT, rules)
    scope = files
    if only_file:
        rel = os.path.relpath(os.path.abspath(only_file), ROOT).replace(os.sep, "/")
        scope = [rel] if rel in files else []
    gate_files.check_junk(rules, scope, err)
    gate_files.check_single_doc(rules, scope, err)
    gate_files.check_sizes(ROOT, rules, scope, err)
    gate_files.check_function_sizes(ROOT, rules, scope, err)
    gate_files.check_headers(ROOT, rules, scope, err)
    gate_code.check_layers(ROOT, rules, scope, err)
    gate_code.check_forbidden(ROOT, rules, scope, err)
    gate_defs.check_magic_numbers(ROOT, rules, scope, err)
    gate_defs.check_ui_state(ROOT, scope, err)
    if not only_file:
        gate_files.check_root(ROOT, rules, files, err)
        gate_code.check_orphans(ROOT, files, err)
        gate_docs.check_claude_md(ROOT, rules, files, err)
        gate_data.check_data(ROOT, rules, files, err)
        gate_defs.check_owners(ROOT, files, err)
        gate_defs.check_data_copies(ROOT, rules, files, err)
        gate_clones.check_clones(ROOT, rules, files, err)
        gate_tuning.check_tuning(ROOT, files, err)
        gate_tuning.check_mirrors(ROOT, rules, err)
    return problems


def main():
    args = sys.argv[1:]
    try:
        only = args[args.index("--file") + 1] if "--file" in args else None
        problems = run(only)
    except Exception as e:  # a crashing gate is a failed run, never a pass
        problems = [f"[GATE_RUNNER] o verificador falhou: {e!r}"]
    if "--summary" in args:
        state = "OK" if not problems else f"{len(problems)} problema(s)"
        print(f"Gates do projeto: {state}. Regras: CLAUDE.md. Mapa do código: log/mapa.md. Rode python3 tools/gates/run_gates.py antes de concluir.")
        for p in problems[:15]:
            print(p)
        return 0
    if problems:
        print("Gates falharam. Corrija antes de concluir (regras em CLAUDE.md):", file=sys.stderr)
        for p in problems[:80]:
            print(p, file=sys.stderr)
        if len(problems) > 80:
            print(f"... e mais {len(problems) - 80}", file=sys.stderr)
        return 2
    print("Gates OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
