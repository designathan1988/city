## The clouds' puffs for atmosphere.gdshader (SkyView lays them): each placed cloud's lobes about its centre, jittered by its id,
## turned by its yaw, at its size, shaped once a sky_revision; and each cloud's bound, look and puffs where the wind and the view put
## it, in the arrays the shader reads.
class_name CloudPuffs
extends RefCounted

## The clouds atmosphere.gdshader holds, and the puffs of each: the sizes of its arrays.
const SLOTS := 24
const LOBES := 7
## The clouds' integer hash: xxHash's PRIME32_5 and PRIME32_4 on the index and the salt, an odd multiplier between two xorshifts, a 32-bit
## word and its range.
const HASH_A := 374761393
const HASH_B := 668265263
const HASH_C := 1274126177
const HASH_SHIFT_A := 13
const HASH_SHIFT_B := 16
const WORD := 0xFFFFFFFF
const WORD_RANGE := 4294967296.0
## How far a cloud's bound reaches, and its centre, in its own size.
const BOUND_REACH := 1.12
## The hash of a puff's jitter: the same puffs every visit (a cloud's id seeds it).
const ID_BASE := 1000003
const ID_STEP := 7919
const PUFF_SEED := 11
const SALT_X := 6
const SALT_Z := 7
const SALT_R := 8
## What each lobe keeps in _shape: its offset from the cloud's centre on x and z, its height, its radius.
const PUFF_FIELDS := 4

var bounds := PackedVector4Array()
var puffs := PackedVector4Array()
var looks := PackedVector4Array()
## The clouds' shapes as placed (per sky_revision): each lobe's offset turned by the cloud's yaw, its height and radius; only the
## centre, carried by the wind and the view, is added each frame.
var _shape := PackedFloat64Array()


func _init() -> void:
	bounds.resize(SLOTS)
	puffs.resize(SLOTS * LOBES)
	_shape.resize(SLOTS * LOBES * PUFF_FIELDS)
	looks.resize(SLOTS)


## 0..1 hash of an integer and a salt.
static func hash01(i: int, salt: int) -> float:
	var h := ((i + 1) * HASH_A) ^ (salt * HASH_B)
	h = ((h ^ (h >> HASH_SHIFT_A)) * HASH_C) & WORD
	return float((h ^ (h >> HASH_SHIFT_B)) & WORD) / WORD_RANGE


## Each placed cloud's lobes about its centre: jittered by its id, turned by its yaw, at its size (`d`: data/game/sky.json).
func shape(clouds: Array, d: Dictionary) -> void:
	var lobes: Array = d.puffs
	for i in mini(clouds.size(), SLOTS):
		var c: Dictionary = clouds[i]
		var yaw: float = c.yaw
		var size: float = c.size
		var cs := cos(yaw)
		var sn := sin(yaw)
		var id := ID_BASE + int(c.id) * ID_STEP
		for k in LOBES:
			var p: Array = lobes[k]
			var ja: float = p[0] + (hash01(id * PUFF_SEED + k, SALT_X) - 0.5) * d.puff_jitter
			var jb: float = p[2] + (hash01(id * PUFF_SEED + k, SALT_Z) - 0.5) * d.puff_jitter
			var jr: float = p[3] * (d.puff_least + d.puff_vary * hash01(id * PUFF_SEED + k, SALT_R))
			var at := (i * LOBES + k) * PUFF_FIELDS
			_shape[at] = (ja * cs - jb * sn) * size
			_shape[at + 1] = c.height + p[1] * size
			_shape[at + 2] = (ja * sn + jb * cs) * size
			_shape[at + 3] = jr * size


## Cloud i's bound, look and puffs about (x, z), shown by `show`.
func lay_one(i: int, c: Dictionary, x: float, z: float, show: float) -> void:
	var size: float = c.size
	bounds[i] = Vector4(x, c.height + size * 0.5, z, size * BOUND_REACH)
	looks[i] = Vector4(c.height, c.density, show, 0.0)
	for k in LOBES:
		var at := (i * LOBES + k) * PUFF_FIELDS
		puffs[i * LOBES + k] = Vector4(x + _shape[at], _shape[at + 1], z + _shape[at + 2], _shape[at + 3])
