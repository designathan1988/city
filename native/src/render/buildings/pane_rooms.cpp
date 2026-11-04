// What a pane draws: the seed of its room (one room, one lamp), the room seen through it and its frame. The room is a
// box round the pane; building_pane.gdshaderinc traces it.
#include "render/buildings/facade_bay.h"

namespace city::b40 {

namespace {
constexpr float ROOM_DEPTH_M = 4.5f, SIDE_ROOM_M = 0.4f;
constexpr uint32_t GOLDEN = 2654435761u, LEVEL_SALT = 0x9e3779b1u, BAY_BYTE = 0xff, SLOT_MASK = 0xffff;
constexpr int BAY_BYTE_SHIFT = 8, HASH_SHIFT = 15;
constexpr uint8_t FLOOR_WOOD = 0, FLOOR_STONE = 1;

uint16_t slot_of(uint32_t id, int level, int vol, uint32_t bay) {
	uint32_t h = (id * GOLDEN) ^ (uint32_t(level + 1) * LEVEL_SALT) ^ (uint32_t(vol + 1) * GOLDEN) ^ bay;
	return uint16_t((h ^ (h >> HASH_SHIFT)) & SLOT_MASK);
}
} // namespace

PaneLook pane_look(const Placed &pl, const BayCtx &c, float a0, float a1, float h0, float h1, bool dark, float frame) {
	PaneLook look;
	look.dark = dark;
	look.bay = uint8_t((c.hash >> BAY_BYTE_SHIFT) & BAY_BYTE);
	look.slot = slot_of(pl.id, c.level, c.vol, c.hash);
	look.trim = c.trim;
	look.frame = frame;
	const float hm = (h0 + h1) / 2;
	PaneRoom &r = look.room;
	r.back = r.ahead = (a1 - a0) / 2 + SIDE_ROOM_M;
	r.depth = ROOM_DEPTH_M;
	r.sill = hm;
	r.head = c.H - hm;
	r.floor = pl.b->use == USE_COMMERCIAL ? FLOOR_STONE : FLOOR_WOOD;
	return look;
}

} // namespace city::b40
