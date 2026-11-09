// Who is drawn as whom: the cast's wardrobes and dress codes read once, each walker's body chosen once (people_casting.h).
#include "render/agents/people_casting.h"

#include "core/random.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace city {

namespace {
// The wardrobes and age bands of the lab's cast (citizenCasting.ts Wardrobe, AgeBand), as the data names them.
constexpr const char *WARDROBE_NAMES[] = { "casual", "smart-casual", "business", "sport-casual", "traditional" };
constexpr const char *BAND_NAMES[] = { "young", "adult", "senior", "child" };
// A walker's sex is the look hash's bit 24: the byte from there was the body's before the casting (people_renderer.cpp), and no
// other reader of the hash takes it (the acts its low bits, people_renderer.cpp its low ten, effect_bodies.cpp bits 0 to 21).
constexpr uint32_t LOOK_SEX_SHIFT = 24;
constexpr int CELL_BITS = 32; // a grid cell's key: its x cell in the high word, its z cell in the low
constexpr uint64_t LOW_WORD = 0xFFFFFFFFull;

int64_t cell_key(int64_t cx, int64_t cz) { return int64_t((uint64_t(cx) << CELL_BITS) ^ (uint64_t(cz) & LOW_WORD)); }
} // namespace

bool PeopleCasting::female_of(uint32_t look) { return ((look >> LOOK_SEX_SHIFT) & 1u) != 0; }

int PeopleCasting::code_named(const DataValue &at, const std::string &name) const {
	for (size_t i = 0; i < code_names_.size(); i++)
		if (code_names_[i] == name) return int(i);
	at.fail(String("no code named ") + name.c_str());
	return 0;
}

bool PeopleCasting::init(const DataValue &cast, const std::vector<PersonVariation> &bodies) {
	entries_.clear();
	by_look_.clear();
	near_m_ = cast["near_m"].f();
	if (!(near_m_ > 0)) cast["near_m"].fail("a distance above zero");
	const DataValue codes = cast["codes"];
	const Array names = codes.keys();
	code_names_.clear();
	codes_.clear();
	for (int i = 0; i < names.size(); i++) {
		const std::string name = String(names[i]).utf8().get_data();
		const DataValue list = codes[name.c_str()];
		uint32_t mask = 0;
		for (int k = 0; k < list.size(); k++) {
			DataReader at = list[k].reader;
			mask |= 1u << at.index_of(WARDROBE_NAMES, list[k].utf8(), "wardrobe");
		}
		code_names_.push_back(name);
		codes_.push_back(mask);
	}
	const DataValue first = cast["solo_first"];
	solo_first_.clear();
	solo_total_ = 0;
	for (int i = 0; i < first.size(); i++) {
		const DataValue row = first[i];
		const SoloCode c{ code_named(row, row["code"].utf8()), code_named(row, row["child"].utf8()), row["weight"].whole() };
		if (c.weight <= 0) row["weight"].fail("a weight above zero");
		solo_first_.push_back(c);
		solo_total_ += c.weight;
	}
	if (solo_total_ <= 0) {
		first.fail("weights that add up above zero");
		return false;
	}
	const DataValue then = cast["solo_then"];
	solo_then_.clear();
	for (int i = 0; i < then.size(); i++) solo_then_.push_back(code_named(then, then[i].utf8()));
	const DataValue list = cast["bodies"];
	if (list.size() != int(bodies.size())) {
		list.fail("one body for each body of people.bin");
		return false;
	}
	wardrobe_bit_.assign(bodies.size(), 0);
	band_.assign(bodies.size(), 0);
	female_.assign(bodies.size(), false);
	for (size_t b = 0; b < bodies.size(); b++) {
		const DataValue row = list[int(b)];
		DataReader at = row.reader;
		wardrobe_bit_[b] = 1u << at.index_of(WARDROBE_NAMES, row["wardrobe"].utf8(), "wardrobe");
		band_[b] = at.index_of(BAND_NAMES, row["band"].utf8(), "age band");
		female_[b] = bodies[b].female;
		if ((band_[b] == CHILD) != bodies[b].child || (band_[b] == SENIOR) != bodies[b].senior) row["band"].fail("not the age of the body in people.bin");
	}
	nearest_.assign(bodies.size(), 0.f);
	if (!list.reader.errors.is_empty()) return false;
	// Everybody, of either sex and any age, is dressed by the codes a lone walker ends on (the lab draws nobody it cannot dress).
	for (int sex = 0; sex < 2; sex++)
		for (int age = 0; age < PED_AGES; age++) {
			bool dressed = false;
			for (int code : solo_then_) {
				pool(sex == 1, age, code, pool_);
				dressed = dressed || !pool_.bodies.empty();
			}
			if (!dressed) {
				then.fail("a code that dresses every sex and age");
				return false;
			}
		}
	return true;
}

