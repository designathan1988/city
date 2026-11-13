"""The register's tests: ids, every allowed and refused move of a status, the rules a save holds, the generated view and its
check, a malformed line refused, the command line's exit codes. Each test works on a register of its own in a temporary
repository (the real schema copied in, the fixture register as its doc/problems.jsonl)."""
import contextlib
import io
import os
import shutil
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
TOOLS_DIR = os.path.abspath(os.path.join(HERE, "..", ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import cli  # noqa: E402
from atlas import problems as register  # noqa: E402
from atlas import render  # noqa: E402

FIXTURE = os.path.join(HERE, "fixtures", "problems.jsonl")


class RegisterCase(unittest.TestCase):
    def setUp(self):
        self.root = tempfile.mkdtemp(prefix="atlas-")
        os.makedirs(os.path.join(self.root, "tools", "gates", "schemas"))
        os.makedirs(os.path.join(self.root, "doc"))
        shutil.copy(os.path.join(register.ROOT, register.SCHEMA), os.path.join(self.root, register.SCHEMA))
        shutil.copy(FIXTURE, os.path.join(self.root, register.REGISTER))

    def tearDown(self):
        shutil.rmtree(self.root, ignore_errors=True)

    def reg(self):
        return register.Register(root=self.root)

    def new(self, reg, **extra):
        given = {"title": "t", "symptom": "s", "area": "traffic", "kind": "defect", "origin": ["test"], "owner": "o",
                 "criterion": "c", "reading": "r"}
        given.update(extra)
        return reg.add(**given)

    def atlas(self, *argv):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
            code = cli.main(list(argv) + ["--root", self.root])
        return code, out.getvalue()


class Ids(RegisterCase):
    def test_next_id_is_one_past_the_area_largest(self):
        reg = self.reg()
        self.assertEqual(self.new(reg)["id"], "Ptraffic-3")
        self.assertEqual(self.new(reg, area="roads")["id"], "Proads-1")

    def test_an_id_is_never_reused_after_a_drop(self):
        reg = self.reg()
        first = self.new(reg)
        reg.drop(first["id"], "no problem")
        self.assertEqual(self.new(reg)["id"], "Ptraffic-4")

    def test_unknown_area_refused(self):
        with self.assertRaises(register.RegisterError):
            self.new(self.reg(), area="moon")


class Moves(RegisterCase):
    def test_the_whole_way_to_fixed(self):
        reg = self.reg()
        p = self.new(reg)
        self.assertEqual(p["status"], "open")
        reg.update(p["id"], cause="a decision")
        self.assertEqual(p["status"], "traced")
        self.assertIsNotNone(p["traced"])
        for status in ("planned", "fixed-pending-check"):
            reg.update(p["id"], status=status)
        reg.close(p["id"], "the decision changed")
        self.assertEqual((p["status"], p["fix"]), ("fixed", "the decision changed"))
        self.assertIsNotNone(p["closed"])
        reg.save()

    def test_a_failed_criterion_goes_back_to_traced(self):
        reg = self.reg()
        p = reg.get("Ptraffic-2")
        reg.update(p["id"], status="planned")
        reg.update(p["id"], status="fixed-pending-check")
        reg.update(p["id"], status="traced")
        self.assertEqual(p["status"], "traced")

    def test_refused_moves(self):
        reg = self.reg()
        for pid, status in (("Ptraffic-1", "fixed"), ("Ptraffic-1", "planned"), ("Ptraffic-2", "fixed-pending-check"),
                            ("Ptraffic-2", "open")):
            with self.subTest(pid=pid, status=status), self.assertRaises(register.RegisterError):
                reg.move(reg.get(pid), status)

    def test_close_only_after_its_check(self):
        with self.assertRaises(register.RegisterError):
            self.reg().close("Ptraffic-2", "skipped the check")

    def test_final_states_do_not_move(self):
        reg = self.reg()
        reg.drop("Ptraffic-1", "no problem")
        for status in register.MOVES:
            with self.subTest(status=status), self.assertRaises(register.RegisterError):
                reg.move(reg.get("Ptraffic-1"), status)

    def test_a_cause_given_at_once_traces(self):
        p = self.new(self.reg(), cause="known", traced_at="2026-11-01T08:00")
        self.assertEqual((p["status"], p["traced"]), ("traced", "2026-11-01T08:00"))


class Save(RegisterCase):
    def test_written_as_lf_bytes_and_read_back(self):
        reg = self.reg()
        self.new(reg, title="a title past ASCII: Ω and λ")
        reg.save()
        with open(os.path.join(self.root, register.REGISTER), "rb") as fh:
            data = fh.read()
        self.assertNotIn(b"\r", data)
        self.assertEqual(len(self.reg().problems), 4)

    def test_unknown_cause_without_reading_refused(self):
        reg = self.reg()
        self.new(reg, reading=None)
        with self.assertRaises(register.RegisterError):
            reg.save()

    def test_missing_evidence_refused(self):
        reg = self.reg()
        reg.update("Ptraffic-2", evidence=["dist/evidence/Ptraffic-2/none.png"])
        with self.assertRaises(register.RegisterError):
            reg.save()

    def test_malformed_line_refused(self):
        with open(os.path.join(self.root, register.REGISTER), "ab") as fh:
            fh.write(b"{not json\n")
        with self.assertRaises(register.RegisterError):
            self.reg()

    def test_query_filters(self):
        reg = self.reg()
        self.assertEqual([p["id"] for p in reg.query(area="traffic", status="open")], ["Ptraffic-1"])
        self.assertEqual([p["id"] for p in reg.query(owner="lane_change")], ["Ptraffic-2"])
        self.assertEqual([p["id"] for p in reg.query(cause="renamed module")], ["Ptools-1"])


class View(RegisterCase):
    def test_render_by_area_then_status_and_check(self):
        self.assertEqual(render.main(["--root", self.root, "--check"]), 1)
        self.assertEqual(render.main(["--root", self.root]), 0)
        self.assertEqual(render.main(["--root", self.root, "--check"]), 0)
        with open(os.path.join(self.root, render.VIEW), encoding="utf-8") as fh:
            text = fh.read()
        self.assertLess(text.index("## traffic"), text.index("## tools"))
        self.assertLess(text.index("### open"), text.index("### traced"))


class CommandLine(RegisterCase):
    def test_exit_codes(self):
        self.assertEqual(self.atlas("problems", "--status", "open")[0], 0)
        code, out = self.atlas("problems", "add", "--title", "t", "--symptom", "s", "--area", "roads", "--kind", "defect",
                               "--origin", "test", "--owner", "o", "--criterion", "c", "--reading", "r")
        self.assertEqual(code, 0, out)
        self.assertIn("Proads-1", out)
        self.assertEqual(self.atlas("problems", "close", "Ptraffic-1", "--fix", "f")[0], 1)
        self.assertEqual(self.atlas("problems", "update", "Ptraffic-1", "--status", "fixed")[0], 1)
        self.assertEqual(self.atlas("nothing")[0], 2)
        self.assertEqual(self.atlas("problems", "add", "--title", "t")[0], 2)

    def test_json_after_the_command(self):
        code, out = self.atlas("problems", "--area", "tools", "--json")
        self.assertEqual(code, 0)
        self.assertIn('"Ptools-1"', out)


if __name__ == "__main__":
    unittest.main()
