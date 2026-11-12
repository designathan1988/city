---
name: bytes-not-write-text
description: Never rewrite repo files with Python Path.write_text/read_text on Windows — it turns LF files into CRLF; edit with the Edit tool or read/write bytes
metadata:
  node_type: memory
  type: feedback
  originSessionId: bceccebc-2abc-4510-8d0b-e2ad9391ef75
  modified: 2026-11-11T17:57:19.232Z
---

Python's `Path.read_text()` + `write_text()` on this Windows machine translates `\n` to `\r\n` on write, so an LF
repo file silently becomes CRLF (2026-11-11: six tracer files committed CRLF, fixed in e03d8418). The repo mixes
endings on purpose (log/pendencias.md, log/feature-trace.md are CRLF; tracer sources, mapa.md, trace-rules.md LF).

**Why:** whole-file line-ending churn hides the real diff and breaks review; the owner's records must keep their form.

**How to apply:** change repo files with the Edit tool, or in Python with `read_bytes()`/`write_bytes()` replacing
exact byte strings; after any scripted edit, check `count(b"\r")` against the committed blob before committing.
Related: [[never-delete-records]].
