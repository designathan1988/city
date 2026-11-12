// Immutable masses, floor plans and pane rooms share a byte LRU. Cold readers prepare independently;
// metadata locks never cover geometry or destruction, and derived entries never pin their source body.
#include "render/buildings/body_cache.h"
#include "render/buildings/pane_rooms.h"
#include "core/job_pool.h"
#include "core/retained_memory.h"
#include "sim/buildings/b40/blocks.h"
#include "sim/buildings/b40/body_memory.h"

#include <cstring>
#include <list>
#include <map>
#include <mutex>
#include <tuple>

namespace city {
namespace {
uint32_t float_bits(float value) { uint32_t bits; std::memcpy(&bits, &value, sizeof bits); return bits; }
} // namespace

BodyIdentity body_identity(const Building &r, const b40::BodyInput &source) {
	return {source.generation, r.body, r.seed, r.floors, r.zone, r.fn, r.style, r.profile,
		uint64_t(int64_t(r.landmark)), float_bits(r.w), float_bits(r.d),
		uint64_t(reinterpret_cast<uintptr_t>(source.edited ? source.edited.get() : source.mass.get()))};
}

struct BodyCache::Impl : b40::BodyDerivedOwner, std::enable_shared_from_this<Impl> {
	// Source address first gives source retirement an ordered range, without a full-cache scan.
	using Key = std::tuple<uintptr_t, int, BodyIdentity, int, std::string, b40::PaneRoomKey>;
	enum Kind { MASS, FLOOR, PANE };
	struct Entry {
		std::shared_ptr<const void> value;
		size_t bytes;
		std::list<Key>::iterator recent;
	};
	struct MassRelease {
		std::weak_ptr<Impl> owner;
		void operator()(const b40::Body *body) const {
			if (auto cache = owner.lock()) cache->source_retired(body);
			delete body;
		}
	};
	explicit Impl(size_t bytes) : limit(bytes) {}
	const size_t limit;
	size_t retained = 0;
	std::mutex mutex;
	std::map<Key, Entry> entries;
	std::list<Key> recent;

	void source_retired(const b40::Body *source) override {
		std::vector<std::shared_ptr<const void>> released;
		const uintptr_t address = reinterpret_cast<uintptr_t>(source);
		{
			std::lock_guard<std::mutex> lk(mutex);
			auto it = entries.lower_bound(Key{address, MASS, {}, 0, {}, {}});
			while (it != entries.end() && std::get<0>(it->first) == address) {
				retained -= it->second.bytes;
				released.push_back(std::move(it->second.value));
				recent.erase(it->second.recent); it = entries.erase(it);
			}
		}
	}

	template<class T, class Make, class Bytes>
	std::shared_ptr<const T> memo(const Key &key, Make make, Bytes size_of) {
		{
			std::lock_guard<std::mutex> lk(mutex);
			const auto found = entries.find(key);
			if (found != entries.end()) {
				recent.splice(recent.end(), recent, found->second.recent);
				return std::static_pointer_cast<const T>(found->second.value);
			}
		}
		std::shared_ptr<const T> made = make();
		// Both ordered-map and list retain their own key/string; active leases are separate explicit roots.
		const size_t bytes = size_of(*made) + sizeof(Entry) + 2 * sizeof(Key) + 9 * sizeof(void *)
			+ 2 * (std::get<4>(key).capacity() + 1);
		if (bytes > limit || JobPool::cancelled()) return made;
		std::vector<std::shared_ptr<const void>> released;
		{
			std::lock_guard<std::mutex> lk(mutex);
			const auto found = entries.find(key);
			if (found != entries.end()) return std::static_pointer_cast<const T>(found->second.value);
			while (!recent.empty() && retained > limit - bytes) {
				const auto it = entries.find(recent.front()); retained -= it->second.bytes;
				released.push_back(std::move(it->second.value)); entries.erase(it); recent.pop_front();
			}
			recent.push_back(key);
			entries.emplace(key, Entry{made, bytes, std::prev(recent.end())});
			retained += bytes;
		}
		return made;
	}

	b40::BodyRef prepare(const b40::Data &data, const Building &record) {
		auto value = std::make_unique<b40::Body>();
		Building seeded = record; seeded.body = 0; b40::record_mass(data, seeded, *value);
		return b40::BodyRef(value.release(), MassRelease{shared_from_this()});
	}
};

BodyCache::BodyCache(size_t retained_bytes) : impl_(std::make_shared<Impl>(retained_bytes)) {}
BodyCache::~BodyCache() = default;

std::shared_ptr<const b40::Body> BodyCache::get(const b40::Data &data, const Building &record, const b40::BodyInput &source) {
	if (source.mass) return source.mass;
	if (source.edited) {
		if (auto prepared = b40::BodyTable::mass_of(source.edited)) return prepared;
		// A transient source has its own exact lifetime and is never retained in the procedural mass cache.
		auto mass = std::make_unique<b40::Body>(*source.edited); b40::resolve_blocks(*mass);
		return b40::BodyRef(mass.release(), Impl::MassRelease{impl_});
	}
	const Impl::Key key{0, Impl::MASS, body_identity(record, source), 0, {}, {}};
	return impl_->memo<b40::Body>(key, [&] { return impl_->prepare(data, record); }, [](const b40::Body &body) {
		return b40::body_retained_bytes(body) + sizeof(Impl::MassRelease) + RetainedMemory::shared_control_bytes();
	});
}

std::unique_ptr<b40::PaneRooms> BodyCache::rooms(const b40::Data &data, const b40::BodyRef &source, b40::Purpose purpose) {
	auto owner = impl_;
	const auto workspace = [&](b40::FloorMemo floor = {}, b40::PaneMemo pane = {}) {
		auto result = std::make_unique<b40::PaneRooms>(data, *source, purpose, std::move(floor), std::move(pane));
		result->source_lease = source;
		return result;
	};
	if (owner->limit == 0) return workspace();
	if (auto version = b40::BodyTable::derived_owner(source, owner)) owner = std::static_pointer_cast<Impl>(version);
	else if (const auto *release = std::get_deleter<Impl::MassRelease>(source)) owner = release->owner.lock();
	else owner.reset(); // an unobserved external root cannot safely key retained derivatives by its address
	if (!owner) return workspace();
	const uintptr_t address = reinterpret_cast<uintptr_t>(source.get());
	b40::FloorMemo floor = [owner, address, purpose](const std::string &type, const std::function<b40::FloorRef()> &make) {
		return owner->memo<b40::FloorInterior>({address, Impl::FLOOR, {}, purpose, type, {}}, make, [](const b40::FloorInterior &value) {
			return sizeof(value) + RetainedMemory::shared_control_bytes() + value.furniture.capacity() * sizeof(b40::Furniture)
				+ value.partitions.capacity() * sizeof(b40::Partition) + value.spaces.capacity() * sizeof(b40::Space);
		});
	};
	b40::PaneMemo pane = [owner, address, purpose](const std::string &type, const b40::PaneRoomKey &key, const std::function<b40::PaneRoom()> &make) {
		return *owner->memo<b40::PaneRoom>({address, Impl::PANE, {}, purpose, type, key}, [&] { return std::make_shared<const b40::PaneRoom>(make()); }, [](const b40::PaneRoom &) {
			return sizeof(b40::PaneRoom) + RetainedMemory::shared_control_bytes();
		});
	};
	return workspace(std::move(floor), std::move(pane));
}
} // namespace city
