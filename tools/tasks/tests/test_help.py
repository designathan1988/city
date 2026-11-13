# Help never acts: --help on each command the tools run by hand (tools/hunt/hunt.py, tools/build/pgo.sh,
# tools/build/free_dll.sh; cli.py's own in test_cli.py) prints its usage, exits 0 and writes nothing; a wrong argument to a
# script exits 2 and writes nothing either. No hunt, no training and no DLL move starts: hunt's run is mocked to fail the test
# if reached, and each script runs from a copy in a throwaway tree whose files are compared before and after.
import contextlib
import importlib.util
import io
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import TOOLS, proc  # noqa: E402

from tasks import locations  # noqa: E402

HELP_TIMEOUT_S = 60  # a help that answers takes well under a second; a script that started acting would pass this
SCRIPTS = ("pgo.sh", "free_dll.sh")  # tools/build/
WRONG_ARGUMENT = "-help"  # a misspelt help: refused, never taken for a run


def _hunt():
    spec = importlib.util.spec_from_file_location("hunt_under_test", TOOLS / "hunt" / "hunt.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def _files(root: Path) -> dict[str, bytes]:
    return {p.relative_to(root).as_posix(): p.read_bytes() for p in sorted(root.rglob("*")) if p.is_file()}


class HuntHelpTest(unittest.TestCase):
    def test_hunt_help_prints_the_usage_exits_0_and_runs_nothing(self):
        """hunt.py --help answers from argparse before the block rule, the selection, the deletion of the last list and
        the run: each is mocked to fail if reached, and a list already there is left whole."""
        hunt = _hunt()
        with tempfile.TemporaryDirectory(prefix="hunt-help-") as tmp:
            out_file = Path(tmp) / "hunt.json"
            out_file.write_text('{"defects": []}\n', encoding="utf-8")
            reached = AssertionError("hunt.py --help reached the work")
            out = io.StringIO()
            with mock.patch.object(hunt, "OUT", out_file), \
                    mock.patch.object(hunt, "block") as block, \
                    mock.patch.object(hunt, "Suites", side_effect=reached) as suites, \
                    mock.patch.object(hunt, "pool") as pool, \
                    contextlib.redirect_stdout(out), contextlib.redirect_stderr(io.StringIO()):
                block.refusal.side_effect = reached
                pool.run.side_effect = reached
                with self.assertRaises(SystemExit) as ended:
                    hunt.main(["--help"])
            self.assertEqual(ended.exception.code, 0)
            self.assertIn("usage: hunt.py", out.getvalue())
            self.assertIn("The defect hunt", out.getvalue(), "the docstring is the help")
            self.assertFalse(block.refusal.called or suites.called or pool.run.called, "help reached the work")
            self.assertEqual(out_file.read_text(encoding="utf-8"), '{"defects": []}\n', "the last list is left whole")


class ScriptHelpTest(unittest.TestCase):
    def setUp(self):
        try:
            self.bash = locations.locate("bash")
        except locations.Missing as e:
            self.skipTest(f"no bash to run the shell scripts: {e}")
        self.root = Path(tempfile.mkdtemp(prefix="script-help-")).resolve()
        self.addCleanup(shutil.rmtree, self.root, True)
        (self.root / "tools" / "build").mkdir(parents=True)

    def run_script(self, name: str, *args: str) -> tuple[subprocess.CompletedProcess, bool]:
        """The script run from its copy at <tree>/tools/build (its root, by its own path, is the tree), and whether the
        tree's files are the same after it."""
        script = self.root / "tools" / "build" / name
        shutil.copyfile(TOOLS / "build" / name, script)
        before = _files(self.root)
        r = subprocess.run([self.bash, script.as_posix(), *args], cwd=self.root, capture_output=True, text=True,
                           timeout=HELP_TIMEOUT_S, creationflags=proc.NO_WINDOW)
        return r, _files(self.root) == before

    def test_help_prints_the_usage_exits_0_and_writes_nothing(self):
        for name in SCRIPTS:
            for flag in ("--help", "-h"):
                r, same = self.run_script(name, flag)
                self.assertEqual(r.returncode, 0, f"{name} {flag}: {r.stderr}")
                self.assertIn(f"usage: bash tools/build/{name}", r.stdout, f"{name} {flag}")
                self.assertIn(f"{name}:", r.stdout, f"{name} {flag}: the header comment is the help")
                self.assertTrue(same, f"{name} {flag} wrote in its tree")

    def test_a_wrong_argument_exits_2_and_writes_nothing(self):
        for name in SCRIPTS:
            r, same = self.run_script(name, WRONG_ARGUMENT)
            self.assertEqual(r.returncode, 2, f"{name} {WRONG_ARGUMENT}: {r.stderr}")
            self.assertIn(f"usage: bash tools/build/{name}", r.stderr, name)
            self.assertTrue(same, f"{name} {WRONG_ARGUMENT} wrote in its tree")


if __name__ == "__main__":
    unittest.main()
