# What the session may run with no block open and whose word lifts a bound (.claude/hooks/port_looks.py); and the hooks from
# end to end: a block's whole life, from the look before it to its commit and a compaction, asked of the hook's real entry
# (port_state.py, as Claude Code calls it) in a project of its own, the block moved by the real tool (cli.py block).
import json
import os
import subprocess
import sys
import time
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support, the gate's own cases
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import RepoCase  # noqa: E402
from test_block import HISTORY  # noqa: E402
from test_port_gate import COMPACTED, HOOKS, RUN, GateCase, transcript  # noqa: E402

import port_looks  # noqa: E402

from tasks import block  # noqa: E402

ORDER = "TAKE ANOTHER PHOTO OF THE GAME AS IT IS"
BOUNDARY = {"type": "system", "subtype": "compact_boundary"}
SKILLS = ("port", "superpowers:verification-before-completion")


def call(name: str, given: dict, at: float | None = None) -> dict:
    """A tool's call in a session's record, at a time."""
    stamp = time.strftime("%Y-%m-%dT%H:%M:%S", time.gmtime(time.time() if at is None else at)) + ".000Z"
    return {"type": "assistant", "timestamp": stamp, "message": {"content": [{"type": "tool_use", "name": name, "input": given}]}}


def run(names: str) -> dict:
    return {"command": f"{RUN} {names}", "run_in_background": True}


class Looks(GateCase):
    def setUp(self):
        super().setUp()
        self.state("closed")
        self.made = transcript(self.folder, ("Skill", {"skill": "port"}))

    def test_with_no_block_the_game_is_looked_at_once(self):
        self.assertFalse(self.refused("Bash", run("near,e2e"), self.made))
        self.assertTrue(self.refused("Bash", run("far_cars"), self.made), "a second run to see")
        self.assertFalse(self.refused("Bash", run("lab_test --all --jobs 1"), self.made), "the one measure")
        self.assertTrue(self.refused("Bash", run("lab_test"), self.made))
        self.assertFalse(self.refused("Bash", {"command": "tools/build/build.sh core-windows", "run_in_background": True}, self.made))
        self.assertFalse(self.refused("Bash", run("tests --list"), self.made), "naming the tasks runs nothing")
        later = port_looks.STATE.stat().st_mtime + 5
        os.utime(port_looks.STATE, (later, later))
        self.assertFalse(self.refused("Bash", run("near"), self.made), "a block moved since: its own look")

    def test_more_than_a_look_runs_on_the_owners_order_only(self):
        for names in ("tests", "@looks", "looks", "a,b,c,d,e,f,g"):
            with self.subTest(names=names):
                self.assertTrue(self.refused("Bash", run(names), self.made))
        self.assertTrue(self.refused("Bash", {"command": "python tools/tasks/km24.py", "run_in_background": True}, self.made))
        self.state("verify")
        stood = transcript(self.folder, *self.stood)
        self.assertTrue(self.refused("Bash", {"command": "python tools/hunt/hunt.py", "run_in_background": True}, stood))
        self.assertFalse(self.refused("Bash", run("tests"), stood), "a block's own runs are the tool's to hold")

    def test_an_order_is_the_owners_own_sentence(self):
        order = {"command": f'python tools/tasks/cli.py block order "{ORDER}"'}
        verify = {"command": 'python tools/tasks/cli.py block verify --owner "run the whole suite this once"'}
        self.assertTrue(self.refused("Bash", order, self.made), "words the owner never wrote")
        self.assertTrue(self.refused("Bash", verify, self.made))
        said = [call("Skill", {"skill": "port"}),
                {"type": "user", "isCompactSummary": True, "message": {"content": f"the owner said {ORDER}"}},
                {"type": "user", "toolUseResult": {}, "message": {"content": [{"type": "tool_result", "content": ORDER}]}},
                call("Write", {"file_path": "x.md", "content": ORDER})]
        record = self.folder / "said.jsonl"
        for owner in ({"type": "queue-operation", "content": f"look: {ORDER.lower()}, let me see it"},
                      {"type": "attachment", "attachment": {"prompt": f"{ORDER},\n  LET ME SEE IT"}},
                      {"type": "user", "message": {"content": f"{ORDER} now"}}):
            with self.subTest(owner=owner["type"]):
                record.write_text("".join(json.dumps(e) + "\n" for e in said), encoding="utf-8")
                self.assertTrue(self.refused("Bash", order, str(record)), "a summary, a tool's result and a file are not the owner")
                record.write_text("".join(json.dumps(e) + "\n" for e in said + [owner]), encoding="utf-8")
                self.assertFalse(self.refused("Bash", order, str(record)))
        self.assertTrue(self.refused("Bash", {"command": 'python tools/tasks/cli.py block order "go on"'}, str(record)))

    def test_an_order_lets_one_run_pass(self):
        self.assertFalse(self.refused("Bash", run("near"), self.made))
        self.assertTrue(self.refused("Bash", run("near"), self.made))
        port_looks.ORDER.write_text("{}", encoding="utf-8")
        self.assertFalse(self.refused("Bash", run("near"), self.made))
        self.assertFalse(port_looks.ORDER.is_file(), "the order is spent")
        self.assertTrue(self.refused("Bash", run("near"), self.made))

    def test_the_photos_are_opened_after_they_were_taken(self):
        self.state("verify")
        commit = {"command": 'git commit -m "traffic"'}
        skill = ("Skill", {"skill": "superpowers:verification-before-completion"})
        taken = self.shot.stat().st_mtime
        record = self.folder / "seen.jsonl"
        for reads, refused in (((self.shot, taken + 60), (self.reference, taken + 60)), False), \
                              (((self.shot, taken - 60), (self.reference, taken + 60)), True), \
                              (((self.shot, taken + 60),), True), (((self.reference, taken + 60),), True):
            with self.subTest(reads=[(f.name, at - taken) for f, at in reads]):
                calls = [call(name, given) for name, given in (*self.stood, skill)]
                calls += [call("Read", {"file_path": str(f)}, at) for f, at in reads]
                record.write_text("".join(json.dumps(e) + "\n" for e in calls), encoding="utf-8")
                self.assertEqual(self.refused("Bash", commit, str(record)), refused)


