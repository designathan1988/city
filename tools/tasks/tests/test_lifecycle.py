# lifecycle: every worktree, copy, branch and stash classed, the ones older than their work failing the listing; clean holding
# each output folder to its bound (keep with the builds an open problem names kept, while_open, max_mb, whole_mb), listing and removing
# what no one declared, a dry run removing nothing, nothing removed while the register cannot be read; the task cache's ledger
# evicting the least recently used past its bound; dist/held (free_dlls) holding what an open game runs from, to its bound;
# the run folders' locks kept and the changed lists bounded by the bounds tools/tasks/suites.json declares; an --only folder no
# one declared a usage error.
import json
import os
import shutil
import stat
import sys
import time
import unittest
from contextlib import redirect_stderr, redirect_stdout
from io import StringIO
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import RepoCase, git  # noqa: E402

from tasks import admission, cache, clean, cli, ledger, lifecycle, outputs, runs, task  # noqa: E402

OLD = "2026-01-01T00:00:00+00:00"
LISTS = "dist/tasks/lists"  # the changed lists suites.py (_changed_list) writes, one per distinct list
EXTRA_LISTS = 6  # lists written past the bound of their folder
USAGE_EXIT = 2  # the contract pinned by value: a wrong call exits 2 whatever clean.USAGE says, so a drift of it fails here
AGE_STEP_S = 10  # seconds between two entries made one after another, so their order by time is certain
LIFECYCLE = {"lifecycle": {"stale_days": 7, "outputs": [
    {"path": "dist/builds", "owner": "deliver.sh", "keep": 2, "index": ["latest.txt"]},
    {"path": "dist/evidence", "owner": "atlas", "while_open": True},
    {"path": "dist/evidence/research", "owner": "research", "whole_mb": 1},
    {"path": "dist/quick", "owner": "quick", "max_mb": 1},
    {"path": "dist/build-x", "owner": "a build", "whole_mb": 1},
    {"path": "dist/small-x", "owner": "a build", "whole_mb": 1}]}}


def quiet(fn, *args):
    out = StringIO()
    with redirect_stdout(out):
        result = fn(*args)
    return result, out.getvalue()


class LifecycleCase(RepoCase):
    def setUp(self):
        super().setUp()
        git(self.root, "branch", "-M", "main")
        self.suites = self.root / "suites.json"
        self.suites.write_text(json.dumps(LIFECYCLE), encoding="utf-8")

    def commit(self, name, when=None):
        (self.root / name).write_text(name, encoding="utf-8")
        git(self.root, "add", name)
        env = {**os.environ, "GIT_COMMITTER_DATE": when, "GIT_AUTHOR_DATE": when} if when else None
        import subprocess
        subprocess.run(["git", "-C", str(self.root), "-c", "user.name=t", "-c", "user.email=t@localhost", "commit", "-q", "-m", name],
                       check=True, capture_output=True, env=env, timeout=60)


class Survey(LifecycleCase):
    def test_classes_and_what_is_older_than_its_work(self):
        tree = self.root.parent / f"{self.root.name}-wt"
        git(self.root, "worktree", "add", "-q", "--detach", str(tree), "main")
        self.addCleanup(shutil.rmtree, tree, True)
        git(self.root, "branch", "merged-one")
        git(self.root, "checkout", "-q", "-b", "fresh")
        self.commit("f.txt")
        git(self.root, "checkout", "-q", "-b", "old", "main")
        self.commit("o.txt", OLD)
        git(self.root, "checkout", "-q", "main")
        copy = self.root.parent / f"{self.root.name}-copy"
        copy.mkdir()
        self.addCleanup(shutil.rmtree, copy, True)
        rows = {r["name"].replace("\\", "/"): r for r in lifecycle.survey(self.root, suites=self.suites)}
        by_kind = {(r["kind"], Path(n).name if r["kind"] in ("worktree", "copy") else n): r for n, r in rows.items()}
        self.assertTrue(by_kind[("worktree", tree.name)]["stale"], "a worktree whose commit is in main is older than its work")
        self.assertTrue(by_kind[("copy", copy.name)]["stale"])
        self.assertTrue(by_kind[("branch", "merged-one")]["stale"])
        self.assertFalse(by_kind[("branch", "fresh")]["stale"])
        self.assertTrue(by_kind[("branch", "old")]["stale"], "an unmerged branch untouched past stale_days")
        code, _ = quiet(lifecycle.cmd_worktrees, ["--root", str(self.root), "--suites", str(self.suites)])
        self.assertEqual(code, 1)


