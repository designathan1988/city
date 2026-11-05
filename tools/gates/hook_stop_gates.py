#!/usr/bin/env python3
"""Claude Code Stop hook: when the turn ends with uncommitted changes in the repository, the gates run once and, if
they fail, the session gets the list back and keeps working. A turn without changes ends in silence, and a turn that is
already a continuation of this hook (stop_hook_active) is never held again."""
import json
import os
import subprocess
import sys

IGNORED_SUFFIXES = (".uid",)
REASON_LIMIT = 4000


def changed_files(root):
    out = subprocess.run(["git", "status", "--porcelain"], cwd=root, capture_output=True, text=True,
                         encoding="utf8", errors="replace")
    if out.returncode != 0:
        return []
    files = [line[3:].strip() for line in out.stdout.splitlines() if len(line) > 3]
    return [f for f in files if not f.endswith(IGNORED_SUFFIXES)]


def main():
    try:
        data = json.load(sys.stdin)
    except ValueError:
        return 0
    if data.get("stop_hook_active"):
        return 0
    root = os.path.abspath(os.environ.get("CLAUDE_PROJECT_DIR") or data.get("cwd") or ".")
    gates = os.path.join(root, "tools", "gates", "run_gates.py")
    if not os.path.exists(gates) or not changed_files(root):
        return 0
    res = subprocess.run([sys.executable, gates], cwd=root, capture_output=True, text=True, encoding="utf8",
                         errors="replace")
    if res.returncode == 0:
        return 0
    report = (res.stdout + res.stderr).strip()[-REASON_LIMIT:]
    sys.stdout.reconfigure(encoding="utf-8")
    print(json.dumps({"decision": "block", "reason": "Gates falharam com as mudanças deste turno. Corrija a causa "
                      "(código, dados ou mapa), nunca a regra:\n" + report}, ensure_ascii=False))
    return 0


if __name__ == "__main__":
    sys.exit(main())
