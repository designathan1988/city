## The Diagnosis at work (the 4.0's doctor/measure.ts): where the frame goes now, and what one switch costs, measured as before,
## with it changed, and again undone, so the gap of the two outer windows is the noise. Holds the last report for the F7 panel. The
## switch is applied without saving: a measure leaves settings.cfg as it found it.
class_name DoctorRun
extends Node

signal changed

const MS_PER_S := 1000.0

var report := {}
var results := {} # switch key -> its comparison
var status := "" # the string key of what runs now
var busy := false
var _rules: Dictionary
var _meter: FrameMeter


func _ready() -> void:
	_rules = DataTables.game("render_switches").doctor
	_meter = FrameMeter.new(get_viewport())
	add_child(_meter)


## The switch that tests a stage ("": none).
func switch_of(stage: String) -> String:
	for band: Dictionary in _rules.stages:
		if band.id == stage:
			return band.get("switch", "")
	return ""


## Where the frame goes, the stages' clock on only for this.
func measure() -> void:
	if busy:
		return
	_begin("lab.status.measuring")
	_meter.clock_on(get_viewport().get_camera_3d(), _rules.stages)
	await _wait(_rules.settle_ms)
	var frames := await _frames(_rules.measure_ms)
	_meter.clock_off()
	report = CityStore.query(&"lab_diagnose", [frames, _rules])
	_end()


## What switch `key` costs: before, flipped, and back.
func compare(key: String) -> void:
	if busy:
		return
	_begin("lab.status.before")
	var was: bool = Settings.switches.is_on(key)
	await _wait(_rules.settle_ms)
	var before := await _frames(_rules.read_ms)
	_say("lab.status.after")
	await _flip(key, not was)
	var after := await _frames(_rules.read_ms)
	_say("lab.status.again")
	await _flip(key, was)
	var again := await _frames(_rules.read_ms)
	results[key] = CityStore.query(&"lab_compare", [before, after, again])
	_end()


func _begin(what: String) -> void:
	busy = true
	_say(what)


func _say(what: String) -> void:
	status = what
	changed.emit()


func _end() -> void:
	busy = false
	_say("")


func _flip(key: String, on: bool) -> void:
	Settings.switches.set_on(key, on)
	Settings.apply_now()
	await _settled()


## Until no pipeline has been made for a while (a change compiles them), at most `most_ms`, and a last moment.
func _settled() -> void:
	var count := FrameMeter.pipelines()
	var still := 0.0
	var spent := 0.0
	while spent < _rules.most_ms and still < _rules.still_ms:
		await _wait(_rules.step_ms)
		spent += _rules.step_ms
		var now := FrameMeter.pipelines()
		still = still + _rules.step_ms if now == count else 0.0
		count = now
	await _wait(_rules.tail_ms)


func _frames(ms: float) -> Dictionary:
	_meter.record()
	await _wait(ms)
	return _meter.frames()


func _wait(ms: float) -> void:
	await get_tree().create_timer(ms / MS_PER_S).timeout
