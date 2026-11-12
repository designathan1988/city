// The engine's command line (log/trace-engine-plan.md): `dump <input> <output>` reads an input file and writes its
// canonical text; `stats <input>` reads it and prints what it holds; `analyse <input> <output> [--no-contexts]
// [--names]` runs the analysis (heap/analysis.py `analyse`) and writes its result (with `--names`, what each local
// and name read holds). Every stage prints a line, with the time since
// the start, as it ends, and the solver one every so many items, so the process that runs it watches it live.
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "analysis.h"
#include "dump.h"
#include "output.h"
#include "pickle_reader.h"
#include "semantics.h"
#include "terms.h"

namespace {

using Clock = std::chrono::steady_clock;
const Clock::time_point kStart = Clock::now();
constexpr int kUsage = 2;
constexpr double kMegabyte = 1e6;
constexpr uint64_t kGrowthEvery = 4;       // ticks between two reports of what grows
constexpr size_t kLargest = 25;            // the nodes a report names
constexpr uint64_t kCollapseEvery = uint64_t{1} << 22;   // items between two searches for cycles to collapse

void say(const std::string& line) {
    double seconds = std::chrono::duration<double>(Clock::now() - kStart).count();
    std::printf("%8.2fs %s\n", seconds, line.c_str());
    std::fflush(stdout);
}

std::vector<uint8_t> read_file(const char* path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error(std::string("cannot open ") + path);
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    say("read " + std::to_string(data.size() / kMegabyte) + " MB");
    return data;
}

ftengine::Value parse(const std::vector<uint8_t>& data, ftengine::Store& store) {
    ftengine::Value root = ftengine::read_pickle(data, store);
    say("parsed: " + std::to_string(store.terms()) + " terms, " + std::to_string(store.strings()) + " strings");
    return root;
}

int analyse(const char* input, const char* output, bool sensitive, bool names, uint64_t collapse_every) {
    ftengine::Input in;
    ftengine::Value root = parse(read_file(input), in.store);
    ftengine::decode(root, in);
    say("decoded: " + std::to_string(in.units.size()) + " units, " + std::to_string(in.classes.size()) + " classes");
    ftengine::Analysis an(in);
    an.contexts = sensitive;
    an.solver.collapse_every = collapse_every;
    ftengine::Semantics semantics(an);
    uint64_t ticks = 0;
    auto tick = [&](uint64_t items) {
        if (++ticks % kGrowthEvery == 0) {
            say("  " + an.growth());
            say("  " + semantics.profile());
            say("  costliest:" + an.costliest(kLargest));
            say("  stores:" + an.stores(kLargest));
            if (ticks % (kGrowthEvery * kGrowthEvery) == 0) say("  runs:" + an.runs(kLargest));
            if (ticks % (kGrowthEvery * kGrowthEvery) == 0) say("  cycles: " + an.solver.cycles());
        }
        say("  " + std::to_string(items) + " items, " + std::to_string(an.solver.nodes()) + " nodes, " +
            std::to_string(an.objects()) + " objects, " + std::to_string(an.links.size()) + " links, queue " +
            std::to_string(an.solver.pending()));
    };
    an.entries(true);
    an.solver.run(semantics, tick);
    int rounds = 0;
    while (an.entries(false)) {
        an.solver.run(semantics, tick);
        ++rounds;
    }
    say("solved: " + std::to_string(an.links.size()) + " links, " + std::to_string(an.objects()) + " objects, " +
        std::to_string(an.solver.nodes()) + " nodes, " + std::to_string(an.solver.items()) + " items, " +
        std::to_string(rounds) + " entry rounds, " + std::to_string(in.missing) + " resolutions missing");
    std::string bytes = ftengine::result_pickle(an, semantics, names);
    for (const std::string& line : in.missed) say("  missing: " + line);
    std::ofstream(output, std::ios::binary).write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
    say("written " + std::to_string(bytes.size()) + " bytes");
    std::fflush(stdout);
    std::_Exit(0);                              // the process's memory goes with it: no structure torn down piece by piece
}

}  // namespace

int main(int argc, char** argv) {
    std::string command = argc > 1 ? argv[1] : "";
    try {
        if (command == "dump" && argc == 4) {
            ftengine::Store store;
            ftengine::Value root = parse(read_file(argv[2]), store);
            std::string text = ftengine::dump(store, root);
            std::ofstream(argv[3], std::ios::binary).write(text.data(), static_cast<std::streamsize>(text.size()));
            say("dumped " + std::to_string(text.size()) + " bytes");
            return 0;
        }
        if (command == "analyse" && argc >= 4) {
            bool sensitive = true, names = false;
            uint64_t collapse_every = kCollapseEvery;
            for (int i = 4; i < argc; ++i) {
                std::string flag = argv[i];
                if (flag == "--no-contexts") sensitive = false;
                else if (flag == "--names") names = true;
                else if (flag == "--collapse-every" && i + 1 < argc) collapse_every = std::stoull(argv[++i]);
                else throw std::runtime_error("unknown flag " + flag);
            }
            return analyse(argv[2], argv[3], sensitive, names, collapse_every);
        }
        if (command == "stats" && argc == 3) {
            ftengine::Store store;
            parse(read_file(argv[2]), store);
            return 0;
        }
    } catch (const std::exception& error) {
        say(std::string("failed: ") + error.what());
        return 1;
    }
    std::fprintf(stderr, "usage: ftengine dump <input> <output> | stats <input> | analyse <input> <output> "
                         "[--no-contexts] [--names] [--collapse-every N]\n");
    return kUsage;
}
