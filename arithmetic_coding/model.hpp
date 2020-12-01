#ifndef MODEL_H
#define MODEL_H

#define Code_bits     16    // 编码位数，值为[0, 2^16)
#define Top_value 0xffff    // 最大值2^16 - 1

#define First_qtr 0x4000    // 第一个四等分点
#define Half      0x8000    // 第二个四等分点
#define Third_qtr 0xc000    // 第三个四等分点

#define Num_of_chars 128    // 字符总数
#define EOF_symbol   128    // 文件结束标志
#define Max_freq  0x3fff    // 字符最大频数

#define char_to_symbol(x) (x + 1)   // 字符->标志
#define symbol_to_char(x) (x - 1)   // 标志->字符

// 概率模型
class Model {
private:
    // freq[i]为第i个字符的频数，字符下标：1-127
    // sum_freq[i] = freq[i + 1] + freq[i + 2] + ... + freq[Num_of_chars]
    int freq[Num_of_chars + 1], sum_freq[Num_of_chars + 1];

public:
    Model() {
        for (int i = 0; i <= Num_of_chars; ++i) {
            freq[i] = 1;    // 将所有字符频数初始化为1
            sum_freq[i] = Num_of_chars - i; // 计算频数和
        }
        freq[0] = 0;    // '\0'不计算
    }

    // 返回频数和
    int &get_sum_freq(int symbol) { return sum_freq[symbol]; }

    // 更新模型
    void update(int symbol) {
        if (sum_freq[0] == Max_freq) { // 频数和达到最大值
            int ans = 0;
            for (int i = Num_of_chars; i >= 0; --i) {
                freq[i] = (freq[i] + 1) >> 1;   // 对频数进行放缩
                sum_freq[i] = ans;  // 重新计算频数和
                ans += freq[i];
            }
        }
        ++freq[symbol]; // 更新symbol字符频数，同时更新频数和
        for (int i = 0; i < symbol; ++i)
            ++sum_freq[i];
    }
};

#endif