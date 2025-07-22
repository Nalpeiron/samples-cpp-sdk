#pragma once
#include "ConfirmOptions.hpp"
#include <iostream>
#include <functional>

inline bool Confirm(const ConfirmOptions& options)
{
    std::string input;

    while (true)
    {
        std::cout << options.Message << " [" << options.TrueOption << "/" << options.FalseOption << "]: ";
        std::getline(std::cin, input);

        if (input.empty()) continue;

        std::string lowerInput;
        for (char c : input) lowerInput += std::tolower(c);

        if (lowerInput == "y" || lowerInput == "yes") return true;
        if (lowerInput == "n" || lowerInput == "no") return false;

        std::cout << "Please enter '" << options.TrueOption << "' or '" << options.FalseOption << "'.\n";
    }
}

inline bool Confirm(const std::function<void(ConfirmOptions&)>& configure)
{
    ConfirmOptions opts;
    configure(opts);
    return Confirm(opts);
}
