#ifndef BITS_H
#define BITS_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// buffer一字节缓冲区，暂存移出的高位，声明成int是为了防止溢出
// bits_to_go为buffer中要处理的位数
int buffer, bits_to_go;

// 初始化读入
void start_input_bits() { bits_to_go = 0; }

// 读入bit
int input_bit(ifstream& fin) {
    if (bits_to_go == 0) {
        if (fin.eof()) {    // 异常处理
            cerr << "Bad input file!\n";
            return -1;
        }
        fin.read((char*)&buffer, sizeof(char)); // 读入一字节
        bits_to_go = 8;
    }
    int t = buffer & 1; // 取最低位
    buffer >>= 1;
    --bits_to_go;
    return t;
}

// 初始化输出
void start_output_bits() { buffer = 0, bits_to_go = 8; }

// 输出bit
void output_bit(int bit, ofstream& fout) {
    buffer >>= 1;
    if (bit) buffer |= 0x80;    // bit存buffer的第8位
    --bits_to_go;
    if (bits_to_go == 0) {  // buffer无需要处理的位，写入文件
        fout.write((char*)&buffer, sizeof(char));
        bits_to_go = 8; // 重置
    }
}

// 输出剩余位
void done_output_bits(ofstream& fout) {
    buffer >>= bits_to_go;  // 将有效位全移至右边
    fout.write((char*)&buffer, sizeof(char));
}

#endif