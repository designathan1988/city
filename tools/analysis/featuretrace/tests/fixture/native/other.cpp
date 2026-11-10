// Fixture: a second file with its own internal helper of the same name.
namespace fx {
namespace {
int helper() { return 2; }
} // namespace
int use_other() { return helper(); }
} // namespace fx
