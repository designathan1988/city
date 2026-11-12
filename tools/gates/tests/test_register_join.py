"""The gates against the register (run_gates --register, a block's close): a finding an open problem names is registered, any
other is NEW; a problem fixed or dropped names nothing any more; the findings themselves are never filtered."""
import json
import os
import shutil
import sys
import tempfile
import unittest

GATES = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
sys.path.insert(0, GATES)
import run_gates  # noqa: E402


def problem(pid, origin, status="open"):
    return {"id": pid, "title": "t", "symptom": "s", "cause": "unknown", "reading": "r", "owner": "o", "area": "tools",
            "kind": "architecture", "status": status, "criterion": "c", "evidence": [], "origin": [origin], "related": [],
            "spec": None, "opened": "2026-11-12", "traced": None, "closed": "2026-11-12" if status == "fixed" else None,
            "fix": "f" if status == "fixed" else None}


class Join(unittest.TestCase):
    def setUp(self):
        self.dir = tempfile.mkdtemp(prefix="gates-join-")
        self.register = os.path.join(self.dir, "problems.jsonl")
        rows = [problem("Ptools-1", "gate:NUMERO_MAGICO:tools/a.py"), problem("Ptools-2", "gate:TAMANHO_ARQUIVO:tools/b.py", "fixed")]
        with open(self.register, "wb") as fh:
            fh.write("".join(json.dumps(r) + "\n" for r in rows).encode("utf-8"))

    def tearDown(self):
        shutil.rmtree(self.dir, ignore_errors=True)

    def test_registered_and_new(self):
        findings = [run_gates.finding("NUMERO_MAGICO", "tools/a.py:12", "a number"),
                    run_gates.finding("NUMERO_MAGICO", "tools/c.py:3", "a number"),
                    run_gates.finding("TAMANHO_ARQUIVO", "tools/b.py", "too long")]
        run_gates.join(findings, self.register)
        self.assertEqual([f["problem"] for f in findings], ["Ptools-1", None, None])
        self.assertEqual(findings[0]["file"], "tools/a.py")
        self.assertEqual(findings[0]["line"], 12)
        self.assertEqual(len(findings), 3, "nothing is filtered: a registered finding is still reported")


if __name__ == "__main__":
    unittest.main()
