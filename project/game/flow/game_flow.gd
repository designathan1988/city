## GameFlow: the explicit state machine of the application. The only place that changes scenes or quits
## (gate FLUXO_CENTRAL). Every transition is validated. It holds the states the game
## has: the play scene (project main scene) opens the last city or generates one, the title screen stands over it (MENU) until
## the first choice, and the game plays on.
extends Node

signal state_changed(from: State, to: State)

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


static func _fixed_fps() -> int:
	var args := OS.get_cmdline_args()
	var at := args.find("--fixed-fps")
	return int(args[at + 1]) if at >= 0 and at + 1 < args.size() else 0


func _ready() -> void:
	DataTables.load_translations()
	TranslationServer.set_locale(Settings.language if Settings.language != "" else _system_locale())
	# closing the window is a request this machine answers (Godot "Handling quit requests"), so quitting has one path
	get_tree().set_auto_accept_quit(false)


## The player's own language where the game has it, English otherwise (the catalog's two columns).
func _system_locale() -> String:
	return LOCALE if OS.get_locale_language() == LOCALE.left(2) else FALLBACK_LOCALE


## The city goes with the window (Roadcraft saveOnClose): the store writes what is unsaved first.
func _notification(what: int) -> void:
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		if state == State.PLAYING and not tests_drive:
			CityStore.saves.save_on_close()
		quit()
	elif what == NOTIFICATION_APPLICATION_FOCUS_OUT and state == State.PLAYING and not tests_drive:
		CityStore.saves.save_on_hide()


func _to(next: State) -> bool:
	if not TRANSITIONS[state].has(next):
		push_error("GameFlow: transição inválida %s -> %s" % [State.keys()[state], State.keys()[next]])
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
