// Programmer : hys
// Date : 2020-11-12
// Objective : Implementation of Huffman tree

#ifndef HUFFTREE_H
#define HUFFTREE_H

#include <map>
#include <queue>
#include <string>
using std::string;
using std::map;
using std::priority_queue;

// Node of Huffman tree
struct node {
    char ch;
    int freq;
    node *left, *right;
    node(char c = 0, int f = 0, node *l = nullptr, node *r = nullptr)
    : ch(c), freq(f), left(l), right(r) {}
    bool operator< (const node &other) const { return freq > other.freq; }
};

// Implementation of Huffman tree
class Hufftree {
public:
    // Build Huffman tree
    Hufftree(map<char, int> &alphabet) {
        // Line up letters and their frequencies
        for (auto alpha : alphabet)
            q.push(node(alpha.first, alpha.second));
        while (q.size() > 1) {
            node *left = new node(q.top());
            q.pop();
            node *right = new node(q.top());
            q.pop();
            q.push(node(0, left->freq + right->freq, left, right));
        }
    }

    // Destroy Huffman tree
    ~Hufftree() {
        destroy(q.top().left);
        destroy(q.top().right);
    }

    // Coding letters
    void huffcode(map<char, string> &CodeMap) {
        node t = q.top();
        encode(&t, "", CodeMap);
    }

    // Get Huffman tree
    node getHufftree() { return q.top(); }

private:
    priority_queue<node> q;

    void destroy(node *root) {
        if (!root) return;
        destroy(root->left);
        destroy(root->right);
        delete root;
    }

    void encode(node *root, string code, map<char, string> &CodeMap) {
        if (!root->left && !root->right) {
            CodeMap[root->ch] = code;
            return;
        }
        if (root->left) encode(root->left, code + "0", CodeMap);
        if (root->right) encode(root->right, code + "1", CodeMap);
    }
};

#endif
