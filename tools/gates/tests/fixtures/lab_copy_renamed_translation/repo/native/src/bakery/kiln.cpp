// Calibration case (LAB_COPY): the same line-by-line translation with every name changed: a rename is still a copy.
#include <algorithm>
#include <cmath>
#include <deque>
#include <vector>

constexpr int SHELF_TOTAL = 14;
constexpr double BROWNING = 1.35;

struct Bun {
    double mass;
    double rested;
};

double bake_span(const std::vector<Bun>& buns) {
    double biggest = 0;
    for (const Bun& bun : buns) {
        if (bun.mass > biggest) biggest = bun.mass;
    }
    const double raw = 18.5 + biggest * 0.042;
    return raw * BROWNING;
}

class Kiln {
    std::deque<Bun> pending;
    int open = SHELF_TOTAL;

public:
    void admit(const Bun& bun) {
        pending.push_back(bun);
    }

    std::vector<Bun> take_load(int room) {
        std::vector<Bun> load;
        while (pending.size() > 0 && load.size() < room && open > 0) {
            Bun bun = pending.front();
            pending.pop_front();
            if (bun.rested < 45) {
                pending.push_back(bun);
                break;
            }
            load.push_back(bun);
            open -= 1;
        }
        return load;
    }

    int reclaim(double elapsed) {
        const int gained = std::floor(elapsed / 7.5);
        open = std::min(SHELF_TOTAL, open + gained);
        return open;
    }
};
