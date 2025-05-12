#pragma once

#include <filesystem>

#if !defined(APP_ROOT_DIR)
    #error "APP_ROOT_DIR is not configured"
#endif

namespace puzzleInputs
{
inline std::filesystem::path getInputDirectory()
{
    return std::filesystem::path(APP_ROOT_DIR) / "inputs";
}
}
