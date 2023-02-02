#pragma once
#include <vector>
#include <map>
#include <deque>
#include <queue>
#include <unordered_set>
#include "util.h"


// FAULTY AND SLOW DNU
namespace acs {
	std::vector<std::pair<int, int>> search(const std::string& pattern, const std::string& text);
	std::vector<std::pair<int, int>> search(const std::vector<std::string> &patterns, const std::string& text);
}