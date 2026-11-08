// The ground a crater heaved up round it: broken slabs of the surface tipped at the lip of the hole,
// plates of the ground thrown up and tilted round it, clods between them. Laid at rest and never moved, from the damage state's
// craters: each crater's pieces are drawn from its own seed, so an opened save, an undo or a redo lays the same ones, as the
// craters' decals (marks.h); a blow lays only its own. Two layers of the debris material (debris.gdshader): the jagged plate and
// the unit box. Out to the crater quad's reach (Marks::CRATER_SCALE), the look of effects.json "heave" (HeaveLook).
#pragma once

#include "render/effects/effect_params.h"
#include "render/effects/mm_layer.h"
#include "sim/demolition/damage_state.h"
#include "sim/state/city_state.h"

#include <godot_cpp/variant/quaternion.hpp>

#include <vector>

namespace city {

class Heave {
public:
	void init(const godot::RID &scenario, const godot::RID &material, const HeaveLook &look);
	void reset();
	void release();
	// The craters as the state has them, on the ground as it stands (ground_cells.h). `kept`: how many of the first are those already
	// laid (the list only grew past them since); none when the ground under them moved since (ground_moved).
	void set_craters(const std::vector<Crater> &list, size_t kept, const CityState &s);
	// The terrain moved where `moved(x, z, half)` says: a crater whose heave reaches there is laid again at the next set_craters.
	template <typename T>
	void ground_moved(T &&moved) {
		for (const Laid &c : laid_)
			if (moved(c.x, c.z, c.reach)) stale_ = true;
	}
	size_t craters() const { return stale_ ? 0 : laid_.size(); }
	int pieces() const { return int(pieces_.size()); }

private:
	struct Piece {
		godot::Vector3 at, size;
		godot::Quaternion turn;
		godot::Color tint;
		bool plate = false; // the jagged plate, else a box
	};
	struct Laid {
		float x = 0, z = 0, reach = 0;
		size_t first = 0; // its first piece
	};
	void lay(const Crater &c, const CityState &s);
	void flush();
	HeaveLook look_;
	std::vector<Piece> pieces_;
	std::vector<Laid> laid_;
	bool stale_ = false;
	MmLayer plates_, boxes_;
};

} // namespace city
