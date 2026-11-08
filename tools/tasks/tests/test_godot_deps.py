# What a Godot run loads (tools/tasks/godot_deps.py) on a small project in a throwaway repo: each link Godot follows is
# reached, and what no link reaches is left out; the loads check of the Godot verdict; {loads} in the suites.
# Run: python -m unittest discover tools/tasks/tests
import json
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))  # support
sys.path.insert(0, str(Path(__file__).resolve().parent.parent.parent))  # tools: the tasks package

from support import RepoCase  # noqa: E402
from tasks import godot_deps, verdicts  # noqa: E402
from tasks.suites import Suites  # noqa: E402
from tasks.task import FAIL, PASS, Task  # noqa: E402

FILES = {
    "project/project.godot": '; a comment naming res://commented.gd\n[application]\nrun/main_scene="res://main.tscn"\n'
                             '[autoload]\nStore="*res://store.gd"\n',
    "project/store.gd": "extends Node\n",
    "project/main.tscn": '[ext_resource type="Script" path="res://ui.gd" id="1"]\n',
    "project/ui.gd": "extends Control\n",
    "project/commented.gd": "extends Node\n",
    "project/scene.tscn": '[ext_resource type="Script" path="res://s.gd" id="1"]\n'
                          '[ext_resource type="Script" uid="uid://abc123" id="2"]\n',
    "project/by_uid.gd": "extends Node\n",
    "project/by_uid.gd.uid": "uid://abc123\n",
    "project/s.gd": 'extends Node\nvar h: Helper\nconst S := preload("res://shaders/a.gdshader")\n'
                    'func f(n): return load("res://icons/%s.svg" % n)\n# UiThing in a comment\nvar t := "UiThing"\n'
                    'func g(): return Store\nconst DATA := "res://data"\nconst M := "res://m.gltf"\n'
                    'func ok(v): return v.begins_with("res://")\n',
    "project/helper.gd": "class_name Helper\nextends RefCounted\n",
    "project/ui_thing.gd": "class_name UiThing\nextends Control\n",
    "project/shaders/a.gdshader": 'shader_type spatial;\n#include "inc.gdshaderinc"\n',
    "project/shaders/inc.gdshaderinc": "// nothing\n",
    "project/icons/a.svg": "<svg/>\n",
    "project/icons/a.svg.import": '[remap]\nuid="uid://svg1"\n',
    "project/data/t.json": '{"texture": "res://tex.png"}\n',
    "project/tex.png": "not a picture\n",
    "project/m.gltf": json.dumps({"images": [{"uri": "m.png"}], "buffers": [{"uri": "m.bin"}]}),
    "project/m.png": "pixels\n",
    "project/m.bin": "bytes\n",
    "project/native.txt": "read by the core\n",
    "project/bin/core.gdextension": '[libraries]\nwindows.debug.x86_64 = "res://bin/core.dll"\n',
    "project/bin/core.dll": "library\n",
    "project/unreached.gd": "extends Node\n",
    "project/.godot/cache.cfg": "res://unreached.gd\n",
    "native/src/x.cpp": 'const char *P = "res://native.txt";\n',
}
REACHED = ["project/project.godot", "project/store.gd", "project/scene.tscn", "project/s.gd", "project/by_uid.gd",
           "project/by_uid.gd.uid", "project/helper.gd", "project/shaders/a.gdshader", "project/shaders/inc.gdshaderinc",
           "project/icons/a.svg", "project/icons/a.svg.import", "project/data/t.json", "project/tex.png", "project/m.gltf",
           "project/m.png", "project/m.bin", "project/native.txt", "project/bin/core.gdextension", "project/bin/core.dll"]
LEFT_OUT = ["project/main.tscn", "project/ui.gd", "project/commented.gd", "project/ui_thing.gd", "project/unreached.gd"]


