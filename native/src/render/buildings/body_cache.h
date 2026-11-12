// Shared immutable masses, floor layouts and pane rooms under one byte LRU with exact source identity.
// Active source/floor leases and mutable cuts stay job-local; final source retirement removes its retained derivatives.
#pragma once

#include "sim/buildings/b40/record.h"

#include <array>
#include <memory>

namespace city {
namespace b40 { struct PaneRooms; }
using BodyIdentity = std::array<uint64_t, 12>;
BodyIdentity body_identity(const Building &record, const b40::BodyInput &source);

class BodyCache {
public:
	explicit BodyCache(size_t retained_bytes);
	~BodyCache();
	std::shared_ptr<const b40::Body> get(const b40::Data &data, const Building &record, const b40::BodyInput &source);
	std::unique_ptr<b40::PaneRooms> rooms(const b40::Data &data, const b40::BodyRef &source, b40::Purpose purpose);
private:
	struct Impl;
	std::shared_ptr<Impl> impl_;
};
} // namespace city