class Clean(LifecycleCase):
    def make(self, rel, size=10, age=0):
        p = self.root / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_bytes(b"x" * size)
        t = time.time() - age
        os.utime(p, (t, t))
        os.utime(p.parent, (t, t))
        return p

    def setUp(self):
        super().setUp()
        for k, name in enumerate(["b1", "b2", "b3", "b4"]):
            self.make(f"dist/builds/{name}/Roadcraft.exe", age=1000 - k * 100)
        self.make("dist/builds/latest.txt")  # the owner's index, the newest file of the folder
        self.make("dist/evidence/Ptools-1/shot.png")
        self.make("dist/evidence/Ptools-9/shot.png")
        self.make("dist/evidence/research/q2/run.log")
        self.make("dist/quick/old.bin", size=800_000, age=500)
        self.make("dist/quick/new.bin", size=800_000, age=10)
        self.make("dist/build-x/a/one.o", size=700_000)
        self.make("dist/build-x/b/two.o", size=700_000)
        self.make("dist/small-x/one.o", size=10)
        self.make("dist/stray/junk.txt")
        os.chmod(self.make("dist/stray/.git/objects/01/object"), stat.S_IREAD)  # git's objects are read-only
        open_one = {"id": "Ptools-1", "status": "open", "evidence": ["dist/builds/b1/Roadcraft.exe"]}
        self.make("doc/problems.jsonl").write_bytes((json.dumps(open_one) + "\n").encode())

    def run_clean(self, *flags):
        return quiet(clean.cmd_clean, ["--root", str(self.root), "--suites", str(self.suites), *flags])[1]

    def declare(self, *paths):
        """The entries of `paths` as tools/tasks/suites.json declares them, added to this case's suites: the bound tested is
        the one the work runs with (a missing entry raises KeyError here)."""
        entries = [outputs.bound(p) for p in paths]
        data = json.loads(self.suites.read_text(encoding="utf-8"))
        data["lifecycle"]["outputs"] += entries
        self.suites.write_text(json.dumps(data), encoding="utf-8")
        return entries

    def test_the_locks_of_runs_stay(self):
        """The locks and specs folders, older than every run folder, are no run folders (the index; a lock held open is
        kept by cache.remove_unless_held anyway, so this checks the index, not a held lock)."""
        keep = self.declare(runs.RUNS, LISTS)[0]["keep"]
        made = [f"{runs.RUNS}/{k:04d}-1-run" for k in range(keep + 1)]
        self.make(f"{admission.LOCKS}/dll.lock", age=AGE_STEP_S * (len(made) + 2))  # older than every run folder
        self.make(f"{task.SPECS}/spec.json", age=AGE_STEP_S * (len(made) + 1))  # a reused spec never refreshes its folder
        for k, rel in enumerate(made):
            self.make(f"{rel}/events.jsonl", age=AGE_STEP_S * (len(made) - k))
        self.run_clean()
        self.assertTrue((self.root / admission.LOCKS / "dll.lock").exists(), "the locks folder is no run folder")
        self.assertTrue((self.root / task.SPECS / "spec.json").exists(), "the specs folder is no run folder")
        self.assertFalse((self.root / made[0]).exists(), "the oldest run folder past the bound went")
        left = [p for p in (self.root / runs.RUNS).iterdir() if p.name[:1].isdigit()]
        self.assertEqual(len(left), keep)

    def test_the_lists_are_bounded(self):
        keep = self.declare(runs.RUNS, LISTS)[1]["keep"]
        made = [f"{LISTS}/changed-{k:016d}.txt" for k in range(keep + EXTRA_LISTS)]
        for k, rel in enumerate(made):
            self.make(rel, age=AGE_STEP_S * (len(made) - k))
        self.run_clean()
        left = sorted(p.name for p in (self.root / LISTS).iterdir())
        self.assertEqual(left, sorted(Path(rel).name for rel in made[-keep:]), "the newest lists kept, the rest gone")

    def test_an_unknown_folder_is_a_usage_error(self):
        err = StringIO()
        with redirect_stdout(StringIO()), redirect_stderr(err):
            code = clean.cmd_clean(["--root", str(self.root), "--suites", str(self.suites), "--only", "dist/nosuch"])
        self.assertEqual(code, USAGE_EXIT)
        self.assertIn("clean: dist/nosuch", err.getvalue())
        self.assertNotIn("Traceback", err.getvalue())

    def test_dry_run_removes_nothing(self):
        said = self.run_clean("--dry-run")
        self.assertIn("dist/builds: 1 entry", said)
        self.assertIn("no owner: dist/stray", said)
        self.assertTrue(all((self.root / "dist/builds" / n).exists() for n in ("b1", "b2", "b3", "b4")))

    def test_each_bound(self):
        self.run_clean()
        left = sorted(p.name for p in (self.root / "dist/builds").iterdir())
        self.assertEqual(left, ["b1", "b3", "b4", "latest.txt"], "the newest two builds, b1 an open problem names, the index")
        self.assertTrue((self.root / "dist/evidence/Ptools-1").exists())
        self.assertFalse((self.root / "dist/evidence/Ptools-9").exists(), "evidence of no open problem")
        self.assertTrue((self.root / "dist/evidence/research/q2/run.log").exists(), "a folder declared inside another keeps its own bound")
        self.assertFalse((self.root / "dist/quick/old.bin").exists())
        self.assertTrue((self.root / "dist/quick/new.bin").exists())
        self.assertFalse((self.root / "dist/build-x").exists(), "a piece written whole goes whole past its size")
        self.assertTrue((self.root / "dist/small-x/one.o").exists(), "and stays whole inside it")
        self.assertTrue((self.root / "dist/stray").exists(), "an unowned folder is listed, not removed, without --unowned")
        self.run_clean("--unowned")
        self.assertFalse((self.root / "dist/stray").exists(), "removed whole, its read-only files included")

    def test_an_unreadable_register_stops_clean_before_anything_goes(self):
        (self.root / "doc/problems.jsonl").write_bytes(b"not a problem\n")
        err = StringIO()
        with redirect_stdout(StringIO()), redirect_stderr(err):
            code = clean.cmd_clean(["--root", str(self.root), "--suites", str(self.suites), "--unowned"])
        self.assertEqual(code, 1)
        self.assertIn("the register cannot be read", err.getvalue())
        self.assertTrue((self.root / "dist/evidence/Ptools-9").exists(), "evidence went while no one could say it was free")
        self.assertTrue(all((self.root / "dist/builds" / n).exists() for n in ("b1", "b2", "b3", "b4")))
        self.assertTrue((self.root / "dist/stray").exists())


