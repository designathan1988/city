// Native logging to stderr.
#include <cstdio>
#include "log.h"

void log_line(const char* message) {
    std::fputs(message, stderr);
}
