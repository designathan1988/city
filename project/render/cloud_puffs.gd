## The lobes of each placed cloud about its centre (render/sky_view.gd lays them): jittered by the cloud's id, so a cloud keeps its
## shape every visit, turned by its yaw, at its size. Worked once a sky_revision, not once a frame.
class_name CloudPuffs
extends RefCounted

const LOBES := 7
## What each lobe keeps: its offset from the cloud's centre on x and z, its height, its radius.
const FIELDS := 4
## The jitter's hash (a cloud's id seeds it): the base and step of a cloud's seed, a lobe's step within it, the salts of its three
## draws, and the integer hash's own multipliers, shifts and range (an xorshift-multiply mix of two 32-bit odd constants).
const ID_BASE := 1000003
const ID_STEP := 7919
const PUFF_SEED := 11
const SALT_X := 6
const SALT_Z := 7
const SALT_R := 8
const MIX_A := 374761393
const MIX_B := 668265263
const MIX_C := 1274126177
const SHIFT_A := 13
const SHIFT_B := 16
const WORD := 0xFFFFFFFF
const WORD_RANGE := 4294967296.0


## 0..1 hash of an integer and a salt.
static func hash01(i: int, salt: int) -> float:
	var h := ((i + 1) * MIX_A) ^ (salt * MIX_B)
	h = ((h ^ (h >> SHIFT_A)) * MIX_C) & WORD
	return float((h ^ (h >> SHIFT_B)) & WORD) / WORD_RANGE


## The lobes of `clouds` (the first `slots`) into `out`, FIELDS a lobe; `sky` is data/game/sky.json.
static func shape(clouds: Array, slots: int, sky: Dictionary, out: PackedFloat64Array) -> void:
	var shapes: Array = sky.puffs
	for i in mini(clouds.size(), slots):
		var c: Dictionary = clouds[i]
		var yaw: float = c.yaw
		var size: float = c.size
		var cs := cos(yaw)
		var sn := sin(yaw)
		var id := ID_BASE + int(c.id) * ID_STEP
		for k in LOBES:
			var p: Array = shapes[k]
			var ja: float = p[0] + (hash01(id * PUFF_SEED + k, SALT_X) - 0.5) * sky.puff_jitter
			var jb: float = p[2] + (hash01(id * PUFF_SEED + k, SALT_Z) - 0.5) * sky.puff_jitter
			var jr: float = p[3] * (sky.puff_least + sky.puff_vary * hash01(id * PUFF_SEED + k, SALT_R))
			var at := (i * LOBES + k) * FIELDS
			out[at] = (ja * cs - jb * sn) * size
			out[at + 1] = c.height + p[1] * size
			out[at + 2] = (ja * sn + jb * cs) * size
			out[at + 3] = jr * size
