## The road tool's keys, rebindable (Roadcraft ui/roads/keys.ts): the height up and down and the alignment. What the game keeps for
## itself comes from data/ui/keys.json and the camera's table, so a key can only be given when nothing else answers it. Page Up and
## Page Down tilt the camera and are lent to the road by default. The player's choices live in Settings.road.keys.
class_name RoadKeys
extends RefCounted

const RAISE := "height_up"
const LOWER := "height_down"
const ALIGNMENT := "alignment"
const ACTIONS: Array[String] = [RAISE, LOWER, ALIGNMENT]
const DEFAULTS := {RAISE: KEY_PAGEUP, LOWER: KEY_PAGEDOWN, ALIGNMENT: KEY_V}
const LABEL_KEYS := {RAISE: "roadKeys.heightUp", LOWER: "roadKeys.heightDown", ALIGNMENT: "roadKeys.alignment"}
const LENT_BY_THE_CAMERA := [KEY_PAGEUP, KEY_PAGEDOWN]

## The action whose chip waits for a key, and the line under the chips.
static var waiting := ""
static var note := ""
const SHORT_NAMES := {KEY_PAGEUP: "PgUp", KEY_PAGEDOWN: "PgDn"}
const ALWAYS_THE_GAMES := [KEY_ESCAPE, KEY_ENTER, KEY_TAB, KEY_SPACE, KEY_DELETE, KEY_BACKSPACE, KEY_SHIFT, KEY_CTRL, KEY_ALT, KEY_META]


static func key_of(action: String) -> Key:
	return Settings.road.keys.get(action, DEFAULTS[action])


## The action a key asks for, or "".
static func action_of(code: Key) -> String:
	for action in ACTIONS:
		if key_of(action) == code:
			return action
	return ""


## Whether a key may be given to a road action: nothing else of the game answers it.
static func bindable(code: Key) -> bool:
	if code in ALWAYS_THE_GAMES or (CameraMotion.KEYS.has(code) and code not in LENT_BY_THE_CAMERA):
		return false
	var t: Dictionary = DataTables.ui("keys")
	var taken: Array = []
	for r: Dictionary in t.rules:
		taken.push_back(r.key)
	taken.append_array(t.tools.keys())
	taken.append_array(t.tabs.keys())
	taken.append_array(Array(t.digit_keys.split("", false)))
	return taken.all(func(name: String) -> bool: return OS.find_keycode_from_string(name) != code)


## Gives a key to an action; the action that had it takes this one's old key. False for a key the game keeps.
static func bind(action: String, code: Key) -> bool:
	if not bindable(code):
		return false
	var other := action_of(code)
	var old := key_of(action)
	var next: Dictionary = Settings.road.keys.duplicate()
	if other != "" and other != action:
		next[other] = old
	next[action] = code
	Settings.set_road_keys(next)
	return true


static func reset() -> void:
	Settings.set_road_keys({})


## A key as the interface writes it.
static func label(code: Key, short := false) -> String:
	return SHORT_NAMES[code] if short and SHORT_NAMES.has(code) else OS.get_keycode_string(code)


## "PgUp · PgDn · V": the keys in the tool's options.
static func summary() -> String:
	return " · ".join(ACTIONS.map(func(action: String) -> String: return label(key_of(action), true)))
