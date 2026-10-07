#include "Parser.hpp"
#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>
#include <stdexcept>

static const std::map<std::string, PizzaType> TYPE_MAP = {
    {"regina",    PizzaType::Regina},
    {"margarita", PizzaType::Margarita},
    {"americana", PizzaType::Americana},
    {"fantasia",  PizzaType::Fantasia}
};

static const std::map<std::string, PizzaSize> SIZE_MAP = {
    {"s",   PizzaSize::S},
    {"m",   PizzaSize::M},
    {"l",   PizzaSize::L},
    {"xl",  PizzaSize::XL},
    {"xxl", PizzaSize::XXL}
};

PizzaType Parser::parseType(const std::string &token)
{
    std::string lower = token;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    auto it = TYPE_MAP.find(lower);
    if (it == TYPE_MAP.end())
        throw std::runtime_error("Unknown pizza type: " + token);
    return it->second;
}

PizzaSize Parser::parseSize(const std::string &token)
{
    std::string lower = token;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    auto it = SIZE_MAP.find(lower);
    if (it == SIZE_MAP.end())
        throw std::runtime_error("Unknown pizza size: " + token);
    return it->second;
}

int Parser::parseCount(const std::string &token)
{
    if (token.empty() || token[0] != 'x')
        throw std::runtime_error("Invalid count format (expected x<N>): " + token);
    std::string digits = token.substr(1);
    if (digits.empty() || !std::all_of(digits.begin(), digits.end(), ::isdigit))
        throw std::runtime_error("Invalid count digits: " + token);
    int count = std::stoi(digits);
    if (count < 1)
        throw std::runtime_error("Count must be >= 1");
    return count;
}

Parser::Order Parser::parse(const std::string &line)
{
    Order result;
    std::stringstream ss(line);
    std::string       segment;

    while (std::getline(ss, segment, ';')) {
        std::stringstream seg(segment);
        std::string       typeStr, sizeStr, countStr;

        if (!(seg >> typeStr >> sizeStr >> countStr))
            throw std::runtime_error("Malformed order segment: " + segment);

        std::string extra;
        if (seg >> extra)
            throw std::runtime_error("Unexpected token in segment: " + extra);

        PizzaType type  = parseType(typeStr);
        PizzaSize size  = parseSize(sizeStr);
        int       count = parseCount(countStr);

        for (int i = 0; i < count; ++i)
            result.emplace_back(type, size);
    }
    return result;
}
