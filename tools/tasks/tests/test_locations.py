# tools/tasks/locations: a tool is its environment variable, else that name in the machine's tools/tasks/local.json, else what
# its locator finds on the PATH; with none of them the selection stops naming what to set.
import json
import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package
from tasks import locations  # noqa: E402


class LocationsTest(unittest.TestCase):
    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(prefix="tasks-locations-"))
        self.local = self.dir / "local.json"
        for patch in (mock.patch.object(locations, "LOCAL", self.local), mock.patch.dict(os.environ)):  # the environment put back after
            patch.start()
            self.addCleanup(patch.stop)

    def test_environment_first(self):
        self.local.write_text(json.dumps({"GODOT": "from-local"}), encoding="utf-8")
        with mock.patch.dict(os.environ, {"GODOT": "from-env"}):
            self.assertEqual(locations.locate("godot"), "from-env")

    def test_local_file_next(self):
        self.local.write_text(json.dumps({"GODOT": "from-local"}), encoding="utf-8")
        with mock.patch.dict(os.environ, {}, clear=False):
            os.environ.pop("GODOT", None)
            self.assertEqual(locations.locate("godot"), "from-local")

    def test_path_last(self):
        os.environ.pop("GODOT", None)
        with mock.patch.object(locations.shutil, "which", lambda name: f"/bin/{name}" if name == "godot" else None):
            self.assertEqual(locations.locate("godot"), "/bin/godot")

    def test_missing_names_what_to_set(self):
        os.environ.pop("MINGW", None)
        with mock.patch.object(locations.shutil, "which", lambda name: None):
            with self.assertRaises(locations.Missing) as caught:
                locations.locate("mingw")
        self.assertIn("MINGW", str(caught.exception))
        self.assertIn("local.json", str(caught.exception))

    def test_git_bash_found_beside_git(self):
        git_root = self.dir / "Git"
        (git_root / "bin").mkdir(parents=True)
        (git_root / "bin" / "bash.exe").write_bytes(b"")
        (git_root / "ucrt64" / "bin").mkdir(parents=True)
        git = git_root / "ucrt64" / "bin" / "git.exe"
        git.write_bytes(b"")
        os.environ.pop("BASH_EXE", None)
        with mock.patch.object(locations.os, "name", "nt"), \
                mock.patch.object(locations.shutil, "which", lambda name: str(git) if name == "git" else None):
            self.assertEqual(locations.locate("bash"), (git_root / "bin" / "bash.exe").as_posix())

    def test_unknown_tool_is_refused(self):
        with self.assertRaises(KeyError):
            locations.locate("nothing")


if __name__ == "__main__":
    unittest.main()