class ProjectCase(RepoCase):
    def setUp(self):
        super().setUp()
        for rel, text in FILES.items():
            path = self.root / rel
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(text, encoding="utf-8")
        self.project = godot_deps.Project(self.root, "project", ("native/src",))


class ClosureTest(ProjectCase):
    def test_every_link_is_reached(self):
        got = set(self.project.closure(["res://scene.tscn"]))
        for f in REACHED:
            with self.subTest(file=f):
                self.assertIn(f, got)

    def test_what_no_link_reaches_is_left_out(self):
        got = set(self.project.closure(["res://scene.tscn"]))
        for f in LEFT_OUT:
            with self.subTest(file=f):
                self.assertNotIn(f, got)

    def test_main_scene_only_when_no_scene_is_named(self):
        self.assertIn("project/main.tscn", self.project.closure([]))
        self.assertIn("project/ui.gd", self.project.closure([]))

    def test_scheme_alone_names_a_path_only_when_one_is_built_on_it(self):
        self.assertEqual(self.project.literals('x.begins_with("res://") and "res://a.gd"'), ["res://a.gd"])
        self.assertEqual(self.project.literals('load("res://" + name)'), ["res://"])

    def test_loaded_reads_the_verbose_lines(self):
        log = ("Loading resource: res://s.gd remapped: res://s.gd\nLoading resource: res://.godot/imported/x.ctex\n"
               "Loading resource: res://tex.png\nother line\n")
        self.assertEqual(godot_deps.loaded(log, "project"), ["project/s.gd", "project/tex.png"])


class LoadsVerdictTest(ProjectCase):
    def judge(self, log: str, inputs: list[str]):
        task = Task(name="scene", argv=["godot"], timeout_s=1, inputs=inputs, params={"loads": "project"})
        return verdicts.undeclared_loads(task, log, self.root)[0]

    def test_all_declared_passes(self):
        self.assertEqual(self.judge("Loading resource: res://s.gd\n", ["project/s.gd"]), PASS)

    def test_an_undeclared_load_fails(self):
        self.assertEqual(self.judge("Loading resource: res://s.gd\nLoading resource: res://ui.gd\n", ["project/s.gd"]), FAIL)

    def test_a_log_with_no_load_fails(self):
        self.assertEqual(self.judge("nothing\n", ["project/s.gd"]), FAIL)


SUITES = {
    "vars": {"godot": {"windows": "godot", "posix": "godot"}},
    "versions": {"missing_version": {"argv": ["no-such-tool-anywhere-x9", "--version"]}},
    "loads": {"native": ["native/src"]},
    "changed": {"include": ["**"]},
    "sets": {"libs": ["project/bin/*.dll"]},
    "tasks": {
        "scene": {"argv": ["{godot}", "--verbose", "--path", "project", "res://scene.tscn"], "timeout_s": 5,
                  "inputs": ["{loads}", "!@libs"]},
        "quiet": {"argv": ["{godot}", "--path", "project", "res://scene.tscn"], "timeout_s": 5, "inputs": ["{loads}"]},
        "built": {"argv": ["{godot}"], "timeout_s": 5, "extra": ["{missing_version}"]},
    },
    "templates": {}, "groups": {}, "suites": {},
}


class SuitesLoadsTest(ProjectCase):
    def suites(self) -> Suites:
        path = self.root / "suites.json"
        path.write_text(json.dumps(SUITES), encoding="utf-8")
        return Suites(path, self.root)

    def test_loads_expands_to_the_closure_with_its_exclusions(self):
        task = self.suites().build("scene")
        self.assertIn("project/s.gd", task.inputs)
        self.assertIn("!project/bin/*.dll", task.inputs)
        self.assertEqual(task.params["loads"], "project")

    def test_loads_without_verbose_is_refused(self):
        with self.assertRaises(ValueError):
            self.suites().build("quiet")

    def test_a_missing_tool_version_is_refused_not_cached(self):
        with self.assertRaises(ValueError):
            self.suites().build("built")


if __name__ == "__main__":
    unittest.main()
