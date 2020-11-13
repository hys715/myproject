// Programmer : hys
// Date : 2020-11-13
// Objective : Compress text file

#include <iostream>
#include <fstream>
#include "function.h"
#include "hufftree.h"
using namespace std;

map<char, int> alphabet; // Record alpha and frequence
map<char, string> CodeMap; // Map alpha to Huffman code

// Get frequence of alpha
bool getAlphaFreq(string &srcFile) {
    char c;
    // Open text file
    ifstream fin(srcFile, ios::binary);
    if (!fin.is_open()) {
        cout << "Open file failed! Filename : " << srcFile << endl;
        return false;
    }
    // Read an alpha one by one
    fin.read(&c, sizeof(char));
    while (!fin.eof()) {
        ++alphabet[c];
        fin.read(&c, sizeof(char));
    }
    // Close text file
    fin.close();
    return true;
}

// Get the number of significant bits of the last byte
int getLastBits() {
    int sum = 0;
    for (auto &code : CodeMap) {
        sum += code.second.size() * alphabet.at(code.first);
        sum &= 0xff; // sum %= 2^8
    }
    sum &= 0x7; // sum %= 2^3
    return sum == 0 ? 8 : sum;
}

// Start to compress
bool zip(string &srcFile, string &tarFile) {
    char c, value;
    int index = 0;

    // Open text file and output file
    ifstream fin(srcFile, ios::binary);
    if (!fin.is_open()) {
        cout << "Open file failed! Filename : " << srcFile << endl;
        return false;
    }
    ofstream fout(tarFile, ios::binary);
    if (!fout.is_open()) {
        cout << "Open file failed! Filename : " << tarFile << endl;
        return false;
    }

    // Initialize the head of output file
    fileHead head = {'H', 'U', 'F', 'F'};
    head.alphaVariety = alphabet.size();
    head.lastBitNum = getLastBits();

    // Write alpha and frequence one by one
    fout.write((char*)&head, sizeof(head));
    for (auto &alpha : alphabet) {
        alphafreq af;
        af.alpha = alpha.first, af.freq = alpha.second;
        fout.write((char*)&af, sizeof(af));
    }

    // Read an alpha one by one
    fin.read(&c, sizeof(char));
    while (!fin.eof()) {
        for (auto ch : CodeMap[c]) {
            // Write Huffman code to a byte
            if (ch == '1') SET(value, index);
            else RESET(value, index);
            ++index;
            if (index >= 8) {
                index = 0;
                // Write the byte into output file
                fout.write(&value, sizeof(char));
            }
        }
        fin.read(&c, sizeof(char));
    }
    // If not a complete byte, write it
    if (index) fout.write(&value, sizeof(char));

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
    srcFile += ".txt", tarFile += ".huff"; // Add suffix

    if (!getAlphaFreq(srcFile)) {
        cout << "~~~ERROR~~~\n";
        return 0;
    }

    // Build Huffman tree and get Huffman code
    Hufftree tree(alphabet);
    tree.huffcode(CodeMap);

    if (!zip(srcFile, tarFile)) cout << "~~~ERROR~~~\n";

    return 0;
}
