#include "model.hpp"
#include "bits.hpp"

// low为下界，high为上界，bits_to_follow在区间扩展时做记录
int low, high = Top_value, bits_to_follow;
Bits bits;  // 创建Bits
Model model;    // 创建模型

// 这是一个奇妙的算法，具体原理请自行摸索
void bit_plus_follow(char bit, ofstream &fout) {
    bits.output_bit(bit, fout);
    while (bits_to_follow > 0) {
        bits.output_bit(!bit, fout);
        --bits_to_follow;
    }
}

// 对symbol进行编码
void encode_symbol(int symbol, ofstream &fout) {
    int range = high - low + 1; // 区间大小
    // 缩小区间从而编码symbol
    high = low + range * model.get_sum_freq(symbol - 1) / model.get_sum_freq(0) - 1;
    low = low + range * model.get_sum_freq(symbol) / model.get_sum_freq(0);
    // 下面是区间的扩展
    while (true) {
        if (high < Half) bit_plus_follow(0, fout);  // 最高位为0，输出0
        else if (low >= Half) {
            bit_plus_follow(1, fout);   // 最高位为1，输出1，然后最高位置0
            low -= Half;
            high -= Half;
        }
        // First_qtr <= low < Half <= high < Third_qtr
        // 请联系上述奇妙算法
        else if (low >= First_qtr && high < Third_qtr) {
            ++bits_to_follow;
            low -= First_qtr;
            high -= First_qtr;
        }
        else break;
        // 左移，扩大区间
        low = low << 1;
        high = (high << 1) | 1;
    }
}

// 结束编码，确定一个具体的value，以便后续解码
void done_encode(ofstream &fout) {
    ++bits_to_follow;
    if (low < First_qtr) bit_plus_follow(0, fout);
    else bit_plus_follow(1, fout);
}

int main(int argc, char *argv[]) {
    // 函数参数判断
    if (argc < 3 || argc > 3) {
        if (argc == 1) cerr << "Source and target files are missing!\n";
        else if (argc == 2)  cerr << "Target file is missing!\n";
        else cerr << "~~~ERROR~~~\n";
        return -1;
    }

    string srcFile(argv[1]), tarFile(argv[2]);  // 源文件与目标文件
    srcFile += ".txt", tarFile += ".arc";

    ifstream fin(srcFile, ios::binary); // 二进制形式打开
    ofstream fout(tarFile, ios::binary);
    if (!fin.is_open()) {   // 是否正常打开
        cerr << "Open file failed! Filename : " << srcFile << "\n";
        return -1;
    }

    char ch;
    int symbol;
    fin.read(&ch, sizeof(char));    // 读字节开始编码
    while (!fin.eof()) {    // 文件是否结束
        symbol = char_to_symbol(ch);    // 获得标志
        encode_symbol(symbol, fout);    // 对标志编码
        model.update(symbol);   // 更新模型
        fin.read(&ch, sizeof(char));    // 读字节
    }
    encode_symbol(EOF_symbol, fout);    // 编码自定义文件结束符
    done_encode(fout);  // 结束编码
    bits.done_output_bits(fout);    // 将buffer清空

    fin.close();    // 关闭文件
    fout.close();
    return 0;
}