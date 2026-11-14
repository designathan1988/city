## A road being drawn by hand: the points a free stroke keeps, how long it has run, and the words and paint of its preview.
class_name RoadDraft
extends RefCounted

const DENSE_SAMPLES := 256
const MOST_SAMPLES := 4096
const SPARSE_SAMPLE_M := 1.0
const ROUND_TO_M := 10
const SHORTEST_STRETCH_M := 5.0 ## a stretch built some way is named from this length on


static func sample(draft: Dictionary, p: Vector3) -> void:
	var samples: PackedVector3Array = draft.samples
	if samples.size() >= MOST_SAMPLES:
		return
	if samples.size() >= DENSE_SAMPLES and p.distance_to(samples[-1]) < SPARSE_SAMPLE_M:
		return
	samples.push_back(p)
	draft.samples = samples


static func traveled(d: Dictionary) -> float:
	var total := 0.0
	var samples: PackedVector3Array = d.samples
	for i in range(1, samples.size()):
		total += samples[i].distance_to(samples[i - 1])
	return total + samples[-1].distance_to(d.end.position)


## Where a stroke begins: {position, lift, adopted}. With the height in hand not set by hand since the last road, a press
## on a road or a junction begins there whatever its height (`seen`: what the pointer's ray landed on) and `adopted` says
## the height in hand becomes its `lift`; set by hand, only what stands at that height is taken (`p`: the pointer at that
## height) and the road passes the rest. `px`: a pixel in metres at p and at seen; `cell`: the grid's cell and offset.
static func start(p: Vector3, seen: Vector3, px: Array, snap: Dictionary, cell: Array, by_hand: bool) -> Dictionary:
	if not by_hand:
		var met: Dictionary = CityStore.query(&"road_start", [seen, px[1], snap.on, snap.grid, cell[0], cell[1], true])
		if met.taken:
			met["adopted"] = true
			return met
	var plain: Dictionary = CityStore.query(&"road_start", [p, px[0], snap.on, snap.grid, cell[0], cell[1], false])
	plain["adopted"] = false
	return plain


## The paint the preview draws: none on a road without lines.
static func paint(profile: int) -> String:
	var p: Dictionary = CityStore.cached(&"road_profiles")[profile]
	if not p.painted:
		return ""
	return "yellow" if p.yellow else "white"


## "120 m · R$ 150.000 · bridge 80 m · embankment 40 m": the length in tens of metres, its cost, then each way it is built
## that is not plain ground, the longest first (the metres the price counts: the plan's `built`; a cutting is "with
## walls" when the tool lays them: `walls`), and whether its end stopped short of the height asked.
static func measure(plan: Dictionary, walls: bool) -> String:
	var tens := roundi(plan.length / ROUND_TO_M) * ROUND_TO_M
	var parts := [Formatters.format_length(tens), Formatters.format_cost(plan.price)]
	var built: Dictionary = plan.get("built", {})
	var kinds := built.keys().filter(func(k: String) -> bool: return built[k] >= SHORTEST_STRETCH_M)
	kinds.sort_custom(func(a: String, b: String) -> bool: return built[a] > built[b])
	for kind: String in kinds:
		var metres := maxi(ROUND_TO_M, roundi(built[kind] / ROUND_TO_M) * ROUND_TO_M)
		var key := "road.built.trench_walls" if kind == "trench" and walls else "road.built." + kind
		parts.append(TranslationServer.translate(key).format({"length": Formatters.format_length(metres)}))
	if plan.get("limited", false):
		parts.append(TranslationServer.translate("road.built.limited"))
	return " · ".join(parts)


## "+3.0 m · 4%": the height the road in hand ends at over the grade line and the grade between its two ends' heights; ""
## for a road drawn on the ground at both ends. The tag shows it in the warning's ink when the grade held the end (`limited`).
static func height_label(plan: Dictionary) -> String:
	var from: float = plan.get("lift_a", 0.0)
	var to: float = plan.get("lift_b", 0.0)
	if (is_zero_approx(from) and is_zero_approx(to)) or plan.length <= 0.0:
		return ""
	var height := ("+" if to > 0.0 else "") + Formatters.format_length_tenths(to)
	return TranslationServer.translate("road.draft.height").format({"height": height, "grade": Formatters.format_percent(absf(to - from) / plan.length)})
