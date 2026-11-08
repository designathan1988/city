// Physics: 9.81 in a comment.
#define SCALE 100
constexpr float G = 9.81f; // the standard gravity, m/s2 (CGPM 1901)
// Three cells of a test table.
static const int TABLE[3] = { 5, 6, 7 };
enum class K { A = 10, B = 20 };
const char *s = "42";
float h(float v) { return v * 0.5f + 1.f - 2; }
// A block of constants under one comment.
constexpr float WIDTH_M = 3.5f;
constexpr float HEIGHT_M = 2.75f;
constexpr float HALF = 0.5f;
