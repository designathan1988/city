"""The area of the register a file belongs to, by its place in the tree (the register's areas are the schema's enum): the one
owner of that mapping, read by the import of the gates' findings and by the migration of the old trackers. The longest prefix
that matches wins; a file under no prefix is of the area `process` (the project's own files: CLAUDE.md, .claude, the root)."""
AREAS_BY_PLACE = (
    ("native/src/sim/traffic/", "traffic"),
    ("native/src/sim/transit/", "transit"),
    ("native/src/sim/roads/", "roads"),
    ("native/src/sim/street/", "street"),
    ("native/src/sim/lots/", "lots"),
    ("native/src/sim/citygen/", "lots"),
    ("native/src/sim/buildings/", "buildings"),
    ("native/src/sim/terrain/", "terrain"),
    ("native/src/sim/nature/", "nature"),
    ("native/src/sim/", "simulation"),
    ("native/src/core/", "simulation"),
    ("native/src/render/", "render"),
    ("native/src/api/", "render"),
    ("native/", "render"),
    ("project/ui/", "interface"),
    ("project/data/ui/", "interface"),
    ("project/data/i18n/", "interface"),
    ("project/game/", "interface"),
    ("project/render/", "render"),
    ("project/data/render/", "render"),
    ("project/data/catalog/", "simulation"),
    ("project/data/game/", "interface"),
    ("project/tests/", "tools"),
    ("project/", "product"),
    ("tools/", "tools"),
    ("log/", "docs"),
    ("specs/", "docs"),
    (".specify/", "docs"),
)
DEFAULT = "process"


def area_of(path):
    """The register's area of a repository path (forward slashes)."""
    path = path.replace("\\", "/")
    best = max((p for p, _a in AREAS_BY_PLACE if path.startswith(p)), key=len, default=None)
    return dict(AREAS_BY_PLACE)[best] if best else DEFAULT
