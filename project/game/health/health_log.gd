## The game's health: what went wrong while it ran, in a ring (the F9 monitor reads it). The engine's errors come through a
## Logger (Godot 4.5+, OS.add_logger: called from any thread, so entries wait in a mutex-held list for the main thread),
## slow frames from the frame time against long_frame_ms. Entries repeat as a count; those of the last session come back as
## "earlier". Limits and the budget: data/game/health.json.
extends Node

const KEPT_PATH := "user://health.json"
const STATUS_ORDER := ["ok", "slow", "broken"]
## The sections of the core's frame, as render_cost names them.

var entries: Array[Dictionary] = []
var changes := 0
var _pending: Array[Dictionary] = []
var _mutex := Mutex.new()
var _cfg: Dictionary
var _serial := 0
var _since_save := 0.0
var _saved_changes := 0
var _logger: EngineLog


## The engine's own errors and warnings.
class EngineLog:
	extends Logger
	var log_owner: Node

	func _init(owner_node: Node) -> void:
		log_owner = owner_node

	func _log_error(function: String, file: String, line: int, code: String, rationale: String, _editor_notify: bool,
			error_type: int, _script_backtrace: Array[ScriptBacktrace]) -> void:
		var text := rationale if rationale != "" else code
		var severity := "warning" if error_type == Logger.ERROR_TYPE_WARNING else "broken"
		log_owner.call("note_from_thread", "error", severity, text, "%s:%d %s" % [file, line, function])

	func _log_message(_message: String, _error: bool) -> void:
		pass


func _ready() -> void:
	_cfg = DataTables.game("health")
	_restore()
	_logger = EngineLog.new(self)
	OS.add_logger(_logger)


func _exit_tree() -> void:
	OS.remove_logger(_logger)
	_save()


func note_from_thread(kind: String, severity: String, message: String, detail: String) -> void:
	_mutex.lock()
	if _pending.size() < int(_cfg.kept):
		_pending.append({"kind": kind, "severity": severity, "message": message, "detail": detail})
	_mutex.unlock()


## One entry; the same message again counts up on the entry it repeats.
func record(kind: String, severity: String, message: String, detail := "", ms := 0) -> void:
	var now := Time.get_ticks_msec()
	changes += 1
	for e in entries:
		if not e.earlier and e.kind == kind and e.message == message:
			e.count += 1
			e.last_ms = now
			e.ms = maxi(e.ms, ms)
			return
	_serial += 1
	entries.push_front({"serial": _serial, "kind": kind, "severity": severity, "message": message, "detail": detail, "ms": ms,
		"count": 1, "wall": Time.get_time_string_from_system(), "last_ms": now, "earlier": false})
	if entries.size() > int(_cfg.kept):
		entries.pop_back()


## ok, slow or broken: the worst of the last status_window_s.
func status() -> String:
	var since := Time.get_ticks_msec() - int(_cfg.status_window_s * MS_PER_S)
	var worst := 0
	for e in entries:
		if e.earlier or e.last_ms < since or e.severity == "warning":
			continue
		worst = maxi(worst, STATUS_ORDER.find(e.severity))
	return STATUS_ORDER[worst]


func _process(delta: float) -> void:
	var taken: Array[Dictionary] = []
	_mutex.lock()
	taken = _pending
	_pending = []
	_mutex.unlock()
	for p in taken:
		record(p.kind, p.severity, p.message, p.detail)
	var ms := delta * MS_PER_S
	if ms >= _cfg.long_frame_ms and GameFlow.state == GameFlow.State.PLAYING:
		_slow_frame(ms)
	_since_save += delta
	if _since_save >= _cfg.save_every_s:
		_since_save = 0.0
		_save()


## A long frame, under the name of the section of the core that took most of it, the others over step_min_ms in the detail. With no
## section to blame the time went outside the game's loop.
func _slow_frame(ms: float) -> void:
	var cost: Dictionary = CityStore.query(&"render_cost")
	var top := ""
	var most := 0.0
	var steps := PackedStringArray()
	for part: String in CityStore.constants.frame_sections:
		var took := float(cost.get(part, 0.0))
		if took > most:
			top = part
			most = took
		if took >= _cfg.step_min_ms:
			steps.append("%s %d ms" % [part, int(took)])
	if top == "" or most < _cfg.step_min_ms:
		record("slow-frame", "slow", TranslationServer.translate("health.msg.slowFrameOutside"), "", int(ms))
		return
	var message := TranslationServer.translate("health.msg.slowFrame").format({"what": top})
	record("slow-frame", "slow", message, TranslationServer.translate("health.msg.steps").format({"list": " · ".join(steps)}), int(ms))


func _save() -> void:
	if changes == _saved_changes or GameFlow.tests_drive:
		return
	_saved_changes = changes
	var f := FileAccess.open(KEPT_PATH, FileAccess.WRITE)
	if f != null:
		f.store_string(JSON.stringify(entries.slice(0, int(_cfg.kept_saved))))


func _restore() -> void:
	if GameFlow.tests_drive: # what the player's last session logged is no part of a test's picture
		return
	var parsed: Variant = JSON.parse_string(FileAccess.get_file_as_string(KEPT_PATH)) if FileAccess.file_exists(KEPT_PATH) else null
	if parsed is Array:
		for e: Dictionary in parsed:
			e.earlier = true
			e.ms = int(e.get("ms", 0))
			entries.append(e)


const MS_PER_S := 1000.0
