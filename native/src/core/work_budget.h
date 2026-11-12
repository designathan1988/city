// One strict shared credit counter; owners decide allocation, participants borrow it rather than renew allowances.
#pragma once
#include <limits>
namespace city {
struct WorkBudget {
	int left = 0;
	explicit WorkBudget(int n) : left(n) {}
	static WorkBudget from_count(double n) {
		if (!(n > 0)) return WorkBudget(0);
		if (n >= double(std::numeric_limits<int>::max())) return unlimited();
		return WorkBudget(int(n));
	}
	static WorkBudget unlimited() { return WorkBudget(std::numeric_limits<int>::max()); }
	bool take() { if (left <= 0) return false; --left; return true; }
};
} // namespace city
