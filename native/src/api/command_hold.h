// CommandHold: the map held exclusively for one command of the player (map_hold::WriterHold with Writer::COMMAND), and in the
// tests' DLL the record checks of what it changed (sim/checks/records.h after_command), still under the hold, when it ends: the
// city-data checks on the ids the ChangeLog holds undrained, edit.refusal_inert after a refused command, the traffic-owned checks
// once the traffic has drained. The export's DLL keeps only the hold.
#pragma once

#include "core/checks.h"
#include "sim/checks/records.h"
#include "sim/state/city_state.h"
#include "sim/state/map_hold.h"

namespace city {

class CommandHold {
public:
	// `cause`: what the command is (a strings.csv key, or the history's step), named by a broken record found after it. `who`: the
	// writer the hold is counted as (map_hold.h), a command's unless named (the load names Writer::LOAD).
	CommandHold(CityState &s, const char *cause, map_hold::Writer who = map_hold::Writer::COMMAND)
		: s_(s), lock_(s.map_mutex, who), cause_(cause) {}
	~CommandHold() { // the body runs before the members end: the checks run under the hold
		if constexpr (CHECKS) {
			if (s_.ready) checks::after_command(s_, cause_, refused_);
		}
	}
	CommandHold(const CommandHold &) = delete;
	CommandHold &operator=(const CommandHold &) = delete;
	void refused() { refused_ = true; } // the command was refused: its rollback must have left the city as it was

private:
	CityState &s_;
	map_hold::WriterHold lock_;
	const char *cause_;
	bool refused_ = false;
};

} // namespace city
