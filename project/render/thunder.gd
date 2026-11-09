## The sound of a strike, made here: brown noise (a random walk, the rumble of air) through a low-pass filter. A near strike keeps
## its high crack; far off, only the low roll arrives, late by the speed of sound. Each clap has a lane of its own (a player on a
## bus with its filter), so claps overlap as the strikes do, and follows its envelope: a rise, a roll down, the far parts of the
## channel coming in later, then a long fade. All numbers come from data/game/sky.json.
class_name Thunder
extends Node

const BOLT_LANE := "Thunder" # the buses are this and the lane's number
const PCM_BYTES := 2
const PCM_TOP := 32767.0 # a 16-bit PCM sample's top
## A roll of the envelope as sky.json thunder_rolls and thunder_fade keep it: from when (s), a random span added to that (s), and
## how fast it heads for its gain (its time constant, s; the fade's is a share of the clap's length).
const ROLL_FROM := 0
const ROLL_RANDOM := 1
const ROLL_SHARE := 2
const ROLL_TAU := 3
const FADE_TAU := 2

var _d: Dictionary
var _noise_s := 0.0
var _lanes: Array[Clap] = []
var _next := 0 # the lane the next clap takes: the oldest


func _init(d: Dictionary) -> void:
	_d = d


func _ready() -> void:
	var noise := _noise()
	_noise_s = noise.get_length()
	for i in int(_d.max_strikes):
		var name := BOLT_LANE + str(i)
		if AudioServer.get_bus_index(name) < 0:
			AudioServer.add_bus()
			var bus := AudioServer.bus_count - 1
			AudioServer.set_bus_name(bus, name)
			AudioServer.add_bus_effect(bus, AudioEffectLowPassFilter.new())
		var lane := Clap.new()
		lane.filter = AudioServer.get_bus_effect(AudioServer.get_bus_index(name), 0)
		lane.player = AudioStreamPlayer.new()
		lane.player.bus = name
		lane.player.stream = noise
		add_child(lane.player)
		_lanes.append(lane)
	set_process(false)


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


## How long the sound of a strike `metres` away takes.
func delay_s(metres: float) -> float:
	return minf(_d.thunder_late_s, metres / _d.sound_ms)


## A clap for a strike `metres` away, at `volume` (0..1): it starts once its sound has come.
func play(metres: float, volume: float) -> void:
	if volume <= 0.0 or _lanes.is_empty():
		return
	var near: float = 1.0 / (1.0 + metres / _d.thunder_near_m)
	var c: Clap = _lanes[_next]
	_next = (_next + 1) % _lanes.size()
	c.player.stop()
	c.filter.cutoff_hz = _d.thunder_low_hz + _d.thunder_crack_hz * near * near
	c.age = -delay_s(metres)
	c.silent = _d.thunder_silent
	c.peak = maxf(_d.thunder_least_peak, volume * (_d.thunder_far_gain + _d.thunder_near_gain * near))
	c.rise = _d.thunder_rise_s + _d.thunder_rise_far_s * (1.0 - near)
	c.length = _d.thunder_length_s + _d.thunder_length_far_s * (1.0 - near)
	c.rolls.clear()
	for roll: Array in _d.thunder_rolls:
		c.rolls.append(Vector3(roll[ROLL_FROM] + randf() * roll[ROLL_RANDOM], roll[ROLL_SHARE] * c.peak, roll[ROLL_TAU]))
	var fade: Array = _d.thunder_fade
	c.rolls.append(Vector3(fade[ROLL_FROM] + randf() * fade[ROLL_RANDOM], c.silent, fade[FADE_TAU] * c.length))
	c.start = randf() * maxf(0.0, _noise_s - c.length) # no two claps the same stretch of the noise
	c.waiting = true
	set_process(true)


func _process(dt: float) -> void:
	var any := false
	for c in _lanes:
		if not c.waiting and not c.player.playing:
			continue
		c.age += dt
		if c.age >= c.length:
			c.waiting = false
			c.player.stop()
			continue
		any = true
		if c.age < 0.0:
			continue
		c.player.volume_db = linear_to_db(c.gain(c.age))
		if c.waiting:
			c.waiting = false
			c.player.play(c.start + c.age)
	set_process(any)


## One clap on its lane: where it is in its envelope.
class Clap:
	var player: AudioStreamPlayer
	var filter: AudioEffectLowPassFilter
	var age := 0.0 # seconds since its sound arrived (negative while it is on its way)
	var silent := 0.0 # the gain it rises from and fades to (an exponential never reaches nothing)
	var peak := 0.0
	var rise := 0.0
	var length := 0.0
	var start := 0.0 # where in the noise it plays from
	var waiting := false # its sound has not arrived yet
	var rolls: Array[Vector3] = [] # from (s), the gain it heads for, its time constant (s); in order of time

	## The exponential rise from silence to the peak.
	func risen(t: float) -> float:
		return silent * pow(peak / silent, minf(t / rise, 1.0)) if rise > 0.0 else peak

	## Its gain at `t`: risen, then each roll heading for its gain from the gain it found, as an audio parameter's
	## setTargetAtTime does (W3C Web Audio API 1.0, AudioParam setTargetAtTime: v(t) = V1 + (V0 - V1) e^(-(t - T0) / tau)).
	func gain(t: float) -> float:
		if rolls.is_empty() or t < rolls[0].x:
			return risen(t)
		var level := risen(rolls[0].x)
		for i in rolls.size():
			var r := rolls[i]
			if t < r.x:
				break
			var until := minf(t, rolls[i + 1].x) if i + 1 < rolls.size() else t
			level = r.y + (level - r.y) * exp(-(until - r.x) / r.z)
		return level
