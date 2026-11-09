// Who is drawn as whom (the lab's render/citizenCasting.ts CastingRegistry): every walker is given one body of the cooked cast, of its
// sex and age and in a wardrobe its dress code allows, the first time it is drawn, and keeps it while the same person walks (the
// record's look hash). A party (AgentRecord::party, ::company) is dressed in one code, settled by its first member, and no two of it
// wear the same body. A newcomer takes the first body of its deck that nobody within near_m wears, else the one worn farthest off,
// so the sixteen bodies do not stand side by side. Who wears what nearby is looked up in a grid of near_m cells built from the
// walkers of the frame (the lab compared every wearer). Data: agents.json people.cast.
#pragma once

#include "render/agents/people_asset.h"
#include "render/data_reader.h"
#include "sim/state/pose_buffer.h"

#include <godot_cpp/variant/dictionary.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace city {

class PeopleCasting {
public:
	// `bodies`: the cooked cast, whose sex, age and number the data's bodies must agree with.
	bool init(const DataValue &cast, const std::vector<PersonVariation> &bodies);
	// The frame's walkers, each with its body (body()) cast once; the newcomers where nobody near wears the same.
	void cast(const std::vector<AgentRecord> &people);
	// After cast(): the body of walker `id`.
	int body(uint32_t id) const { return id < entries_.size() && entries_[id].body >= 0 ? entries_[id].body : 0; }
	// The body a person of `look` wears, or wore last (somebody struck down); for somebody never drawn the first of its deck.
	int worn(uint32_t look, int age) const;
	// The frame's walkers as cast (for the tests): how many, how many bodies they wear, the pairs within near_m in the same body,
	// those in a body not of their age, how many walk in a party, and those of a party in a wardrobe outside the party's code.
	godot::Dictionary census() const;

private:
	enum Band { YOUNG = 0, ADULT, SENIOR, CHILD, BANDS };
	struct Entry {
		uint32_t look = 0; // the person: a new look in the same slot is somebody else
		uint32_t party = 0; // its party's leader's look
		int body = -1;
		int code = 0; // the dress code it was cast in
		int age = 0; // PedAge
		int company = 0; // PedCompany
		float x = 0, z = 0;
	};
	// The bodies a person may wear under one code, best age first, and the elders' grown-up stand-ins (citizenCasting.ts candidates).
	struct Pool {
		std::vector<int> bodies, spare;
	};
	struct SoloCode {
		int code = 0, child = 0, weight = 0;
	};
	// A company's codes in order of preference, turned by the party's hash, and whether one in traditional_one_in wears the traditional first.
	struct Company {
		std::vector<int> codes;
		bool tradition = false;
	};
	// A party as the frame finds it: its settled code (-1 none yet), the bodies and wardrobes its members wear, its members.
	struct Group {
		int code = -1;
		uint32_t bodies = 0, wardrobes = 0;
		std::vector<uint32_t> members;
	};
	std::vector<Entry> entries_; // by walker id
	std::unordered_map<uint32_t, uint32_t> by_look_; // look -> walker id, for worn()
	std::vector<uint32_t> wardrobe_bit_; // each body's wardrobe as a bit
	std::vector<int> band_; // each body's age band
	std::vector<bool> female_; // each body's sex
	std::vector<std::string> code_names_;
	std::vector<uint32_t> codes_; // each code, a mask of wardrobe bits
	std::vector<SoloCode> solo_first_;
	std::vector<int> solo_then_;
	Company companies_[int(PedCompany::COUNT)];
	int solo_total_ = 0, traditional_ = 0, traditional_one_in_ = 1, traditional_shift_ = 0;
	float near_m_ = 1;
	std::vector<std::pair<int64_t, uint32_t>> grid_; // the frame's wearers: (cell, walker id) in order of cell
	std::vector<size_t> newcomers_; // indices into the frame's records
	std::unordered_map<uint32_t, size_t> group_of_; // a party's leader's look -> its place in groups_
	std::vector<Group> groups_;
	size_t groups_used_ = 0;
	std::vector<int> options_;
	Pool pool_;
	std::vector<float> nearest_; // by body: the nearest wearer to a newcomer, metres
	int64_t cell_of(float x, float z) const;
	// Each walker filed in `cells` (sorted by cell) in the 3 x 3 cells of near_m round (x, z): fn(walker id).
	template <typename Fn>
	void each_near(const std::vector<std::pair<int64_t, uint32_t>> &cells, float x, float z, Fn &&fn) const;
	int code_named(const DataValue &at, const std::string &name) const;
	// The codes a walker may be dressed in, in order of preference (citizenCasting.ts codesFor): by its own hash alone, by its
	// party's hash in a party.
	void company_codes(int company, uint32_t hash, bool child, std::vector<int> &out) const;
	void pool(bool female, int age, int code, Pool &out) const;
	// The first code that dresses this person alone, its pool in `out`: -1 when none does.
	int dress(uint32_t look, int age, Pool &out, std::vector<int> &options) const;
	static bool female_of(uint32_t look);
	Group &group(uint32_t party);
	int pick(Entry &e);
};

} // namespace city
