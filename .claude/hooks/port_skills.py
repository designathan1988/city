"""The domain skills a kind of the game's files is written with (CLAUDE.md, 'Skills are used, not named'; asked by
port_gate.py at an edit of the game's code): for the path an edit writes, the skills whose rules and pitfalls hold for it,
and whether the session used one of them since its start or its last compaction. A skill is used when it was invoked with
the Skill tool, or when its SKILL.md was Read whole (a plugin installed while a session runs is not in that session's
listing: its text is read from the disk). A kind none of whose skills is on this machine asks nothing.

The skills, by where they come from: `godot-skills@godot-claude-code-skills` (enabled by the project's settings; every
example run against Godot 4.7.2), `godot-prompter@skillsmith` (the user's), `cpp-pro` (the project's own folder, MIT)."""

import os
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CACHE = "CITY_PLUGINS"   # the environment's name of another plugin folder (the tests' own)
# what the files are; the paths, from the project's root; the skills, one of which is enough
KINDS = (
    ("a shader", re.compile(r"\.(?:gdshader|gdshaderinc|glsl)$"), ("godot-shaders-vfx", "shader-basics")),
    ("the interface", re.compile(r"^project/ui/"), ("godot-ui",)),
    ("a test or a capture of the game", re.compile(r"^project/tests/"), ("godot-headless-testing", "godot-testing")),
    ("the renderer", re.compile(r"^native/src/render/"), ("godot-performance", "rendering-pipeline", "godot-optimization")),
    ("the extension's boundary", re.compile(r"^native/src/api/"), ("gdextension",)),
    ("the core's C++", re.compile(r"^native/src/.*\.(?:cpp|h)$"), ("cpp-pro",)),
)


def homes() -> list:
    """Where a skill's folder may stand: the project's own skills, then each installed plugin's."""
    cache = Path(os.environ.get(CACHE) or Path.home() / ".claude" / "plugins" / "cache")
    found = [ROOT / ".claude" / "skills"]
    if cache.is_dir():
        found += sorted(cache.glob("*/*/*/skills"))
    return found


def where(skill: str) -> Path | None:
    """The SKILL.md of a skill on this machine; none when it is not installed."""
    for home in homes():
        text = home / skill / "SKILL.md"
        if text.is_file():
            return text
    return None


def used(skill: str, did: dict) -> bool:
    """The session invoked the skill, or Read its text whole (did: port_gate.used)."""
    if any(name == skill or name.endswith(":" + skill) for name in did.get("skills", ())):
        return True
    tail = f"/skills/{skill}/skill.md"
    return any(path.endswith(tail) for path in did.get("read", {}))


def owed(inside: str, did: dict) -> str:
    """Why the file at `inside` (from the project's root) may not be edited yet: the first kind it is of whose skills are
    installed and none of which the session used; nothing when every kind is met."""
    for what, paths, skills in KINDS:
        if not paths.search(inside):
            continue
        texts = [(skill, where(skill)) for skill in skills]
        if not any(text for _, text in texts) or any(used(skill, did) for skill in skills):
            continue
        ways = "; ".join(f"`{skill}` ({text.as_posix()})" for skill, text in texts if text)
        return (f"CLAUDE.md, 'Skills are used, not named': {inside} is {what}, written with its domain skill. Since this "
                "session's start or its last compaction none was used. Invoke one with the Skill tool, or Read its SKILL.md "
                f"whole (no offset, no limit) and hold to its rules and pitfalls: {ways}. A step of it that runs a build, a "
                "test or a measure while the code is written, writes a plan or stops to ask does not apply here. Then make "
                "this edit again.")
    return ""
