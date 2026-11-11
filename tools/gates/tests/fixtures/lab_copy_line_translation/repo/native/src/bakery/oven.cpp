// Calibration case (LAB_COPY): the reference's oven translated line by line into C++, its names kept.
#include <algorithm>
#include <cmath>
#include <deque>
#include <vector>

constexpr int RACK_SLOTS = 14;
constexpr double CRUST_FACTOR = 1.35;

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
    return baseMinutes * CRUST_FACTOR;
}

class OvenQueue {
    std::deque<Loaf> waiting;
    int rackFree = RACK_SLOTS;

public:
    void enqueueLoaf(const Loaf& loaf) {
        waiting.push_back(loaf);
    }

    std::vector<Loaf> nextBatch(int capacity) {
        std::vector<Loaf> batch;
        while (waiting.size() > 0 && batch.size() < capacity && rackFree > 0) {
            Loaf loaf = waiting.front();
            waiting.pop_front();
            if (loaf.proofMinutes < 45) {
                waiting.push_back(loaf);
                break;
            }
            batch.push_back(loaf);
            rackFree -= 1;
        }
        return batch;
    }

    int coolingSlots(double minutesSinceBake) {
        const int freedSlots = std::floor(minutesSinceBake / 7.5);
        rackFree = std::min(RACK_SLOTS, rackFree + freedSlots);
        return rackFree;
    }
};
