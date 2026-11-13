# lifecycle: every worktree, copy, branch and stash classed, the ones older than their work failing the listing; clean holding
# each output folder to its bound (keep with the builds an open problem names kept, while_open, max_mb, whole_mb), listing and removing
# what no one declared, a dry run removing nothing; the task cache's ledger evicting the least recently used past its bound.
import json
import os
import shutil
import stat
import sys
import time
import unittest
from contextlib import redirect_stdout
from io import StringIO
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import RepoCase, git  # noqa: E402

from tasks import clean, ledger, lifecycle  # noqa: E402

OLD = "2026-01-01T00:00:00+00:00"
LIFECYCLE = {"lifecycle": {"stale_days": 7, "outputs": [
    {"path": "dist/builds", "owner": "deliver.sh", "keep": 2, "index": ["latest.txt"]},
    {"path": "dist/evidence", "owner": "atlas", "while_open": True},
    {"path": "dist/evidence/research", "owner": "research", "whole_mb": 1},
    {"path": "dist/quick", "owner": "quick", "max_mb": 1},
    {"path": "dist/build-x", "owner": "a build", "whole_mb": 1},
    {"path": "dist/small-x", "owner": "a build", "whole_mb": 1},
    {"path": "dist/trace", "owner": "skill", "migrating": "a later task"}]}}


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
        self.make("dist/trace/slug/ledger.json")
        self.make("dist/build-x/a/one.o", size=700_000)
        self.make("dist/build-x/b/two.o", size=700_000)
        self.make("dist/small-x/one.o", size=10)
        self.make("dist/stray/junk.txt")
        os.chmod(self.make("dist/stray/.git/objects/01/object"), stat.S_IREAD)  # git's objects are read-only
        open_one = {"id": "Ptools-1", "status": "open", "evidence": ["dist/builds/b1/Roadcraft.exe"]}
        self.make("doc/problems.jsonl").write_bytes((json.dumps(open_one) + "\n").encode())

    def run_clean(self, *flags):
        return quiet(clean.cmd_clean, ["--root", str(self.root), "--suites", str(self.suites), *flags])[1]

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
        self.assertTrue((self.root / "dist/trace/slug").exists(), "a migrating folder is left to its migration")
        self.assertFalse((self.root / "dist/build-x").exists(), "a piece written whole goes whole past its size")
        self.assertTrue((self.root / "dist/small-x/one.o").exists(), "and stays whole inside it")
        self.assertTrue((self.root / "dist/stray").exists(), "an unowned folder is listed, not removed, without --unowned")
        self.run_clean("--unowned")
        self.assertFalse((self.root / "dist/stray").exists(), "removed whole, its read-only files included")


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
