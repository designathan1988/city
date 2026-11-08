// The lot cutter of one cell (internal to sim/citygen): its occupancy grid, the streets' setbacks and the lots
// placed so far. Methods in lot_gen.cpp (grid, streets, tests) and lot_place.cpp (blocks and street strips).
#pragma once

#include "sim/citygen/lot_gen.h"

#include <cstdint>
#include <vector>

namespace city {

class LotStrips {
public:
	LotStrips(const GenShapes &s, const LotGraph &g, const std::vector<Profile> &profiles, const GenZones &zones, const LotGround &ground, double x0,
		double z0);
	std::vector<Lot> run();

	// A lot's frame: centre, along-front unit (t), away-from-street unit (l), width and depth.
	struct Frame {
		double cx, cz, tx, tz, lx, lz, w, d;
	};

private:
	const GenShapes &S; // growth.json shapes: the strips' grid and tests (K) and the lots' sizes (P)
	const StripsShape &K;
	const LotsShape &P;
	const LotGraph &g_;
	const std::vector<Profile> &prof_;
	const GenZones &zones_; // the catalogue's: works stand on its industrial zone
	const LotGround &ground_;
	double cx0_, cz0_, ax_, az_;
	int W_;
	std::vector<uint8_t> grid_;
	std::vector<std::vector<double>> tables_;
	std::vector<double> half_;
	std::vector<Lot> out_;

	double len(int s) const { return tables_[s].back(); }
	void mark(double x, double z, double r, uint8_t v);
	void mark_roads();
	V2 way_out(int s, int n) const;
	double stops_short(int s, int n, const std::vector<int> &arms) const;
	void facing(double fx, double fz, double ox, double oz, int &zone, int &profile) const;
	bool unfit(const Frame &f) const;
	void mark_lot(const Frame &f);
	bool lot_free(double x, double z) const; // no paving and no lot there
	uint8_t site_of(const Frame &f) const; // the lot's b40::LotSite bits
	void keep(const Frame &f, int zone, int profile);
	void place_blocks();
	void place_strips();
	struct Along {
		double px, pz, tx, tz, lx, lz;
	};
	Along along(int s, double at, double sign) const;
	void place_side(int s, int side, int zone);
	struct Strip {
		int s;
		double sign, face, depth, frontage;
		int zone;
	};
	void cut_run(const Strip &strip, double from, double to);
};

} // namespace city
