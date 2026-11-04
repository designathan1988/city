## The signal cases of save_undo_test: a junction's signal through the store and the panel, with undo and redo.
class_name SignalCases
extends RefCounted

const SIGNAL_TICKS := 20
const SIGNAL_OFFSET := 7.0
const PANEL_BUTTONS := 2 # the mode and the green wave

var _host: Node
var _check: Callable


func _init(host: Node, check: Callable) -> void:
	_host = host
	_check = check


## A signal: fixed time takes hold on the next step, undo and redo bring the plan back, a wave sets its neighbours.
func run() -> void:
	CityStore.step_ticks(SIGNAL_TICKS)
	var at: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
	var was: Dictionary = CityStore.query(&"inspect", [at])
	_check.call(was.get("kind", "") == "junction", "semáforo inspecionado %s" % [was])
	if was.get("kind", "") != "junction":
		return
	var r: Dictionary = CityStore.dispatch(&"set_signal", [was.id, true, was.greens, SIGNAL_OFFSET])
	var fixed_plan: Dictionary = CityStore.query(&"inspect", [at])
	_check.call(r.ok and fixed_plan.fixed and fixed_plan.offset == SIGNAL_OFFSET, "tempo fixo %s" % [fixed_plan])
	CityStore.step_ticks(SIGNAL_TICKS)
	_check.call(CityStore.dispatch(&"undo") and not CityStore.query(&"inspect", [at]).fixed, "tempo fixo desfeito")
	_check.call(CityStore.dispatch(&"redo") and CityStore.query(&"inspect", [at]).fixed, "tempo fixo refeito")
	CityStore.step_ticks(SIGNAL_TICKS)
	_check.call(CityStore.dispatch(&"undo"), "volta ao adaptativo")
	CityStore.step_ticks(SIGNAL_TICKS)
	_panel(at, was)
	var wave: Dictionary = CityStore.dispatch(&"green_wave", [was.id])
	var after: Dictionary = CityStore.query(&"inspect", [at])
	print("onda verde: ", wave, " ", after)
	_check.call(wave.ok == after.fixed, "onda verde deixa o semáforo em tempo fixo, ou diz que não há outro %s" % [wave])
	if wave.ok:
		_check.call(CityStore.dispatch(&"undo") and not CityStore.query(&"inspect", [at]).fixed, "onda verde desfeita num passo")
		CityStore.step_ticks(SIGNAL_TICKS)


## The panel as the player meets it: drawn from what `inspect` reports, its first button taking the signal to fixed time.
func _panel(at: Vector3, info: Dictionary) -> void:
	var col := VBoxContainer.new()
	_host.add_child(col)
	var redrawn := []
	JunctionPanel.fill(col, info, func() -> void: redrawn.append(true))
	var buttons := col.find_children("*", "Button", true, false)
	_check.call(buttons.size() >= PANEL_BUTTONS, "painel do semáforo com %d botões" % buttons.size())
	(buttons[0] as Button).pressed.emit()
	_check.call(not redrawn.is_empty() and CityStore.query(&"inspect", [at]).fixed, "botão do painel passa a tempo fixo")
	_check.call(CityStore.dispatch(&"undo"), "painel desfeito")
	CityStore.step_ticks(SIGNAL_TICKS)
	col.queue_free()
