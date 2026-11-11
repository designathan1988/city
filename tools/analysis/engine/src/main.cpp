// The engine's command line (log/trace-engine-plan.md): `dump <input> <output>` reads an input file and writes its
// canonical text; `stats <input>` reads it and prints what it holds. Every stage prints a line, with the time since the
// start, as it ends, so the process that runs it watches it live.
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include "dump.h"
#include "pickle_reader.h"
#include "terms.h"

namespace {

using Clock = std::chrono::steady_clock;
const Clock::time_point kStart = Clock::now();
constexpr int kUsage = 2;
constexpr double kMegabyte = 1e6;

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
        if (command == "stats" && argc == 3) {
            ftengine::Store store;
            parse(read_file(argv[2]), store);
            return 0;
        }
    } catch (const std::exception& error) {
        say(std::string("failed: ") + error.what());
        return 1;
    }
    std::fprintf(stderr, "usage: ftengine dump <input> <output> | stats <input>\n");
    return kUsage;
}
