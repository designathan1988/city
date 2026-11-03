#!/usr/bin/env python3
"""Claude Code PostToolUse hook (Write|Edit): runs the per-file gates on the edited file.
Exit 2 shows the problems to Claude right away."""
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

try:
    payload = json.load(sys.stdin)
    path = payload.get("tool_input", {}).get("file_path", "")
except (json.JSONDecodeError, AttributeError):
    path = ""
if not path:
    sys.exit(0)
res = subprocess.run([sys.executable, os.path.join(HERE, "run_gates.py"), "--file", path], capture_output=True, text=True)
if res.returncode != 0:
    sys.stderr.write(res.stderr or res.stdout)
    sys.exit(2)
sys.exit(0)
