#!/usr/bin/env python3
"""Claude Code PreToolUse hook (Write|Edit): blocks creating new documents or memory files.
CLAUDE.md is the only project document; notes, plans and memories go into it."""
import json
import os
import sys

DOC_EXT = (".md", ".txt", ".rst", ".adoc")

try:
    path = json.load(sys.stdin).get("tool_input", {}).get("file_path", "")
except (json.JSONDecodeError, AttributeError):
    sys.exit(0)
root = os.environ.get("CLAUDE_PROJECT_DIR", "")
inside = bool(root) and os.path.abspath(path).startswith(os.path.abspath(root) + os.sep)
if inside and path.lower().endswith(DOC_EXT) and os.path.basename(path) != "CLAUDE.md" and not os.path.exists(path):
    print(f"Bloqueado: '{path}' seria um novo documento. O projeto tem um único documento (CLAUDE.md); "
          "atualize a seção certa dele.", file=sys.stderr)
    sys.exit(2)
sys.exit(0)
