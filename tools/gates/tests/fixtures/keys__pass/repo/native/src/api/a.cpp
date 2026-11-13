// A: a hint the table holds, a command key built at run time, a key named only in a comment ("hint.road.nowhere").
#include <string>
std::string refuse() { return "hint.road.here"; }
std::string label(const std::string &name) { return "cmd." + name; }
std::string prose() { return "see hint.road.nowhere for more"; }
std::string reason(const std::string &why) { return "hint.road." + why; }
