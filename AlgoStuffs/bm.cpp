#include "bm.h"

using namespace std;

vector<size_t> fastsearch(const string &text, const string &pattern)
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