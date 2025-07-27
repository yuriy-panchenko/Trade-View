// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include <YxSoft/TradeSummary.mqh>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <future> // Add this include at the top of the file to use std::async
#include <map>
#include <algorithm>
#include <set>
#include "framework.h"

#define KEY_SETTINGS			_T("Settings")

namespace fs = std::filesystem;
using SetFile = std::unordered_map<std::wstring, std::wstring>;

#endif //PCH_H
