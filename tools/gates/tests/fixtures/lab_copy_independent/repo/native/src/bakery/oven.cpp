// Calibration case (LAB_COPY): the oven's behaviour designed for this game: the timing and the rack read from data,
// the batch chosen by partitioning the line, the rack as shelves freed per cooling step.
#include <algorithm>
#include <vector>

struct OvenTable {  // read from project/data (the bakery's table)
    double minutes_fixed;
    double minutes_per_gram;
    double crust;
    double proof_needed;
    double cool_step;
    int shelves;
};

struct Bread {
    double grams;
    double proofed;
};

double bake_time(const OvenTable& table, const std::vector<Bread>& breads) {
    auto lighter = [](const Bread& a, const Bread& b) { return a.grams < b.grams; };
    const auto top = std::max_element(breads.begin(), breads.end(), lighter);
    const double grams = top == breads.end() ? 0.0 : top->grams;
    return (table.minutes_fixed + table.minutes_per_gram * grams) * table.crust;
}

class Kiln {
public:
    explicit Kiln(const OvenTable& table) : table_(table), open_(table.shelves) {}

    std::vector<Bread> load(std::vector<Bread>& line, int room) {
        const auto ready = std::stable_partition(line.begin(), line.end(),
                                                 [this](const Bread& b) { return b.proofed >= table_.proof_needed; });
        const int take = std::min({room, open_, static_cast<int>(ready - line.begin())});
        std::vector<Bread> out(line.begin(), line.begin() + take);
        line.erase(line.begin(), line.begin() + take);
        open_ -= take;
        return out;
    }

    int cool(double elapsed) {
        open_ = std::min(table_.shelves, open_ + static_cast<int>(elapsed / table_.cool_step));
        return open_;
    }

private:
    OvenTable table_;
    int open_;
};
