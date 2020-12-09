#include "model.hpp"
#include "bits.hpp"

// low区间下界，high区间上界
// value为压缩文件结果值
int low, high, value;

// 开始译码
bool start_decode(ifstream& fin) {
    value = 0;
    for (int i = 1; i <= Code_value_bits; ++i) {
        int bit = input_bit(fin);
        if (bit == -1) return false;    // 异常处理
        value = (value << 1) | bit;
    }
    low = 0, high = Top_value;
    return true;
}

// 译码得到symbol
int decode_symbol(ifstream& fin) {
    int range = high - low + 1; // 区间长度
    int cum = ((value - low + 1) * cum_freq[0] - 1) / range; // value占比
    int symbol = 1;
    while (cum_freq[symbol] > cum) ++symbol;    // 查找
    if (symbol == EOF_symbol) return symbol;
    high = low + range * cum_freq[symbol - 1] / cum_freq[0] - 1;    // 更新
    low = low + range * cum_freq[symbol] / cum_freq[0];
    while (true) {
        if (high < Half) {}
        else if (low >= Half) {
            value -= Half;
            low -= Half;
            high -= Half;
        }
        // 关键算法
        else if (low >= First_qtr && high < Third_qtr) {
            value -= First_qtr;
            low -= First_qtr;
            high -= First_qtr;
        }
        else break;
        low <<= 1;
        high = (high << 1) | 1;
        int bit = input_bit(fin);
        if (bit == -1) return -1;   // 异常处理
        value = (value << 1) | bit;
    }
    return symbol;
}

int main(int argc, char* argv[]) {
    // 异常处理
    if (argc < 3 || argc > 3) cerr << "Command error!\n";
    string srcFile(argv[1]), tarFile(argv[2]);
    srcFile += ".arc", tarFile += ".txt";
    ifstream fin(srcFile, ios::binary);
    ofstream fout(tarFile, ios::binary);
    if (!fin.is_open() || !fout.is_open()) {
        cerr << "Fail to open files!\n";
        return -1;
    }

    // 正式解码
    start_model();
    start_input_bits();
    start_decode(fin);
    while (true) {
        int ch, symbol;
        symbol = decode_symbol(fin);
        if (symbol == -1) return -1;    // 异常处理
        if (symbol == EOF_symbol) break;    // 结束
        ch = symbol - 1;
        fout.write((char*)&ch, sizeof(char));
        update_model(symbol);
    }

    fin.close();
    fout.close();
    return 0;
}