#ifndef BITS
#define BITS

#include <iostream>
#include <fstream>
#include <string>
using std::ifstream;
using std::ofstream;
using std::cerr;
using std::ios;
using std::string;

class Bits {
private:
    // buffer为8位的缓冲区(声明成int是为了防止溢出)
    // bits_counter代表buffer有效位数的
    // eof_counter统计解压文件EOF的个数，目的是检查文件是否正确
    int buffer, bits_counter, eof_counter;

public:
    // op主要判断Bits用于输入还是输出
    Bits() : buffer(0), bits_counter(0), eof_counter(0) {}

    // 读入一个bit
    char input_bit(ifstream &fin) {
        if (bits_counter == 0) { // buffer已经清空
            fin.read((char *)&buffer, sizeof(char));    // 读入一个字节
            if (buffer == EOF) {    // 异常判断
                ++eof_counter;
                if (eof_counter == 5) { // 多统计几次，防止出错
                    cerr << "Bad input file\n";
                    return -1;
                }
            }
            bits_counter = 8;   // buffer有效位为8
        }
        char bit = buffer & 1;  // 输出最低位
        buffer >>= 1;
        --bits_counter; // 有效位减一
        return bit;
    }

    // 输出一个bit
    void output_bit(char bit, ofstream &fout) {
        buffer >>= 1;
        if (bit) buffer |= 0x80;    // bit添加在最高位
        ++bits_counter; // 有效位加一
        if (bits_counter == 8) {    // buffer装满了
            fout.write((char *)&buffer, sizeof(char));  // 写入文件
            bits_counter = 0;   // 有效位清零
        }
    }

    void done_output_bits(ofstream &fout) {
        buffer >>= bits_counter ^ 7;    // 将有效位右移，无效的高位补0
        fout.write((char *)&buffer, sizeof(char));  // 写入文件
    }
};

#endif