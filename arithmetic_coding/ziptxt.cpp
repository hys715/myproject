#include "model.hpp"
#include "bits.hpp"

// low区间下界，high区间上界
// bits_to_follow记录放缩次数
int low, high, bits_to_follow;

// 关键算法，详见README.md
void bit_plus_follow(int bit, ofstream& fout) {
    output_bit(bit, fout);
    while (bits_to_follow > 0) {
        output_bit(!bit, fout);
        --bits_to_follow;
    }
}

// 编码准备
void start_encode() {
    low = bits_to_follow = 0;
    high = Top_value;
}

// 编码symbol
void encode_symbol(int symbol, ofstream& fout) {
    int range = high - low + 1; // 区间[low, high]长度
    // 由symbol重新确定low、high
    high = low + range * cum_freq[symbol - 1] / cum_freq[0] - 1;
    low = low + range * cum_freq[symbol] / cum_freq[0];
    while (true) {
        // low、high第8位都为0
        if (high < Half) bit_plus_follow(0, fout);
        // low、high第8位都为1
        else if (low >= Half) {
            bit_plus_follow(1, fout);
            low -= Half;
            high -= Half;
        }
        // 关键算法，放缩
        else if (low >= First_qtr && high < Third_qtr) {
            ++bits_to_follow;
            low -= First_qtr;
            high -= First_qtr;
        }
        else break;
        // 左移扩大区间
        low = low << 1;
        high = (high << 1) | 1;
    }
}

// 确定区间一个明确的数
void done_encode(ofstream& fout) {
    ++bits_to_follow;
    if (low < First_qtr) bit_plus_follow(0, fout);
    else bit_plus_follow(1, fout);
}

int main(int argc, char* argv[]) {
    // 异常处理
    if (argc < 3 || argc > 3) cerr << "Command error!\n";
    string srcFile(argv[1]), tarFile(argv[2]);
    srcFile += ".txt", tarFile += ".arc";
    ifstream fin(srcFile, ios::binary);
    ofstream fout(tarFile, ios::binary);
    if (!fin.is_open() || !fout.is_open()) {
        cerr << "Fail to open files!\n";
        return -1;
    }

    // 正式编码
    start_model();
    start_output_bits();
    start_encode();
    int ch = 0, symbol = 0;
    fin.read((char*)&ch, sizeof(char));
    while (!fin.eof()) {
        symbol = ch + 1;
        encode_symbol(symbol, fout);
        update_model(symbol);
        fin.read((char*)&ch, sizeof(char));
    }
    encode_symbol(EOF_symbol, fout);    // 添加自定义结束符
    done_encode(fout);
    done_output_bits(fout);

    fin.close();
    fout.close();
    return 0;
}