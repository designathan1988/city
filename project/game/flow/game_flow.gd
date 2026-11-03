## GameFlow: the explicit state machine of the application. The only place that changes scenes or quits
## (gate FLUXO_CENTRAL). Every transition is validated. It holds the states the game
## has: the play scene (project main scene) generates a city and plays it; a main menu (Roadcraft's title screen) is not
## built yet (CLAUDE.md Pendências), so there is no menu nor loading state to go to.
extends Node

signal state_changed(from: State, to: State)

enum State { BOOT, PLAYING, QUITTING }

const TRANSITIONS := {
	State.BOOT: [State.PLAYING, State.QUITTING],
	State.PLAYING: [State.QUITTING],
	State.QUITTING: [],
}
const LOCALE := "pt_BR"

var state: State = State.BOOT
## Tests and captures drive the store directly (-- --no-flow): no menu flow, and their saves stay apart (SaveSlots).
var tests_drive := OS.get_cmdline_user_args().has("--no-flow")


func _ready() -> void:
	DataTables.load_translations()
	TranslationServer.set_locale(LOCALE)
	# closing the window is a request this machine answers (Godot "Handling quit requests"), so quitting has one path
	get_tree().set_auto_accept_quit(false)


func _notification(what: int) -> void:
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		quit()


func _to(next: State) -> bool:
	if not TRANSITIONS[state].has(next):
		push_error("GameFlow: transição inválida %s -> %s" % [State.keys()[state], State.keys()[next]])
		return false
	var prev := state
	state = next
	state_changed.emit(prev, next)
	return true


## The play scene has its city: the game is on.
func playing() -> void:
	_to(State.PLAYING)


func quit() -> void:
	if state == State.QUITTING or _to(State.QUITTING):
		get_tree().quit()


## Ends the process from a scene that runs without the menu flow (captures, tests).
func quit_now() -> void:
	state = State.QUITTING
	get_tree().quit()
