#include "kmp.h"

using namespace std;

vector<size_t> getDFA(string pattern) {
    size_t length = pattern.length();
    vector<size_t> dfa(length);
    dfa[0] = 0;
    size_t longestPrefixIndex = 0;

    for (size_t i = 2; i < length; i++) {
        // back-track
        while (longestPrefixIndex > 0 && pattern[longestPrefixIndex + 1] != pattern[i]) {
            longestPrefixIndex = dfa[longestPrefixIndex];
        }

        // match
        if (pattern[longestPrefixIndex + 1] == pattern[i]) {
            longestPrefixIndex++;
        }
        dfa[i] = longestPrefixIndex;
    }
    return dfa;
}

vector<size_t> search(const string &text, const string &pattern, char wildcard) {
    size_t textLength = text.length();
    size_t patternLength = pattern.length();
    if (patternLength > textLength) {
        return vector<size_t>(-1);
    }

    // create dfa
    vector<size_t> prefixTable = getDFA(pattern);

    vector<size_t> ret;

    size_t matchLength = 0;
    char wildLetter = '\0';
    for (size_t i = 0; i < textLength; i++) {
        // back-track on failure
        while (matchLength > 0 && pattern[matchLength] != text[i]) {
            // check if fail was due to wildcard
            if (pattern[matchLength] == wildcard) {
                // if initial wildcard, set it
                if (wildLetter == '\0') {
                    wildLetter = text[i];

                    // loop-back with KMP - double check already matched pattern
                    size_t kmpValue = search(text.substr(i - matchLength, i), pattern.substr(0, matchLength), wildcard)[0];
                    if (kmpValue != 0) {
                        matchLength = 0; // reset match
                    }
                    else if (pattern[matchLength - 1] == wildcard) {
                        wildLetter = text[i - 1]; // reset wildcard
                    }
                    break;
                }
                else if (wildLetter == text[i]) {
                    break; // wildcard matches
                }
            }

            matchLength = prefixTable[matchLength - 1]; // fall-back
            wildLetter = '\0';

            // edge case - match previous seen for proper shift
            if (matchLength == 0 && pattern[matchLength + 1] == wildcard
                && text[i - 1] == pattern[matchLength]) {
                matchLength++;
            }
        }

        // match or wildcard
        if (pattern[matchLength] == text[i] || pattern[matchLength] == wildcard) {
            // wildcard
            if (pattern[matchLength] == wildcard) {
                if (wildLetter == '\0') {
                    wildLetter = text[i]; // set wildcard
                }
                else if (wildLetter != text[i]) {
                    // doesn't match current wildcard
                    if (matchLength == 1) {
                        wildLetter = text[i]; // edge case, new wildcard
                        continue;
                    }
                    // reset
                    wildLetter = '\0';
                    matchLength = 0;
                    continue;
                }
            }
            else {
                wildLetter = '\0'; // reset wildcard
            }
            matchLength++; // matched
        }

        // found the pattern
        if (matchLength == patternLength) {
            ret.push_back(i - (patternLength - 1));
        }
    }

    // couldn't find the pattern
    return ret;
}

int* pre_kmp(string pattern)
{
    int size = pattern.size();
    int* pie = new int[size];
    pie[0] = 0;
    int k = 0;
    for (int i = 1; i < size; i++)
    {
        while (k > 0 && pattern[k] != pattern[i])
        {
            k = pie[k - 1];
        }
        if (pattern[k] == pattern[i])
        {
            k = k + 1;
        }
        pie[i] = k;
    }

    return pie;
}

vector<size_t> search(const string &text, const string &pattern) 
{
    int* pie = pre_kmp(pattern);
    vector<size_t> ret;
    int matched_pos = 0;
    for (int current = 0; current < text.length(); current++)
    {
        while (matched_pos > 0 && pattern[matched_pos] != text[current])
            matched_pos = pie[matched_pos - 1];

        if (pattern[matched_pos] == text[current])
            matched_pos = matched_pos + 1;

        if (matched_pos == (pattern.length()))
        {
            ret.push_back(current - (pattern.length() - 1));
            matched_pos = pie[matched_pos - 1];
        }
    }
    return ret;
}

/*int main() {
    string text, pattern;
    cout << "Enter the text: ";
    cin >> text;
    cout << "Enter the pattern: ";
    cin >> pattern;

    int index = search(text, pattern, '?');
    if (index != -1) {
        cout << index << endl;
    }
    else {
        cout << "Couldn't find pattern in the text." << endl;
    }
    return 0;
}*/