// What each walker is drawn in (people_renderer.h): the crouch's clip and phase from when it went down, and the pose remembered for a
// frame or two, that a body struck down falls from (body_renderer.cpp).
#include "render/agents/people_renderer.h"

#include "core/math.h"

#include <cmath>

namespace city {

// What the walker is drawn in, by what weighs most in it (people_pose.gdshaderinc person_pose): the walk at its phase while it walks more
// than it stands; else what it does (an act from its start, a crouch at its phase), else the idle.
void PeopleRenderer::remember_pose(uint32_t id, float rest, float doing, float walk) {
	if (id >= drawn_.size()) drawn_.resize(size_t(id) + 1);
	DrawnPose &d = drawn_[id];
	d.frame = frame_;
	if (rest < WALK_WEIGHS_MORE) {
		d.clip = walk_clip_;
		d.phase = walk;
	} else if (doing != 0.f) {
		d.clip = int(std::fabs(doing));
		d.phase = doing < 0.f ? walk : 0.f;
	} else {
		d.clip = idle_clip_;
		d.phase = 0.f;
	}
}

bool PeopleRenderer::drawn_pose(int id, int &clip, float &phase) const {
	if (id < 0 || size_t(id) >= drawn_.size()) return false;
	const DrawnPose &d = drawn_[size_t(id)];
	if (d.clip < 0 || frame_ - d.frame > DRAWN_LATELY) return false;
	clip = d.clip;
	phase = d.phase;
	return true;
}

PeopleRenderer::Played PeopleRenderer::crouch_at(int female, float since, float left) const {
	const PersonClip &down = asset_.clips[size_t(crouch_down_)], &up = asset_.clips[size_t(crouch_up_)];
	const PersonClip &stay = asset_.clips[size_t(act_clips_[int(PedAct::CROUCH)])];
	// the last frame is the end of a clip played once: a clip's phase 1 is its first frame again
	auto once = [](const PersonClip &c, float share) { return clampf(share, 0.f, 1.f) * float(c.frames - 1) / float(c.frames); };
	if (left < up.seconds[female]) return { crouch_up_, once(up, 1.f - left / up.seconds[female]) };
	if (since < down.seconds[female]) return { crouch_down_, once(down, since / down.seconds[female]) };
	const float turns = (since - down.seconds[female]) / stay.seconds[female];
	return { act_clips_[int(PedAct::CROUCH)], turns - std::floor(turns) };
}

} // namespace city
