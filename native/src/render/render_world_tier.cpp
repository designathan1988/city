// The graphics tier in RenderWorld (render/render_tier.h): each part kept when it is given and handed to the renderer that owns
// it at once when the renderers stand, and all the parts given so far when init has just made them.
#include "render/render_world.h"

namespace city {

bool RenderWorld::stand(bool made) {
	made_ = made;
	if (!made_) return false;
	if (tier_.lamps) apply_lamps();
	if (tier_.agents) apply_agents();
	if (tier_.detail_scale) buildings_.set_detail_scale(*tier_.detail_scale);
	if (tier_.tree_share) trees_.set_share(*tier_.tree_share);
	return true;
}

void RenderWorld::set_lamp_budget(int count, int casting, int heads) {
	tier_.lamps = RenderTier::Lamps{ count, casting, heads };
	if (made_) apply_lamps();
}

void RenderWorld::set_agent_detail(float cabin_m, int cabin_cap, int people_floor) {
	tier_.agents = RenderTier::Agents{ cabin_m, cabin_cap, people_floor };
	if (made_) apply_agents();
}

void RenderWorld::set_detail_scale(float scale) {
	tier_.detail_scale = scale;
	if (made_) buildings_.set_detail_scale(scale);
}

void RenderWorld::set_tree_share(float share) {
	tier_.tree_share = share;
	if (made_) trees_.set_share(share);
}

// The pool of the street lamps is given its budget only here, after its look (PropRenderer::init): a casting budget with no shadow
// look is an error of the pool.
void RenderWorld::apply_lamps() {
	props_.set_lamp_budget(tier_.lamps->count, tier_.lamps->casting);
	agents_.set_head_lights(tier_.lamps->heads);
}

void RenderWorld::apply_agents() { agents_.set_detail(tier_.agents->cabin_m, tier_.agents->cabin_cap, tier_.agents->people_floor); }

} // namespace city
