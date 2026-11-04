## The graphics tiers on the running game: a tier's table (data/game/settings.json "tiers") with the player's kept choices and
## the optimisations over it, and each of its fields on the renderer that owns it: the Viewport (MSAA, SMAA, 3D scale,
## anisotropy), the sun (shadows), the Environment (SSAO), the RenderingServer (shadow atlas, SSAO quality) and the core.
class_name QualityTier
extends RefCounted

const MSAA_MODES := [Viewport.MSAA_DISABLED, Viewport.MSAA_2X, Viewport.MSAA_4X, Viewport.MSAA_8X]
const FILTER_LEVELS := [Viewport.ANISOTROPY_DISABLED, Viewport.ANISOTROPY_2X, Viewport.ANISOTROPY_4X, Viewport.ANISOTROPY_8X,
	Viewport.ANISOTROPY_16X]
## Screen pixels per inch of a 1:1 display (Windows' 96 DPI): what a device pixel ratio is measured against.
const BASE_DPI := 96.0
## Four cascades tile an atlas of twice their side (PSSM 4 splits).
const ATLAS_PER_CASCADE := 2
const SSAO_QUALITY := RenderingServer.ENV_SSAO_QUALITY_MEDIUM


## The tier the GPU's name gives: the first rule that matches, else gpu_default.
static func tier_for_gpu(cfg: Dictionary, name: String) -> String:
	for rule: Array in cfg.gpu_rules:
		var re := RegEx.create_from_string(rule[0])
		if re != null and re.search(name) != null:
			return rule[1]
	return cfg.gpu_default


## The tier's fields with the kept choices over them; `shadows` kept 0 is no shadows, ao and smaa force their pass.
static func merged(tier: Dictionary, kept: Dictionary) -> Dictionary:
	var out := tier.duplicate()
	if kept.has("shadows"):
		out.shadows = int(kept.shadows) > 0
		if out.shadows:
			out.shadow_px = int(kept.shadows)
	if kept.has("ssao"):
		out.ssao = kept.ssao
	if kept.has("smaa"):
		out.smaa = kept.smaa
	return out


## The 3D scale the tier's pixel ratio cap allows on this screen (1 on a display of one pixel per CSS pixel).
static func cap_scale(pixel_ratio: float) -> float:
	var dpi := float(DisplayServer.screen_get_dpi())
	var ratio := maxf(1.0, dpi / BASE_DPI)
	return minf(1.0, pixel_ratio / ratio)


## Everything a tier sets, live.
static func apply(q: Dictionary, world: CityWorld, viewport: Viewport, dynamic_scale: float) -> void:
	world.sun.shadow_enabled = q.shadows
	world.sun.directional_shadow_max_distance = q.shadow_distance_m
	RenderingServer.directional_shadow_atlas_set_size(int(q.shadow_px) * ATLAS_PER_CASCADE, true)
	viewport.msaa_3d = MSAA_MODES[int(q.msaa)]
	viewport.screen_space_aa = Viewport.SCREEN_SPACE_AA_SMAA if q.smaa else Viewport.SCREEN_SPACE_AA_DISABLED
	viewport.anisotropic_filtering_level = FILTER_LEVELS[int(q.anisotropy)]
	viewport.scaling_3d_scale = cap_scale(q.pixel_ratio) * dynamic_scale
	world.environment.ssao_enabled = q.ssao
	var ao: Dictionary = DataTables.game("settings").ssao
	RenderingServer.environment_set_ssao_quality(SSAO_QUALITY, q.ssao_half, ao.adaptive_target, int(ao.blur_passes),
			ao.fadeout_from_m, ao.fadeout_to_m)
	CityStore.set_detail_scale(sqrt(float(q.triangle_share)))
