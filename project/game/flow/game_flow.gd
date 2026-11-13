## GameFlow: the explicit state machine of the application. The only place that changes scenes or quits
## (gate CENTRAL_FLOW). Every transition is validated. It holds the states the game
## has: the play scene (project main scene) opens the last city or generates one, the title screen stands over it (MENU) until
## the first choice, and the game plays on.
extends Node

signal state_changed(from: State, to: State)
## Closing the window found the city's save failed: the game stays (the save's failure is already shown, HudActions) and the next
## close quits unsaved, which the HUD says (save.closeAgain).
signal close_held

enum State { BOOT, MENU, PLAYING, QUITTING }

const TRANSITIONS := {
	State.BOOT: [State.MENU, State.PLAYING, State.QUITTING],
	State.MENU: [State.PLAYING, State.QUITTING],
	State.PLAYING: [State.QUITTING],
	State.QUITTING: [],
}
const LOCALE := "pt_BR"
const FALLBACK_LOCALE := "en"

var state: State = State.BOOT
## Tests and captures drive the store directly (-- --no-flow): no menu flow, and their saves stay apart (SaveSlots).
var tests_drive := OS.get_cmdline_user_args().has("--no-flow")
## A test run in simulated time (--fixed-fps over 0, tools/tasks/suites.json test_fps): each frame is 1/fps s of the game whatever
## it takes, so nothing may hold a frame back to the wall clock.
var simulated_time := tests_drive and _fixed_fps() > 0
var _close_held := false # a close the failed save held back: the next one quits unsaved, until a save is written


static func _fixed_fps() -> int:
	var args := OS.get_cmdline_args()
	var at := args.find("--fixed-fps")
	return int(args[at + 1]) if at >= 0 and at + 1 < args.size() else 0


func _ready() -> void:
	DataTables.load_translations()
	TranslationServer.set_locale(Settings.language if Settings.language != "" else _system_locale())
	# closing the window is a request this machine answers (Godot "Handling quit requests"), so quitting has one path
	get_tree().set_auto_accept_quit(false)
	CityStore.saved.connect(_on_saved) # CityStore is the autoload before this one (project.godot)


## The player's own language where the game has it, English otherwise (the catalog's two columns).
func _system_locale() -> String:
	return LOCALE if OS.get_locale_language() == LOCALE.left(2) else FALLBACK_LOCALE


## The city goes with the window: the store writes what is unsaved first.
func _notification(what: int) -> void:
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		_close_requested()
	elif what == NOTIFICATION_APPLICATION_FOCUS_OUT and state == State.PLAYING and not tests_drive:
		CityStore.saves.save_on_hide()


## The window's close button: the game quits once the city is written (or nothing was due). A save that fails keeps the game on,
## its reason on screen, and a second close quits unsaved, as the hint says: the player chooses to lose the city, never the disk.
## That second close still lets a save started since (save_on_hide, the autosave) finish: it may be the one that writes the city.
func _close_requested() -> void:
	if state == State.PLAYING and not tests_drive:
		if _close_held:
			CityStore.saves.settle_in_flight()
		elif not CityStore.saves.write_before_leaving().get("ok", false):
			_close_held = true
			close_held.emit()
			return
	quit()


## A save written after a held close: the next close saves again before it quits.
func _on_saved(_slot: String, result: Dictionary) -> void:
	if result.get("ok", false):
		_close_held = false


func _to(next: State) -> bool:
	if not TRANSITIONS[state].has(next):
		push_error("GameFlow: invalid transition %s -> %s" % [State.keys()[state], State.keys()[next]])
		return false
	var prev := state
	state = next
	state_changed.emit(prev, next)
	return true


## The title screen is up.
func menu() -> void:
	_to(State.MENU)


## The play scene has its city: the game is on.
func playing() -> void:
	_to(State.PLAYING)


func quit() -> void:
	if state == State.QUITTING or _to(State.QUITTING):
		get_tree().quit()


## Ends the process from a scene that runs without the menu flow (captures, tests): through the table, as every way out
## (every state leads to QUITTING), so state_changed is heard there too.
func quit_now() -> void:
	if state != State.QUITTING:
		_to(State.QUITTING)
	get_tree().quit()
