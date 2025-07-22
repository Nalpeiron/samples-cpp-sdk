#pragma once
#include <string>

struct ConfirmOptions
{
    std::string Message = "Are you sure?";
    std::string TrueOption = "Yes";
    std::string FalseOption = "No";
};
