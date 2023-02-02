#pragma once
#include "bm.h"

using namespace std;

vector<size_t> bmh(const string &text, const string &pattern)
{
	size_t m = pattern.size();
	size_t n = text.size();

	size_t i;
	size_t bad[256];

    for (size_t i = 0; i < 256; ++i)
        bad[i] = m;

    size_t lastPatternByte = m - 1;

    for (size_t i = 0; i < lastPatternByte; ++i)
        bad[pattern[i]] = lastPatternByte - i;

    // Beginning

    size_t index = 0;
    vector<size_t> ret;

    while (index <= (n - m))
    {
        for (size_t i = lastPatternByte; text[(index + i)] == pattern[i]; --i)
        {
            if (i == 0)
            {
                ret.push_back(index);
                break;
            }
        }

        index += bad[text[(index + lastPatternByte)]];
    }

    return ret;
}

std::vector<int> bmh_w_fast(const std::string& text, const std::string& pattern, char wildcard) {
    // Does not account for overlaps
    int m = pattern.length();
    int n = text.length();

    vector<int> right(16, -1); // 16 = hex alphabet
    for (int j = 0; j < m; j++) {
        if (pattern[j] != wildcard) {
            int a = (pattern[j] >= '0' && pattern[j] <= '9') ? (pattern[j] - '0') : (pattern[j] - 'A' + 10);
            right[a] = j;
        }
    }

    vector<int> positions;
    int skip;
    for (int i = 0; i <= n - m; i += skip) {
        skip = 0;
        for (int j = m - 1; j >= 0; j--) {
            if (pattern[j] != wildcard && pattern[j] != text[i + j]) {
                int a = (text[i + j] >= '0' && text[i + j] <= '9') ? (text[i + j] - '0') : (text[i + j] - 'A' + 10);
                skip = max(1, j - right[a]);
                break;
            }
        }
        if (skip == 0) {
            positions.push_back(i);
            skip = 1;
        }
    }
    return positions;
}

vector<int> bmh_w_safe(const string& text, const string& pattern, char wildcard) {
    // Does account for overlaps
    int m = pattern.length();
    int n = text.length();

    vector<int> right(16, -1); // 16 = hex alphabet
    for (int j = 0; j < m; j++) {
        if (pattern[j] != wildcard) {
            int a = (pattern[j] >= '0' && pattern[j] <= '9') ? (pattern[j] - '0') : (pattern[j] - 'A' + 10);
            right[a] = j;
        }
    }

    vector<int> positions;
    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = m - 1; j >= 0; j--) {
            if (pattern[j] != wildcard && pattern[j] != text[i + j]) {
                break;
            }
        }
        if (j == -1) {
            positions.push_back(i);
        }
    }
    return positions;
}