void PeopleCasting::solo_codes(uint32_t hash, bool child, std::vector<int> &out) const {
	out.clear();
	int k = int(hash % uint32_t(solo_total_));
	for (const SoloCode &c : solo_first_) {
		if (k < c.weight) {
			out.push_back(child ? c.child : c.code);
			break;
		}
		k -= c.weight;
	}
	out.insert(out.end(), solo_then_.begin(), solo_then_.end());
}

void PeopleCasting::pool(bool female, int age, int code, Pool &out) const {
	out.bodies.clear();
	out.spare.clear();
	const auto add = [&](int band, std::vector<int> &to) {
		for (size_t b = 0; b < band_.size(); b++)
			if (band_[b] == band && female_[b] == female && (codes_[size_t(code)] & wardrobe_bit_[b]) != 0) to.push_back(int(b));
	};
	if (age == int(PedAge::CHILD)) add(CHILD, out.bodies);
	else if (age == int(PedAge::ELDER)) {
		add(SENIOR, out.bodies);
		add(ADULT, out.bodies.empty() ? out.bodies : out.spare); // a grown-up's body when the cast has no senior's in this code
	} else {
		add(YOUNG, out.bodies);
		add(ADULT, out.bodies);
	}
}

bool PeopleCasting::dress(uint32_t look, int age, Pool &out, std::vector<int> &options) const {
	solo_codes(hash_u32(look), age == int(PedAge::CHILD), options);
	for (int code : options) {
		pool(female_of(look), age, code, out);
		if (!out.bodies.empty()) return true;
	}
	return false;
}

int64_t PeopleCasting::cell_of(float x, float z) const {
	return cell_key(int64_t(std::floor(x / near_m_)), int64_t(std::floor(z / near_m_)));
}

int PeopleCasting::pick(uint32_t look, int age, float x, float z) {
	Pool &p = pool_;
	if (!dress(look, age, p, options_)) return 0; // init made sure somebody's codes always dress them
	// Who wears what within near_m: the 3 x 3 cells round it.
	std::fill(nearest_.begin(), nearest_.end(), std::numeric_limits<float>::infinity());
	const int64_t cx = int64_t(std::floor(x / near_m_)), cz = int64_t(std::floor(z / near_m_));
	for (int64_t i = cx - 1; i <= cx + 1; i++)
		for (int64_t j = cz - 1; j <= cz + 1; j++) {
			const int64_t key = cell_key(i, j);
			auto it = std::lower_bound(grid_.begin(), grid_.end(), std::make_pair(key, uint32_t(0)));
			for (; it != grid_.end() && it->first == key; ++it) {
				const Entry &e = entries_[it->second];
				float &n = nearest_[size_t(e.body)];
				n = std::min(n, std::hypot(e.x - x, e.z - z));
			}
		}
	const uint32_t hash = hash_u32(look);
	const size_t own = p.bodies.size(), deck = own + p.spare.size();
	int fallback = p.bodies[hash % own];
	float farthest = -1;
	for (size_t k = 0; k < deck; k++) {
		const bool spare = k >= own;
		const int body = spare ? p.spare[(hash + k) % p.spare.size()] : p.bodies[(hash + k) % own];
		const float near = nearest_[size_t(body)];
		if (near >= near_m_) return body;
		if (!spare && near > farthest) {
			farthest = near;
			fallback = body;
		}
	}
	return fallback;
}

