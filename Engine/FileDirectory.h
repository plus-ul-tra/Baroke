#pragma once
#include <filesystem>

using namespace std;

filesystem::path GetExecutableDir();
filesystem::path FindSolutionRoot(const std::wstring& marker = L".sln");