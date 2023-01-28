#pragma once
#include <vector>
#include <map>
#include <deque>
#include <queue>
#include <unordered_set>
#include "util.h"


// FAULTY AND SLOW DNU
std::vector<size_t> AcsFindFuzzyMatches(const std::string& pattern_with_wildcards, const std::string& text, char wildcard);
std::vector<size_t> AcsFindFuzzyMatchesFromFile(const std::string& pattern_with_wildcards, const std::string& file, char wildcard);
std::vector<size_t> AcsFindFuzzyHexMatches(std::string& pattern_with_wildcards, const std::string& text, char wildcard);
std::vector<size_t> AcsFindFuzzyHexMatchesFromFile(std::string& pattern_with_wildcards, const std::string& file, char wildcard);