class Held(unittest.TestCase):
    """free_dlls: each DLL set aside under dist/held and copied back; dist/held held to its bound, the newest set aside kept."""

    def setUp(self):
        import tempfile
        self.root = Path(tempfile.mkdtemp(prefix="tasks-held-")).resolve()
        self.addCleanup(shutil.rmtree, self.root, True)
        (self.root / cache.BIN).mkdir(parents=True)
        for name in ("a.dll", "b.dll"):
            (self.root / cache.BIN / name).write_bytes(name.encode() * 10)
        self.keep = outputs.bound(cache.HELD)["keep"]
        self.old = []
        for k in range(self.keep + 2):  # left by earlier links, the script of before included (no time in the name)
            p = self.root / cache.HELD / f"old{k}.dll"
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_bytes(b"old")
            t = time.time() - 1000 + k
            os.utime(p, (t, t))
            self.old.append(p)

    def test_each_dll_goes_aside_and_a_copy_stands_in_its_place(self):
        with redirect_stdout(StringIO()):
            cache.free_dlls(self.root)
        for name in ("a.dll", "b.dll"):
            self.assertEqual((self.root / cache.BIN / name).read_bytes(), name.encode() * 10)
        held = sorted(p.name for p in (self.root / cache.HELD).iterdir())
        self.assertEqual(len(held), self.keep, "dist/held past its bound")
        self.assertEqual(len([n for n in held if n.startswith(("a.dll.", "b.dll."))]), 2, "the DLLs just set aside went")
        self.assertTrue(self.old[-1].exists(), "the newest of the earlier ones went before an older one")
        self.assertFalse(self.old[0].exists())

    @unittest.skipUnless(sys.platform == "win32", "Windows refuses to delete a file a process holds open")
    def test_a_held_file_in_use_is_kept_and_said(self):
        out = StringIO()
        with open(self.old[0], "rb"), redirect_stdout(out):
            cache.free_dlls(self.root)
        self.assertTrue(self.old[0].exists())
        self.assertIn(f"kept, in use: {cache.HELD}/old0.dll", out.getvalue())

    def test_the_command_says_a_failed_move(self):
        err = StringIO()
        with mock.patch.object(cache, "set_aside", side_effect=PermissionError("held by another")), \
                redirect_stdout(StringIO()), redirect_stderr(err):
            code = cli.main(["free-dll", "--root", str(self.root)])
        self.assertEqual(code, 1)
        self.assertIn("free-dll failed: held by another", err.getvalue())


class Ledger(unittest.TestCase):
    def test_the_least_recently_used_go_past_the_bound(self):
        import tempfile
        root = Path(tempfile.mkdtemp(prefix="tasks-ledger-"))
        self.addCleanup(shutil.rmtree, root, True)
        led = ledger.Ledger(root, max_bytes=2500)
        entries = []
        for k in range(3):
            e = root / "task" / "ab" / f"key{k}"
            e.mkdir(parents=True)
            (e / "result.json").write_bytes(b"y" * 1000)
            t = time.time() - ledger.YOUNG_S * (10 - k)
            os.utime(e, (t, t))
            entries.append(e)
            led.add(e)
        self.assertFalse(entries[0].exists(), "the least recently used entry went when the total passed the bound")
        self.assertTrue(entries[2].exists())
        self.assertLessEqual(int((root / ledger.TOTAL).read_text()), 2500)


if __name__ == "__main__":
    unittest.main()
