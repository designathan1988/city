## The sound of a strike: brown noise made once, a clap cut from it for `play(metres, volume)`: the sound arrives late, a far strike
## keeps only the low roll (low-pass by nearness) and is softer. The weather (session landscape) calls it; one player, the clap's gain
## and filter set per strike on a bus of its own. Numbers: data/game/thunder.json.
class_name Thunder
extends Node

const AUDIO_LANE := &"Thunder"
const PCM_MAX := 32767.0 # a 16-bit sample

var _cfg: Dictionary
var _player: AudioStreamPlayer
var _filter: AudioEffectLowPassFilter


func _ready() -> void:
	_cfg = DataTables.game("thunder")
	var at := AudioServer.bus_count
	AudioServer.add_bus(at)
	AudioServer.set_bus_name(at, AUDIO_LANE)
	_filter = AudioEffectLowPassFilter.new()
	AudioServer.add_bus_effect(at, _filter)
	_player = AudioStreamPlayer.new()
	_player.bus = AUDIO_LANE
	_player.stream = _noise()
	add_child(_player)


## 16-bit brown noise: a random walk, the rumble of air.
func _noise() -> AudioStreamWAV:
	var frames := int(_cfg.rate_hz * _cfg.noise_s)
	var data := PackedByteArray()
	data.resize(frames * 2)
	var last := 0.0
	for i in frames:
		last = (last + _cfg.step * randf_range(-1.0, 1.0)) / _cfg.keep
		data.encode_s16(i * 2, int(clampf(last * _cfg.gain, -1.0, 1.0) * PCM_MAX))
	var wav := AudioStreamWAV.new()
	wav.format = AudioStreamWAV.FORMAT_16_BITS
	wav.mix_rate = int(_cfg.rate_hz)
	wav.data = data
	return wav


## A thunderclap for a strike `metres` away, at `volume` (0..1).
func play(metres: float, volume: float) -> void:
	if volume <= 0.0:
		return
	var near: float = 1.0 / (1.0 + metres / _cfg.near_m)
	_filter.cutoff_hz = _cfg.cutoff_low_hz + _cfg.cutoff_span_hz * near * near
	_player.volume_db = maxf(_cfg.floor_db, linear_to_db(volume * (_cfg.far_gain + _cfg.near_gain * near)))
	await get_tree().create_timer(minf(_cfg.max_delay_s, metres / _cfg.sound_ms)).timeout
	_player.play(randf() * _cfg.noise_s * _cfg.start_share)
