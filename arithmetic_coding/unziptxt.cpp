#include "model.hpp"
#include "bits.hpp"

// low为下界，high为上界，value(16位)为判断的值
int low, high = Top_value, value;
Bits bits;  // 创建Bits
Model model;    // 创建模型

bool start(ifstream &fin) {
    // 先读取16位数据到value中
    for (int i = 1; i <= Code_bits; ++i) {
        char bit = bits.input_bit(fin);
        if (bit == -1) return false;    // 异常处理
        value = (value << 1) | bit;
    }
    return true;
}

// 解码
int decode_symbol(ifstream &fin) {
    int range = high - low + 1; // 区间
    int cum = ((value - low + 1) * model.get_sum_freq(0) - 1) / range;  // 所占频数和
    int symbol = 1;
    while (model.get_sum_freq(symbol) > cum) ++symbol;  // 查找标志
    // 找到标志后缩小区间
    high = low + range * model.get_sum_freq(symbol - 1) / model.get_sum_freq(0) - 1;
    low = low + range * model.get_sum_freq(symbol) / model.get_sum_freq(0);
    // 区间扩展
    while (true) {
        if (high < Half) {} // 最高位为0，不需要操作，对左移无影响
        else if (low >= Half) { // 最高位为1，数据的值减Half
            value -= Half;
            low -= Half;
            high -= Half;
        }
        // 又是那个奇妙的算法
        else if (low >= First_qtr && high < Third_qtr) {
            value -= First_qtr;
            low -= First_qtr;
            high -= First_qtr;
        }
        else break;
        // 左移，扩大区间，记得value也要跟着左移
        low <<= 1;
        high = (high << 1) | 1;
        
        char bit = bits.input_bit(fin);
        if (bit == -1) return -1;   // 异常处理
        value = (value << 1) | bit;
    }
    return symbol;
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
    srcFile += ".arc", tarFile += ".txt";

    ifstream fin(srcFile, ios::binary); // 二进制形式打开
    ofstream fout(tarFile, ios::binary);
    if (!fin.is_open()) {   // 是否正常打开
        cerr << "Open file failed! Filename : " << srcFile << "\n";
        return -1;
    }

    char ch;
    int symbol;
    // 预处理
    if (!start(fin)) {
        cerr << "Bad input file\n";
        return -1;
    }
    // 开始解码
    while (true) {
        symbol = decode_symbol(fin);    // 获得标志
        if (symbol == -1) { // 异常处理
            cerr << "Bad input file1\n";
            return -1;
        }
        if (symbol == EOF_symbol) break;    // 判断文件结尾
        ch = symbol_to_char(symbol);    // 获得字符
        fout.write(&ch, sizeof(char));  // 写入文件
        model.update(symbol);   // 更新模型
    }
    
    fin.close();    // 关闭文件
    fout.close();
    return 0;
}