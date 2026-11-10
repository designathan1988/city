## The F7 recorder (the 4.0's lab/recorder): the frames of a session, cut at every change of a switch or an option, so each stretch is measured on its
## own; the moment after a change compiles and loads, and stays out of the figures. The report goes to user://lab as JSON.
class_name LabRecorder
extends Node

signal changed

const MS_PER_S := 1000.0
const FOLDER := "user://lab"
const FILE := "%s/recording_%d.json"

var recording := false
var stretches: Array[Dictionary] = []
var saved := "" # where the last report went
var _meter: FrameMeter
var _cut := 0 # the change that started the stretch in hand; a later one overtakes the wait for it
var _started_ms := 0
var _measuring := false


func _ready() -> void:
	_meter = FrameMeter.new(get_viewport())
	add_child(_meter)


func start() -> void:
	if recording:
		return
	recording = true
	stretches = []
	saved = ""
	Settings.changed.connect(_on_change)
	_open()
	changed.emit()


## Ends the recording and writes its report; the path, "" when nothing was measured.
func stop() -> String:
	if not recording:
		return ""
	recording = false
	Settings.changed.disconnect(_on_change)
	_close()
	_cut += 1
	saved = _write()
	changed.emit()
	return saved


func _on_change() -> void:
	_close()
	_open()
	changed.emit()


## The next stretch starts once the change has settled.
func _open() -> void:
	_cut += 1
	var mine := _cut
	await get_tree().create_timer(DataTables.game("lab").recorder.settle_ms / MS_PER_S).timeout
	if mine != _cut or not recording:
		return
	_started_ms = Time.get_ticks_msec()
	_measuring = true
	_meter.record()


func _close() -> void:
	if not _measuring:
		return
	_measuring = false
	var s := _meter.record_end()
	s.seconds = float(Time.get_ticks_msec() - _started_ms) / MS_PER_S
	s.pipelines = FrameMeter.pipelines()
	stretches.append(s)


func _write() -> String:
	if stretches.is_empty():
		return ""
	DirAccess.make_dir_recursive_absolute(FOLDER)
	var path := FILE % [FOLDER, int(Time.get_unix_time_from_system())]
	var f := FileAccess.open(path, FileAccess.WRITE)
	if f == null:
		return ""
	f.store_string(JSON.stringify({"adapter": RenderingServer.get_video_adapter_name(), "method": RenderingServer.get_current_rendering_method(), "stretches": stretches}, " "))
	return path
