#pragma once
#include "imports.h"

vector<size_t> bmh(const string& text, const string& pattern);
vector<int> bmh_w_fast(const string& text, const string& pattern, char wildcard = '?');
vector<int> bmh_w_safe(const string& text, const string& pattern, char wildcard = '?');