class WholeLife(RepoCase):
    """The hooks as Claude Code calls them, on a project of their own."""

    def setUp(self):
        super().setUp()
        hooks = self.root / ".claude" / "hooks"
        hooks.mkdir(parents=True)
        for file in HOOKS.glob("port_*.py"):
            (hooks / file.name).write_bytes(file.read_bytes())
        (self.root / block.HISTORY).parent.mkdir(parents=True)
        (self.root / block.HISTORY).write_bytes(HISTORY.encode("utf-8"))
        (self.root / "doc" / "port.md").write_text("# Port\n\n## 1. Roads\n- [ ] roads, complete\n", encoding="utf-8")
        self.note = self.root / "doc" / "work" / "roads.md"
        self.record = self.root / "session.jsonl"
        self.entries = [{"type": "user", "message": {"content": "port the complete roads"}}]
        self.code = {"file_path": str(self.root / "native" / "src" / "sim" / "x.cpp"), "old_string": "a", "new_string": "b"}

    def did(self, *entries: dict) -> None:
        self.entries += entries
        self.record.write_text("".join(json.dumps(e) + "\n" for e in self.entries), encoding="utf-8")

    def refusal(self, tool: str, given: dict) -> str:
        """What the hook answers to this call: its refusal's words, or nothing."""
        payload = {"hook_event_name": "PreToolUse", "tool_name": tool, "tool_input": given, "transcript_path": str(self.record)}
        done = subprocess.run([sys.executable, str(self.root / ".claude" / "hooks" / "port_state.py"), "hook"],
                              input=json.dumps(payload).encode("utf-8"), capture_output=True, timeout=60)
        self.assertEqual(done.returncode, 0, done.stderr.decode("utf-8", "replace"))
        said = json.loads(done.stdout) if done.stdout.strip() else {}
        return said.get("hookSpecificOutput", {}).get("permissionDecisionReason", "")

    def tool(self, *args: str) -> int:
        """The block moved by the real tool."""
        cli = Path(__file__).resolve().parent.parent / "cli.py"
        return subprocess.run([sys.executable, str(cli), "block", *args, "--root", str(self.root)], capture_output=True,
                              timeout=120).returncode

    def test_a_block_from_the_look_before_it_to_its_commit(self):
        self.did(call("Skill", {"skill": "port"}))
        # no block: one look, a second only on the owner's own words
        self.assertEqual(self.refusal("Bash", run("near")), "")
        self.assertIn("already looked at", self.refusal("Bash", run("near")))
        order = {"command": f'python tools/tasks/cli.py block order "{ORDER}"'}
        self.assertIn("did not write", self.refusal("Bash", order))
        self.did({"type": "queue-operation", "content": ORDER + ", ONE NO OTHER PHOTO SHOWS"})
        self.assertEqual(self.refusal("Bash", order), "")
        self.assertEqual(self.tool("order", ORDER), 0)
        self.assertEqual(self.refusal("Bash", run("near")), "")
        self.assertIn("already looked at", self.refusal("Bash", run("near")))
        self.assertIn("only with its block open", self.refusal("Edit", self.code))
        # the block open: the note, then the sources, then the code; nothing runs
        self.assertEqual(self.tool("open", "roads"), 0)
        self.assertIn("has no note", self.refusal("Edit", self.code))
        self.note.parent.mkdir(parents=True)
        self.note.write_text("# roads\n\n## Now\n- open; next: the heights.\n", encoding="utf-8")
        self.assertIn("has not stood on its records", self.refusal("Edit", self.code))
        self.did(call("Read", {"file_path": str(self.note)}))
        self.assertIn("Context7", self.refusal("Edit", self.code))
        self.did(call("mcp__plugin_context7_context7__query-docs", {}), call("WebSearch", {}))
        self.assertEqual(self.refusal("Edit", self.code), "")
        self.assertIn("refused", block.refusal("run near", self.root, ["near"]) or "", "the tool runs nothing while it is open")
        self.assertIn("the session does its own work", self.refusal("Agent", {"prompt": "read the roads"}))
        # the verification: its run, the look photographed, the photos opened, the skill; then the commit
        block.reviewed(self.root)
        self.assertEqual(self.tool("verify"), 0)
        self.assertIn("only with its block open", self.refusal("Edit", self.code))
        self.assertEqual(self.tool("close"), 1, "a block closes on checks that ran")
        commit = {"command": 'git commit -m "roads"'}
        self.assertIn("photographed", self.refusal("Bash", commit))
        block.record({"near": "PASS", "zoom_ladder": "PASS"}, self.root)
        shot = self.root / "dist" / "captures" / "zoom_ladder" / "00-near.png"
        reference = self.root / "doc" / "look" / "far_town.png"
        for picture in (shot, reference):
            picture.parent.mkdir(parents=True)
            picture.write_bytes(b"png")
        self.assertIn("not looked at", self.refusal("Bash", commit))
        self.did(call("Read", {"file_path": str(shot)}, time.time() + 5), call("Read", {"file_path": str(reference)}))
        self.assertIn("verification-before-completion", self.refusal("Bash", commit))
        self.did(call("Skill", {"skill": "superpowers:verification-before-completion"}))
        self.assertEqual(self.refusal("Bash", commit), "")
        # a compaction: nothing before the skill and the note again
        self.did(BOUNDARY)
        self.assertIn("has not stood on its records", self.refusal("Bash", {"command": "git status"}))
        self.did(call("Skill", {"skill": "port"}), call("Read", {"file_path": str(self.note)}))
        self.assertEqual(self.refusal("Bash", {"command": "git status"}), "")
        self.assertEqual(self.tool("close"), 0)
        self.did(BOUNDARY)
        self.assertIn("no block is open", self.refusal("Bash", {"command": "git status"}), "a compaction, whatever the block")


if __name__ == "__main__":
    unittest.main()
