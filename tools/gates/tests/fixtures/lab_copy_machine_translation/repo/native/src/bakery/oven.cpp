// Calibration case (LAB_COPY): the reference's oven rewritten statement by statement with types added, as a machine
// translation writes it (its own array and math runtime, every number a double).
#include "core/js_array.h"
#include "core/js_number.h"

const double RACK_SLOTS = 14;
const double CRUST_FACTOR = 1.35;

struct Loaf {
    double weightGrams = 0;
    double proofMinutes = 0;
};

double batchMinutes(js::Array<Loaf> loaves) {
    double heaviest = 0;
    for (Loaf loaf : loaves) {
        if (loaf.weightGrams > heaviest) heaviest = loaf.weightGrams;
    }
    const double baseMinutes = 18.5 + heaviest * 0.042;
    return baseMinutes * CRUST_FACTOR;
}

class OvenQueue {
public:
    js::Array<Loaf> waiting = js::Array<Loaf>();
    double rackFree = RACK_SLOTS;

    void enqueueLoaf(Loaf loaf) {
        this->waiting.push(loaf);
    }

    js::Array<Loaf> nextBatch(double capacity) {
        const js::Array<Loaf> batch = js::Array<Loaf>();
        while (this->waiting.length() > 0 && batch.length() < capacity && this->rackFree > 0) {
            const Loaf loaf = this->waiting.shift();
            if (loaf.proofMinutes < 45) {
                this->waiting.push(loaf);
                break;
            }
            batch.push(loaf);
            this->rackFree -= 1;
        }
        return batch;
    }

    double coolingSlots(double minutesSinceBake) {
        const double freedSlots = js::Math::floor(minutesSinceBake / 7.5);
        this->rackFree = js::Math::min(RACK_SLOTS, this->rackFree + freedSlots);
        return this->rackFree;
    }
};
