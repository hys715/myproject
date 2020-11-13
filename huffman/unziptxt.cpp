// Programmer : hys
// Date : 2020-11-13
// Objective : Decompress file compressed with huffman tree

#include <iostream>
#include <fstream>
#include "function.h"
#include "hufftree.h"
using namespace std;

map<char, int> alphabet; // Record alpha and frequence
fileHead head; // Head of Huffman file
alphafreq af;

// Judge whether the file is Huffman file
// If yes, get alpha and frequence
bool getAlphaFreq(string &srcFile) {
    // Open file
    ifstream fin(srcFile, ios::binary);
    if (!fin.is_open()) {
        cout << "Open file failed! Filename : " << srcFile << endl;
        return false;
    }

    // Read the head of file and judge
    fin.read((char*)&head, sizeof(head));
    if (head.flag[0] != 'H' || head.flag[1] != 'U' || head.flag[2] != 'F' || head.flag[3] != 'F') {
        cout << "Not support this file format! Filename : " << srcFile << endl;
        return false;
    }

    // Read alpha and frequence one by one
    for (int i = 1; i <= (int)head.alphaVariety; ++i) {
        fin.read((char*)&af, sizeof(af));
        alphabet[af.alpha] = af.freq;
    }

    // Close file
    fin.close();
    return true;
}

// Start to decompress
bool unzip(string &srcFile, string &tarFile, node *root) {
    // Get the size of source file
    int fileSize;
    ifstream fin(srcFile, ios::binary);
    if (!fin.is_open()) {
        cout << "Open file failed! Filename : " << srcFile << endl;
        return false;
    }
    fin.seekg(0, ios_base::end); // Skip to the end of file
    fileSize = fin.tellg(); // Get the position of the end of file
    fin.close();

    // Open files
    fin.open(srcFile, ios::binary);
    if (!fin.is_open()) {
        cout << "Open file failed! Filename : " << srcFile << endl;
        return false;
    }
    ofstream fout(tarFile, ios::binary);
    if (!fout.is_open()) {
        cout << "Open file failed! Filename : " << tarFile << endl;
        return false;
    }

    fin.seekg(sizeof(fileHead) + sizeof(alphafreq) * alphabet.size()); // Skip the head of file
    int cur = fin.tellg(), index = 0; // Get the position of the current file stream
    char value; // One byte
    node *p = root;

    // Read a byte one by one
    fin.read(&value, sizeof(char));
    while (1) {
        // If p is leaf, p->ch is a valid alpha
        if (!p->left && !p->right) {
            fout.write(&(p->ch), sizeof(char));
            // If read over, then exit
            if (cur >= fileSize && index >= head.lastBitNum)
                break;
            p = root;
        }
        // Find the leaf of Huffman tree
        if (GET(value, index)) p = p->right;
        else p = p->left;
        if (++index >= 8) {
            index = 0;
            fin.read(&value, sizeof(char));
            cur = fin.tellg(); // Renew the position of the current file stream
        }
    }

    // Close files
    fin.close();
    fout.close();
    return true;
}

int main(int argc, char *argv[]) {
    // Judge whether the format is correct
    if (argc < 3 || argc > 3) {
        if (argc == 1) cout << "Source and target files are missing!\n";
        else if (argc == 2)  cout << "Target file is missing!\n";
        else cout << "~~~ERROR~~~\n";
        return 0;
    }

    string srcFile(argv[1]), tarFile(argv[2]);
    srcFile += ".huff", tarFile += ".txt"; // Add suffix

    if (!getAlphaFreq(srcFile)) {
        cout << "~~~ERROR~~~\n";
        return 0;
    }

    // Build huffman tree and get the root of Huffman tree
    Hufftree tree(alphabet);
    node top = tree.getHufftree();

    if (!unzip(srcFile, tarFile, &top)) cout << "~~~ERROR~~~\n";

    return 0;
}