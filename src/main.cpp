#include "reception/Reception.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

static void printUsage(const char *progName)
{
    std::cerr << "Usage: " << progName << " <timeMultiplier> <numCooks> <restockMs>\n"
              << "  timeMultiplier : float > 0 (cooking time multiplier)\n"
              << "  numCooks       : int >= 1  (cooks per kitchen)\n"
              << "  restockMs      : int >= 0  (stock replenishment interval in ms)\n";
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printUsage(argv[0]);
        return 84;
    }

    double      multiplier  = 0.0;
    std::size_t numCooks    = 0;
    long        restockMs   = 0;

    try {
        multiplier = std::stod(argv[1]);
        numCooks   = static_cast<std::size_t>(std::stoul(argv[2]));
        restockMs  = std::stol(argv[3]);
    } catch (const std::exception &) {
        printUsage(argv[0]);
        return 84;
    }

    if (multiplier <= 0.0 || numCooks < 1 || restockMs < 0) {
        printUsage(argv[0]);
        return 84;
    }

    try {
        Reception reception(multiplier, numCooks,
                            std::chrono::milliseconds(restockMs));
        reception.run();
    } catch (const std::exception &e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 84;
    }
    return 0;
}
