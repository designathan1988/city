## GameFlow: the explicit state machine of the application. The only place that changes scenes, pauses
## the simulation for menus or quits (gate FLUXO_CENTRAL). Every transition is validated.
extends Node

signal state_changed(from: State, to: State)

enum State { BOOT, MAIN_MENU, LOADING, PLAYING, PAUSED, QUITTING }

const TRANSITIONS := {
	State.BOOT: [State.MAIN_MENU],
	State.MAIN_MENU: [State.LOADING, State.QUITTING],
	State.LOADING: [State.PLAYING, State.MAIN_MENU],
	State.PLAYING: [State.PAUSED, State.MAIN_MENU, State.QUITTING],
	State.PAUSED: [State.PLAYING, State.MAIN_MENU, State.QUITTING],
	State.QUITTING: [],
}
const MAIN_MENU_SCENE := "res://ui/menus/main_menu.tscn"
const GAME_SCENE := "res://game/game.tscn"
const SAVE_DIR := "user://saves"
const SAVE_EXT := ".roadcraft"
const LOCALE := "pt_BR"

var state: State = State.BOOT
## What LOADING should do once the game scene is up: {"new": true, "map": int, "seed": int} or {"path": String}.
var pending: Dictionary = {}
var _speed_before_pause := 1


func _ready() -> void:
	DataTables.load_translations()
	TranslationServer.set_locale(LOCALE)
	DirAccess.make_dir_recursive_absolute(SAVE_DIR)
	if OS.get_cmdline_user_args().has("--no-flow"):
		return # tests drive the store directly
	_to(State.MAIN_MENU) # the play scene (project main scene) starts a generated city straight away


func _to(next: State) -> bool:
	if not TRANSITIONS[state].has(next):
		push_error("GameFlow: transição inválida %s -> %s" % [State.keys()[state], State.keys()[next]])
		return false
	var prev := state
	state = next
	state_changed.emit(prev, next)
	return true


func new_city(map: int, seed: int) -> void:
	pending = {"new": true, "map": map, "seed": seed}
	if _to(State.LOADING):
		get_tree().change_scene_to_file(GAME_SCENE)


func load_city(path: String) -> void:
	pending = {"path": path}
	if _to(State.LOADING):
		get_tree().change_scene_to_file(GAME_SCENE)


## Called by game/game.gd when the world is ready. Returns an error key or "".
func finish_loading() -> String:
	var error := ""
	if pending.get("new", false):
		if not CityStore.new_city(pending.map, pending.seed):
			error = "ERR_INVALID"
	else:
		error = CityStore.load_from(pending.get("path", ""))
	pending = {}
	if error.is_empty():
		_to(State.PLAYING)
	else:
		back_to_menu()
	return error


func pause() -> void:
	if state == State.PLAYING and _to(State.PAUSED):
		_speed_before_pause = int(CityStore.snapshot.get("speed", 1))
		CityStore.dispatch(&"set_speed", [0])


func resume() -> void:
	if state == State.PAUSED and _to(State.PLAYING):
		CityStore.dispatch(&"set_speed", [_speed_before_pause])


func back_to_menu() -> void:
	if _to(State.MAIN_MENU):
		get_tree().change_scene_to_file(MAIN_MENU_SCENE)


func quit() -> void:
	if _to(State.QUITTING):
		get_tree().quit()


## Ends the process from a scene that runs without the menu flow (captures, tests).
func quit_now() -> void:
	state = State.QUITTING
	get_tree().quit()


static func save_path(name: String) -> String:
	return "%s/%s%s" % [SAVE_DIR, name.validate_filename(), SAVE_EXT]


static func list_saves() -> PackedStringArray:
	var out := PackedStringArray()
	for f in DirAccess.get_files_at(SAVE_DIR):
		if f.ends_with(SAVE_EXT):
			out.append(f.trim_suffix(SAVE_EXT))
	return out
