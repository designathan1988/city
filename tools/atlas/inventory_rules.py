"""The inventory's shared rules (tools/atlas/inventory.py): the area of a feature by the words of its places (one small table of
words, the folder, module, key and table names of both games, to the shared areas; a place no word of the table names is
`unclassified`, reported, never hidden), the words of a name, a player's text and an id in their normal forms (what the pairing
compares), a language's code, a feature record, and a file's first comment (its own statement of its responsibility).
Rules only: no feature of either game is listed here."""
import locale
import re
import unicodedata

UNCLASSIFIED = "unclassified"
# The shared areas and the words that name them in either game's places: folders, file stems, text keys, table names. A
# phrase of two words is matched before its single words ("street furniture" before "street").
AREAS = {
    "roads": ("road", "roads", "streets", "carriageway", "lane", "lanes", "lanelets", "profile", "profiles", "grade", "bridge",
              "tunnel", "network", "marking", "markings", "highway", "avenue"),
    "junctions": ("junction", "junctions", "intersection", "intersections", "roundabout", "crossing", "crossings", "node",
                  "nodes", "approach", "turns", "corner", "uturn"),
    "signals": ("signal", "signals", "control", "conflict", "green"),
    "traffic": ("traffic", "vehicle", "vehicles", "car", "cars", "drive", "agent", "agents", "meso", "routing", "route",
                "routes", "lorry", "parking", "kerb", "fleet", "congestion"),
    "people": ("people", "person", "ped", "peds", "walker", "walkers", "pedestrian", "crowd", "ragdoll", "cast", "residents",
               "motion", "clip", "clips", "animation"),
    "transit": ("transit", "bus", "train", "metro", "tram", "station", "stations"),
    "buildings": ("building", "buildings", "b40", "interior", "interiors", "facade", "roof", "roofs", "massing", "blueprint",
                  "blueprints", "builder", "body", "bodies", "storey", "storeys", "floor", "floors", "architecture", "tower",
                  "model", "models"),
    "lots": ("lot", "lots", "citygen", "yard", "garden", "square", "squares", "plot"),
    "growth": ("growth", "grow"),
    "zones": ("zone", "zones", "zoning"),
    "terrain": ("terrain", "relief", "heightfield", "ground", "water", "nature", "tree", "trees", "landscape", "gully",
                "gullies", "biome", "cover", "ecology", "elements", "shore", "canyon"),
    "street furniture": ("street furniture", "street", "furniture", "pole", "poles", "lamp", "lamps", "barrier", "barriers",
                         "sign", "signs", "wire", "wires", "streetscape", "utilities", "utility", "prop", "props"),
    "effects": ("effect", "effects", "blast", "demolition", "demolish", "explosion", "fire", "ruin", "ruins", "debris", "wear",
                "strike", "shoot", "bulldoze", "bulldozer", "bomb", "pistol", "damage", "decay", "thunder"),
    "sky": ("sky", "daylight", "sun", "moon", "weather", "cloud", "clouds", "fog", "atmosphere", "night", "ambient"),
    "camera": ("camera", "view", "viewport", "cull"),
    "saves": ("save", "saves", "saved", "persistence", "load"),
    "interface": ("ui", "hud", "i18n", "panel", "shell", "overlay", "overlays", "theme", "icon", "icons", "menu", "inspector",
                  "tooltip", "dock", "drawer", "drawers", "keys", "help", "about"),
    "lab": ("lab", "probe", "probes", "perf", "health", "profiler", "doctor", "diagnose", "hunt", "bench", "recorder",
            "optimization", "optimizations", "metrics"),
}
WORD_AREA = {w: area for area, names in AREAS.items() for w in names}
ACTION_TYPES = ("tool", "mode", "action")  # an enumeration whose name ends in one of these words is a set of the player's actions
LANGUAGE_FILE = re.compile(r"^[a-z]{2}(?:[-_][A-Za-z]{2})?$")  # a file named by a language's code (en, pt-BR) holds its texts
WORD = re.compile(r"[A-Z]+\d*(?![a-z])|[A-Z]?[a-z]+(?:\d+[a-z]*)?|\d+")  # a word keeps its digits: i18n, b40, v2
PLACEHOLDER = re.compile(r"\{\w*\}|%[sd]")
NOT_ALNUM = re.compile(r"[^0-9a-z]+")
DESCRIPTION_CHARS = 240


def singular(word):
    """A word without the plural's last `s` (lamps, lamp; status stays): what two names compare when one says many."""
    return word[:-1] if len(word) > 3 and word.endswith("s") and not word.endswith("ss") and not word.endswith("us") else word


def words(text):
    """The lower-case words of a name, split on case, digits, underscores and any punctuation."""
    return [w.lower() for w in WORD.findall(text or "")]


def area_of(*places):
    """The first shared area a place names, in the order the places are given (each place: its whole words as one phrase,
    then each two-word phrase, then each word); None when none does."""
    for place in places:
        ws = words(place)
        for phrase in [" ".join(ws)] + [" ".join(ws[i:i + 2]) for i in range(len(ws) - 1)] + ws:
            if phrase in WORD_AREA:
                return WORD_AREA[phrase]
    return None


