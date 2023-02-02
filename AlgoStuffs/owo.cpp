#include "owo.h"

const int HEX_ALPHABET_SIZE = 16;

std::vector<int> match(const std::string& text, const std::string& pattern, char wildcard) {
    int m = pattern.length();
    int n = text.length();

    unsigned long long d = 256;  // number of characters in the alphabet
    unsigned long long q = 998244353;  // prime number
    unsigned long long h = 1;
    unsigned long long p = 0;
    unsigned long long t = 0;

    // Compute the hash value of the pattern and the first window of the text
    for (int i = 0; i < m; i++) {
        if (pattern[i] != wildcard) {
            h = (h * d) % q;
            int a = (pattern[i] >= '0' && pattern[i] <= '9') ? (pattern[i] - '0') : (pattern[i] - 'A' + 10);
            p = (p * d + a) % q;
            int b = (text[i] >= '0' && text[i] <= '9') ? (text[i] - '0') : (text[i] - 'A' + 10);
            t = (t * d + b) % q;
        }
    }

    vector<int> positions;
    for (int i = 0; i <= n - m; i++) {
        // Check if the hash values match
        if (p == t) {
            // Check if the actual substrings match
            bool match = true;
            for (int j = 0; j < m; j++) {
                if (pattern[j] != wildcard && pattern[j] != text[i + j]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                positions.push_back(i);
            }
        }
        if (i < n - m) {
            if (text[i + m] != wildcard) {
                int b = (text[i + m] >= '0' && text[i + m] <= '9') ? (text[i + m] - '0') : (text[i + m] - 'A' + 10);
                t = (d * (t - b * h) + b) % q;
            }
        }
    }

    return positions;
}