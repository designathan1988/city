// The element brush (see sim/nature/elements.h): a dab scatters instances of a kind over the disc, kept apart by the
// spacing of the brush, or takes away what stands there; every change goes through the journal, one undo step a stroke.
#include "core/math.h"
#include "sim/commands/nature_commands.h"
#include "core/random.h"
#include "sim/commands/commands.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <memory>

namespace city {

namespace {
constexpr uint32_t DAB_SALT = 0xe1e3e47u; // a stream's salt: any fixed odd number apart from the others
constexpr int DRAWS_A_SPOT = 3; // a spot too near another is drawn again this many times
const char *const FULL = "hint.element.full";

EditResult done() {
	EditResult r;
	r.ok = true;
	return r;
}
EditResult refused(const char *key) {
	EditResult r;
	r.error = key;
	return r;
}

using Items = std::vector<ElementItem>;

// Puts `items` on the map or takes them off (`lay`), marking the chunks the render reads them from.
void apply(CityState &st, const Items &items, bool lay) {
	for (const ElementItem &e : items) {
		if (lay) st.nature.elements.add(e);
		else st.nature.elements.remove(e);
		mark_nature(st, e.x, e.z, e.x, e.z);
	}
}

void journal(CityState &s, const Items &items, bool lay) {
	auto kept = std::make_shared<const Items>(items);
	apply(s, *kept, lay);
	j_nature(s, kept->size() * sizeof(ElementItem), [kept, lay](CityState &st, bool fwd) { apply(st, *kept, lay == fwd); });
	s.touch();
}

Items scatter(const CityState &s, float x, float z, float radius, int kind, const ElementBrush &b) {
	const ElementDef &d = s.catalog.nature.elements;
	Rng r{ hash_combine(hash_combine(s.seed ^ DAB_SALT, uint32_t(s.nature.elements.count())), uint32_t(int32_t(std::floor(x))) ^ uint32_t(int32_t(std::floor(z)))) };
	float wanted = b.density / d.area_m2 * PI_F * radius * radius * b.strength;
	int count = std::min(d.max_per_dab, int(wanted) + (r.f() < wanted - std::floor(wanted) ? 1 : 0));
	Items near, out;
	s.nature.elements.for_near(x, z, radius + b.spacing, [&](const ElementItem &e) {
		if (e.kind == kind) near.push_back(e);
	});
	for (int i = 0; i < count * DRAWS_A_SPOT && int(out.size()) < count; i++) {
		float dist = radius * std::sqrt(r.f()), a = r.f() * TAU_F;
		float px = x + std::cos(a) * dist, pz = z + std::sin(a) * dist;
		auto free = [&](const ElementItem &e) { return std::hypot(e.x - px, e.z - pz) >= b.spacing; };
		if (!s.terrain.in_bounds(px, pz) || !std::all_of(near.begin(), near.end(), free) || !std::all_of(out.begin(), out.end(), free)) continue;
		ElementItem e;
		e.x = px;
		e.z = pz;
		e.kind = kind;
		e.size = std::max(d.least_size_m, b.size * (1.f + b.variation * (r.f() * 2.f - 1.f)));
		e.yaw = r.f() * TAU_F;
		e.seed = r.f();
		e.intensity = b.intensity;
		out.push_back(e);
	}
	return out;
}
} // namespace

EditResult element_dab(CityState &s, double x, double z, double radius, int kind, const ElementBrush &b, bool erase) {
	const ElementDef &d = s.catalog.nature.elements;
	float fx = float(x), fz = float(z), r = float(radius);
	if (!s.terrain.in_bounds(fx, fz) || kind >= d.kinds || (kind < 0 && !erase)) return refused("tool.failed");
	if (erase) {
		Items gone;
		s.nature.elements.for_near(fx, fz, r, [&](const ElementItem &e) {
			if (kind < 0 || e.kind == kind) gone.push_back(e); // no kind: everything
		});
		if (!gone.empty()) journal(s, gone, false);
		return done();
	}
	Items laid = scatter(s, fx, fz, r, kind, b);
	if (int(s.nature.elements.count() + laid.size()) > d.max_items) return refused(FULL);
	if (!laid.empty()) journal(s, laid, true);
	return done();
}

EditResult clear_elements(CityState &s) {
	Items all = s.nature.elements.all();
	if (!all.empty()) journal(s, all, false);
	return done();
}

} // namespace city
