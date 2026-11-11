// Calibration case (LAB_COPY): the change touches only its own function; the copy below it, on lines the change leaves
// alone, is the scan's to find, not this gate's.
#include <algorithm>
#include <cmath>
#include <vector>

int shelves_left(int open, int taken) {
    return std::max(0, open - taken);
}

struct Loaf {
    double weightGrams;
    double proofMinutes;
};

double batchMinutes(const std::vector<Loaf>& loaves) {
    double heaviest = 0;
    for (const Loaf& loaf : loaves) {
        if (loaf.weightGrams > heaviest) heaviest = loaf.weightGrams;
    }
    const double baseMinutes = 18.5 + heaviest * 0.042;
    return baseMinutes * 1.35;
}