void PeopleCasting::cast(const std::vector<AgentRecord> &people) {
	grid_.clear();
	newcomers_.clear();
	for (size_t i = 0; i < people.size(); i++) {
		const AgentRecord &rec = people[i];
		if (rec.id >= entries_.size()) entries_.resize(size_t(rec.id) + 1);
		Entry &e = entries_[rec.id];
		if (e.body < 0 || e.look != rec.look) {
			newcomers_.push_back(i);
			continue;
		}
		const Pose &at = rec.at[POSE_TRAIL - 1];
		e.x = at.x;
		e.z = at.z;
		grid_.emplace_back(cell_of(e.x, e.z), rec.id);
	}
	if (newcomers_.empty()) return;
	std::sort(grid_.begin(), grid_.end());
	for (size_t i : newcomers_) {
		const AgentRecord &rec = people[i];
		Entry &e = entries_[rec.id];
		if (e.body >= 0) {
			auto was = by_look_.find(e.look);
			if (was != by_look_.end() && was->second == rec.id) by_look_.erase(was);
		}
		const Pose &at = rec.at[POSE_TRAIL - 1];
		e.look = rec.look;
		e.x = at.x;
		e.z = at.z;
		e.age = rec.age < PED_AGES ? rec.age : int(PedAge::ADULT);
		e.body = pick(rec.look, e.age, e.x, e.z);
		by_look_[rec.look] = rec.id;
		const std::pair<int64_t, uint32_t> place(cell_of(e.x, e.z), rec.id);
		grid_.insert(std::upper_bound(grid_.begin(), grid_.end(), place), place);
	}
}

Dictionary PeopleCasting::census() const {
	std::vector<std::pair<int64_t, uint32_t>> cells = grid_; // every walker of the frame (cast())
	std::sort(cells.begin(), cells.end());
	uint32_t worn_mask = 0;
	int twins = 0, wrong_age = 0;
	for (const auto &place : cells) {
		const uint32_t id = place.second;
		const Entry &e = entries_[id];
		worn_mask |= 1u << uint32_t(e.body);
		const int band = band_[size_t(e.body)];
		const bool fits = e.age == int(PedAge::CHILD) ? band == CHILD : e.age == int(PedAge::ELDER) ? band == SENIOR || band == ADULT : band == YOUNG || band == ADULT;
		wrong_age += fits ? 0 : 1;
		const int64_t cx = int64_t(std::floor(e.x / near_m_)), cz = int64_t(std::floor(e.z / near_m_));
		for (int64_t i = cx - 1; i <= cx + 1; i++)
			for (int64_t j = cz - 1; j <= cz + 1; j++) {
				const int64_t key = cell_key(i, j);
				for (auto it = std::lower_bound(cells.begin(), cells.end(), std::make_pair(key, uint32_t(0))); it != cells.end() && it->first == key; ++it) {
					const Entry &o = entries_[it->second];
					if (it->second > id && o.body == e.body && std::hypot(o.x - e.x, o.z - e.z) < near_m_) twins++;
				}
			}
	}
	int bodies = 0;
	for (uint32_t m = worn_mask; m != 0; m &= m - 1) bodies++;
	Dictionary out;
	out["walkers"] = int(cells.size());
	out["bodies"] = bodies;
	out["twins"] = twins;
	out["wrong_age"] = wrong_age;
	return out;
}

int PeopleCasting::worn(uint32_t look, int age) const {
	auto it = by_look_.find(look);
	if (it != by_look_.end() && it->second < entries_.size() && entries_[it->second].look == look && entries_[it->second].body >= 0)
		return entries_[it->second].body;
	Pool p;
	std::vector<int> options;
	return dress(look, age, p, options) ? p.bodies[hash_u32(look) % p.bodies.size()] : 0;
}

} // namespace city
