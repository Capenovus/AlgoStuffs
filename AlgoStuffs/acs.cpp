#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <unordered_map>

const int ALPHABET_SIZE = 256;
const char WILDCARD = '?';

struct TrieNode {
    std::map<char, int> children;
    int end_word;
    int fail;

    TrieNode() {
        end_word = -1;
        fail = 0;
    }
};

void add_word(std::vector<TrieNode>& trie, const std::string& word) {
    int node = 0;
    for (char c : word) {
        char ch = c;
        if (ch == WILDCARD) {
            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                if (!trie[node].children[i]) {
                    int next = trie.size();
                    trie.push_back(TrieNode());
                    trie[node].children[i] = next;
                }
                node = trie[node].children[i];
            }
            break;
        }

        if (!trie[node].children[ch]) {
            int next = trie.size();
            trie.push_back(TrieNode());
            trie[node].children[ch] = next;
        }
        node = trie[node].children[ch];
    }
    trie[node].end_word = true;
}

void build_fail_pointers(std::vector<TrieNode>& trie) {
    std::queue<int> q;
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (trie[0].children[i]) {
            q.push(trie[0].children[i]);
        }
    }

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            if (trie[node].children[i]) {
                int next = trie[node].children[i];
                q.push(next);
                int fail = trie[node].fail;
                while (fail && !trie[fail].children[i]) {
                    fail = trie[fail].fail;
                }
                trie[next].fail = trie[fail].children[i] ? trie[fail].children[i] : 0;
            }
        }
    }
}

void isearch(const std::vector<TrieNode>& trie, const std::string& text, const std::vector<std::string>& words, std::vector<std::pair<int, int>>& result) {
    std::unordered_map<char, int> hex_map{ {'0', 0}, {'1', 1}, {'2', 2}, {'3', 3},
                                             {'4', 4}, {'5', 5}, {'6', 6}, {'7', 7},
                                             {'8', 8}, {'9', 9}, {'A', 10}, {'B', 11},
                                             {'C', 12}, {'D', 13}, {'E', 14}, {'F', 15} };

    int node = 0;
    for (int i = 0; i < text.size(); i++) {
        int idx = hex_map[text[i]];
        while (node && !trie[node].children.count(idx)) {
            node = trie[node].fail;
        }
        node = trie[node].children.count(idx) ? trie[node].children.at(idx) : 0;
        for (int cur = node; cur != 0; cur = trie[cur].fail) {
            if (trie[cur].end_word >= 0) {
                result.push_back({ i - words[trie[cur].end_word].size() + 1, trie[cur].end_word });
            }
        }
    }
}


namespace acs {
    std::vector<std::pair<int, int>> search(const std::string& pattern, const std::string& text)
    {
        std::vector<std::pair<int, int>> result;
        std::vector<std::string> words;
        words.push_back(pattern);
        std::vector<TrieNode> trie;
        add_word(trie, pattern);
        build_fail_pointers(trie);
        isearch(trie, text, words, result);
        return result;
    }
    std::vector<std::pair<int, int>> search(const std::vector<std::string> &patterns, const std::string& text)
    {
        std::vector<std::pair<int, int>> result;
        std::vector<TrieNode> trie;
        for (const std::string pattern : patterns)
            add_word(trie, pattern);
        build_fail_pointers(trie);
        isearch(trie, text, patterns, result);
    }
}