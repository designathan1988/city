## The signal cases of save_undo_test: a junction's signal through the store and the panel, with undo and redo.
class_name SignalCases
extends RefCounted

const SIGNAL_TICKS := 20
const SIGNAL_OFFSET := 7.0
const PANEL_BUTTONS := 2 # the mode and the green wave

var t: Node # the test (TestCase): its check, ticks and tree


func _init(test: Node) -> void:
	t = test


## A signal: fixed time takes hold on the next step, undo and redo bring the plan back, a wave sets its neighbours.
func run() -> void:
	t.ticks(SIGNAL_TICKS)
	var at: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
	var was: Dictionary = CityStore.query(&"inspect", [at])
	t.check(was.get("kind", "") == "node" and was.has("greens"), "signal inspected %s" % [was])
	if not was.has("greens"):
		return
	var r: Dictionary = CityStore.dispatch(&"set_signal", [was.id, true, was.greens, SIGNAL_OFFSET])
	var fixed_plan: Dictionary = CityStore.query(&"inspect", [at])
	t.check(r.ok and fixed_plan.fixed and fixed_plan.offset == SIGNAL_OFFSET, "fixed time %s" % [fixed_plan])
	t.ticks(SIGNAL_TICKS)
	t.check(CityStore.dispatch(&"undo") and not CityStore.query(&"inspect", [at]).fixed, "fixed time undone")
	t.check(CityStore.dispatch(&"redo") and CityStore.query(&"inspect", [at]).fixed, "fixed time redone")
	t.ticks(SIGNAL_TICKS)
	t.check(CityStore.dispatch(&"undo"), "back to adaptive")
	t.ticks(SIGNAL_TICKS)
	_panel(at, was)
	var wave: Dictionary = CityStore.dispatch(&"green_wave", [was.id])
	var after: Dictionary = CityStore.query(&"inspect", [at])
	print("green wave: ", wave, " ", after)
	t.check(wave.ok == after.fixed, "a green wave leaves the signal on fixed time, or says there is no other %s" % [wave])
	if wave.ok:
		t.check(CityStore.dispatch(&"undo") and not CityStore.query(&"inspect", [at]).fixed, "green wave undone in one step")
		t.ticks(SIGNAL_TICKS)


## The panel as the player meets it: drawn from what `inspect` reports, its first button taking the signal to fixed time.
func _panel(at: Vector3, info: Dictionary) -> void:
	var col := VBoxContainer.new()
	t.add_child(col)
	var redrawn := []
	JunctionPanel.fill(col, info, func() -> void: redrawn.append(true))
	var buttons := col.find_children("*", "Button", true, false)
	t.check(buttons.size() >= PANEL_BUTTONS, "signal panel with %d buttons" % buttons.size())
	(buttons[0] as Button).pressed.emit()
	t.check(not redrawn.is_empty() and CityStore.query(&"inspect", [at]).fixed, "the panel's button goes to fixed time")
	t.check(CityStore.dispatch(&"undo"), "panel undone")
	t.ticks(SIGNAL_TICKS)
	col.queue_free()
