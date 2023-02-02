#include "aho.h"
#include <map>
#include <vector>
#include <queue>
using namespace std;

const int ALPHABET_SIZE = 256;  // number of characters in the alphabet
const int N = 100005;  // maximum length of patterns and text
const int M = 1000;  // maximum number of patterns

int m;  // number of patterns
int trie_cnt;  // number of nodes in the trie
int fail[N];  // failure links for each node
int end_word[N];  // indicates if a node represents the end of a word
vector<int> trie[N][ALPHABET_SIZE];  // trie data structure
vector<int> matches[M];  // indices of all matches in the text
char pattern[M][N];  // patterns to match
char text[N];  // text to match

// Builds the trie data structure and precomputes the failure links
void build_trie() {
    queue<int> q;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (trie[0][i].size() > 0) {
            q.push(trie[0][i][0]);
        }
    }
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (trie[u][i].size() > 0) {
                int v = trie[u][i][0];
                q.push(v);
                fail[v] = trie[fail[u]][i][0];
                end_word[v] |= end_word[fail[v]];
            }
            else {
                trie[u][i].push_back(trie[fail[u]][i][0]);
            }
        }
    }
}

// Matches all patterns in the text
void match_patterns() {
    int n = strlen(text);
    int u = 0;
    for (int i = 0; i < n; i++) {
        u = trie[u][text[i]][0];
        for (int j = u; j > 0; j = fail[j]) {
            if (end_word[j]) {
                matches[j].push_back(i);
            }
        }
    }
}