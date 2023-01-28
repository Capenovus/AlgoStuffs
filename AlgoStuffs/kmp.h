#pragma once
#include <vector>
#include "util.h"

std::vector<size_t> search(const std::string& text, const std::string& pattern, char wildcard);
std::vector<size_t> search(const std::string& text, const std::string& pattern);