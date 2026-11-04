## The sound of a strike, made here: brown noise (a random walk, the rumble of air) through a low-pass filter on a bus of
## its own. A near strike keeps its high crack; far off, only the low roll arrives, late by the speed of sound.
class_name Thunder
extends Node

const LANE := &"Thunder"
const PCM_BYTES := 2
const PCM_TOP := 32767.0

var _d: Dictionary
var _player: AudioStreamPlayer
var _filter: AudioEffectLowPassFilter


func _init(d: Dictionary) -> void:
	_d = d


func _ready() -> void:
	if AudioServer.get_bus_index(LANE) < 0:
		AudioServer.add_bus()
		var bus := AudioServer.bus_count - 1
		AudioServer.set_bus_name(bus, LANE)
		AudioServer.add_bus_effect(bus, AudioEffectLowPassFilter.new())
	_filter = AudioServer.get_bus_effect(AudioServer.get_bus_index(LANE), 0)
	_player = AudioStreamPlayer.new()
	_player.bus = LANE
	_player.stream = _noise()
	add_child(_player)


func _noise() -> AudioStreamWAV:
	var rate := int(_d.thunder_rate_hz)
	var frames := int(rate * _d.thunder_noise_s)
	var bytes := PackedByteArray()
	bytes.resize(frames * PCM_BYTES)
	var r := RandomNumberGenerator.new()
	r.seed = int(_d.rng_seed)
	var last := 0.0
	for i in frames:
		last = (last + _d.thunder_step * r.randf_range(-1.0, 1.0)) / (1.0 + _d.thunder_step)
		bytes.encode_s16(i * PCM_BYTES, int(clampf(last * _d.thunder_gain, -1.0, 1.0) * PCM_TOP))
	var s := AudioStreamWAV.new()
	s.format = AudioStreamWAV.FORMAT_16_BITS
	s.mix_rate = rate
	s.data = bytes
	return s


## A clap for a strike `metres` away, at `volume` (0..1).
func play(metres: float, volume: float) -> void:
	if volume <= 0.0 or _player == null:
		return
	var near: float = 1.0 / (1.0 + metres / _d.thunder_near_m)
	_filter.cutoff_hz = _d.thunder_low_hz + _d.thunder_crack_hz * near * near
	_player.volume_db = linear_to_db(volume * (_d.thunder_far_gain + _d.thunder_near_gain * near))
	await get_tree().create_timer(minf(_d.thunder_late_s, metres / _d.sound_ms)).timeout
	_player.play(randf() * _d.thunder_noise_s * _d.thunder_start_share) # no two claps the same stretch of the noise
