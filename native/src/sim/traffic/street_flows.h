// The far traffic of the whole map as flows (street_flows.cpp): each directed segment of the lane graph a link of the link
// transmission model (traffic.json _flow_doc: the sources), vehicles counted in at its start and out at its end, passed on at the
// nodes; nobody is simulated one by one outside the window. The window's streets hand their vehicles to the detailed simulation
// and take them back (flow_window.cpp). A step's work is spread over the ticks of its second by a budget, so a tick costs the
// same on any map. Pure C++ (no Godot types); the state lives in TrafficState::flows.
#pragma once

#include "sim/state/flow_mark.h"

#include <cstdint>
#include <vector>

namespace city {

struct CityState;

constexpr int FLOW_TURNS = 8; // the ways out of an approach kept: the arms a node may have (NODE_ARMS_MAX)
constexpr int FLOW_RING_MOST = 512; // steps of a street's past kept at most (a longer street's queue moves back sooner than it should)

// What a step's work is doing (StreetFlows::phase), in order: each street's rates, what each can send and take, the nodes, the
// starts from the streets' sides, the curves moved on; then idle until the next step. Its one setter: flow_step.cpp
// enter_flow_phase (FLOW_PHASES).
enum class FlowPhase : uint8_t { IDLE = 0, RATES, OFFERS, NODES, STARTS, CURVES };

struct StreetFlows {
	// By directed segment (the lane graph's ds); every table sized with the graph.
	std::vector<uint8_t> live; // it carries traffic (the ds is ok and has lanes)
	std::vector<uint8_t> inside; // the window holds it: its vehicles are the detailed simulation's
	std::vector<float> cap; // vehicles a step it lets through
	std::vector<float> store; // vehicles it holds standing bumper to bumper
	std::vector<float> start_rate; // vehicles a second that want to start a trip on it
	std::vector<float> end_share; // of what reaches its end, the share that ends its trip there
	std::vector<float> origin; // waiting to start on it
	std::vector<float> send, room, came, left; // the step's: what it can send and take, what came onto it and what left it
	std::vector<float> asked; // a node's round: what its approaches ask of this exit (zero between rounds)
	std::vector<float> owed; // inside: vehicles the window has still to make of it
	std::vector<float> along; // of `owed`, those to make anywhere along it (handed with the street): the rest come in at its mouth
	std::vector<uint16_t> detailed; // inside: the detailed vehicles on it at the step's start
	// The cumulative counts in at its start and out at its end, back in time a step a cell: two rings in `curve` (-1: none).
	std::vector<int> up_at, down_at;
	std::vector<uint16_t> up_n, down_n;
	std::vector<float> curve;
	std::vector<std::vector<int>> spare; // the rings given back, by their length
	// Its turns: FLOW_TURNS places a ds, the ds each leads onto (-1: none) and the share of its traffic taking it.
	std::vector<int> turn_to;
	std::vector<float> turn_share;
	// What the render was last told of it (marks are sent only on a change of these).
	std::vector<float> told_count, told_queue, told_speed;
	std::vector<uint8_t> told_flags;
	std::vector<uint8_t> line_due; // its line changed since the render had it

	uint64_t steps = 0; // steps done: the rings' clock
	double clock = 0; // seconds of the city since the last step began
	double day_shift = 0; // the city clock's shift the streets were last seeded at: a jump of the hour seeds them again
	double published_s = 0; // the traffic's clock at the last publication of the poses: the far cars' clock runs at its pace
	bool due = false; // a step is owed (one is still under way)
	bool relook = false; // streets were laid, changed or removed: the window looks at its streets again (window.cpp)
	FlowPhase phase = FlowPhase::IDLE;
	int cursor = 0; // where the phase under way stands (a ds or a node)
	std::vector<int> owing; // inside ds with a vehicle or more to make, each once (`in_owing`)
	std::vector<uint8_t> in_owing;
	std::vector<int> counted; // the ds given a count of detailed vehicles at the last step
	std::vector<FlowMark> marks; // the step's, moved to the ChangeLog at its end
	// Since the city opened: steps the clock owed while one was under way (the far traffic ran slower than the clock), vehicles
	// that started a trip, were not made at a full origin, ended their trip, were handed to the window and taken back from it.
	int late = 0;
	double started = 0, unmade = 0, ended = 0, handed_out = 0, handed_in = 0;
};

namespace traffic {

// street_flows.cpp
void reset_flows(CityState &s); // a new map or a load: every street fitted, seeded with its demand's traffic, every node's turns
void seed_flows(CityState &s); // every street carrying what the demand puts on it at this hour (the opening, a jump of the clock)
void fit_flow(CityState &s, int ds); // one ds's numbers read again off the lane graph (an edit): its vehicles kept, laid again
void fit_turns(CityState &s, int node); // the turns of the approaches into `node`, from the connectors out of their lanes
// After an edit of the roads: the flows of the segments removed and changed, and of every approach into a node that changed.
void refit_flows(CityState &s, const std::vector<int> &removed, const std::vector<int> &changed, const std::vector<int> &nodes);
// flow_step.cpp
void step_flows(CityState &s, float dt); // the clock, and a budget of the step's work
float flow_count(const StreetFlows &f, int ds); // vehicles on a ds outside the window
float flow_queued(const CityState &s, int ds); // of them, those that had the time to reach its end and did not leave
float flow_speed(const CityState &s, int ds); // its mean speed: its length over the free run plus the wait in its queue
double flow_vehicles(const StreetFlows &f); // every vehicle the flows hold: on the streets, at the origins, owed to the window
float flow_owed(const StreetFlows &f); // the vehicles the window is still to make: its fleet's already, though none drives yet
void lay_flow(CityState &s, int ds, float n); // `n` vehicles laid along a ds as in free flow: entered evenly over its free run
void flow_take(StreetFlows &f, int ds); // a detailed vehicle drove onto a ds outside the window: counted in at its start
void flow_owes(StreetFlows &f, int ds); // an inside ds with a vehicle or more owed is on the list of those to make

// flow_rates.cpp: the start rate and the end share of the ds in [from, to), from the zones beside them at this hour.
void flow_rates(CityState &s, int from, int to);
float flow_density(const CityState &s, int ds); // the vehicles a lane metre the demand puts on it at this hour and these levels

// demand.cpp: how much of the hour's trips a segment's sides start (`origin`) or end, by the zones beside it (0 to 1); and one
// detailed vehicle made on a ds of the window with a trip of its own, anywhere `along` it or at its mouth (-1: no room, no trip).
float side_weight(const CityState &s, int seg, bool origin);
int make_vehicle(CityState &s, int ds, bool along);

// flow_window.cpp: the window's side of the flows.
void flow_enters(CityState &s, int ds); // the ds comes into the window: its vehicles are owed to it, made along it
void flow_leaves(CityState &s, int ds); // it leaves: the detailed vehicles on it are taken off and counted on it again
void flow_begin(CityState &s); // a step begins: who drove out of the window is taken back, the detailed vehicles counted
void flow_admit(CityState &s); // the vehicles owed to the window made, a few a tick

} // namespace traffic
} // namespace city