def module_area(rel):
    """A file's area: the first word of its own name (a file says more of itself than its folder: `traffic/ped_steps` is
    people's), then its deepest folder, then its whole name, then the folders above (a generic folder names nothing)."""
    parts = rel.replace("\\", "/").split("/")
    folders, stem = parts[:-1], parts[-1].split(".")[0]
    return area_of(*(words(stem)[:1] + folders[-1:] + [stem] + folders[-2::-1]))


def language_file(stem):
    """Whether a file's name is a language's code (en, pt-BR): its form, and a language Python's locale aliases know
    (locale.normalize returns a name it cannot normalize unchanged: a file named ui or io names no language)."""
    code = language(stem)
    return bool(LANGUAGE_FILE.match(stem)) and locale.normalize(code) != code


def language(code):
    """A language's code in one form: pt_BR, pt-BR and pt are `pt`."""
    return re.split(r"[-_]", code.strip())[0].lower()


def norm_text(text):
    """A player's text as the pairing compares it: placeholders out, accents out, case folded, punctuation and spacing one
    blank; empty when no letter is left (a number or a symbol pairs nothing)."""
    plain = unicodedata.normalize("NFD", PLACEHOLDER.sub(" ", text or ""))
    plain = "".join(ch for ch in plain if not unicodedata.combining(ch)).casefold()
    out = NOT_ALNUM.sub(" ", plain).strip()
    return out if any(ch.isalpha() for ch in out) else ""


def norm_id(name):
    """An id as the pairing compares it: its words in their order (roundabout, ROUNDABOUT and Roundabout are one id;
    build_road and buildRoad are one; road_build is another)."""
    return "_".join(words(name))


def names_actions(type_name):
    """Whether an enumeration's name says it holds the player's actions (its last word tool, mode or action)."""
    ws = words(type_name)
    return bool(ws) and ws[-1] in ACTION_TYPES


def camel(member):
    """An upper-case enumerator as a text key writes it: BUILD_LOT is buildLot."""
    ws = words(member)
    return ws[0] + "".join(w.capitalize() for w in ws[1:]) if ws else member


def label_keys(type_name, member):
    """The text keys a member of an enumeration of actions is labelled by, most specific first: the type's words joined
    with the member (ZoneMode brush: zone.mode.brush, zone.brush), then the member alone."""
    ws = words(type_name)
    out = [".".join(ws[:n] + [member]) for n in range(len(ws), 0, -1)]
    return out + [member]


def mark_uses(texts, uses):
    """Each text's uses (`uses`: key -> [path:line], the first in each file, in the order read): `uses`, the first as
    `used`, and its description; a key no literal names may still be composed as the program runs (`prefix.%s`)."""
    for f in texts:
        f["uses"] = list(uses.get(f["name"], ()))
        f["used"] = f["uses"][0] if f["uses"] else ""
        f["description"] = (f"used at {f['used']}" + (f" and in {len(f['uses']) - 1} more file(s)" if len(f["uses"]) > 1 else "")
                            if f["used"] else "no literal use of its key in the code read (it may be composed as it runs)")


def feature(game, area, kind, name, source, label="", description="", **extra):
    out = {"game": game, "area": area or UNCLASSIFIED, "kind": kind, "name": name, "label": label or "",
           "description": description or "", "source": source}
    out.update(extra)
    return out


def _clean(lines):
    text = " ".join(re.sub(r"^\s*(?:/\*\*?|\*/|\*|//+|##?|\"\"\")\s?", "", ln).rstrip("*/ ").strip() for ln in lines)
    text = re.sub(r"\s+", " ", text).strip()
    if len(text) > DESCRIPTION_CHARS:
        cut = text.rfind(". ", 0, DESCRIPTION_CHARS)
        text = text[:cut + 1] if cut > DESCRIPTION_CHARS // 3 else text[:DESCRIPTION_CHARS].rstrip() + "..."
    return text


PREAMBLE_HASH = ("#!", "@tool", "class_name", "extends")  # GDScript lines that may come before a file's first comment
PREAMBLE_C = ("#pragma", "#include", "\"use strict\"", "'use strict'")
IMPORT_END = re.compile(r"""\bfrom\s+['"]|^import\s+['"]|;\s*$""")


def first_comment(text, hash_comments=False):
    """A file's own statement of its responsibility: the first run of comment lines (a block comment whole), after what may
    come before it (a shebang, GDScript's class lines, #pragma, the imports); empty when code comes first."""
    marks = ("#",) if hash_comments else ("//", "/*")
    preamble = PREAMBLE_HASH if hash_comments else PREAMBLE_C
    head, inside, importing = [], False, False
    for ln in text.splitlines():
        s = ln.strip()
        if inside:
            head.append(s)
            inside = "*/" not in s
            continue
        if not head and not hash_comments and (importing or s.startswith("import ")):
            importing = not IMPORT_END.search(s)
            continue
        if not head and (not s or s.startswith(preamble)):
            continue
        if s.startswith(marks) and not s.startswith("#!"):
            head.append(s)
            inside = s.startswith("/*") and "*/" not in s[2:]
            continue
        break
    return _clean(head)
