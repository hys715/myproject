#ifndef MODEL_H
#define MODEL_H

#define Code_value_bits 16  // 编码位数
#define Top_value 0xffff    // 区间上界
#define First_qtr 0x4000    // 1/4区间
#define Half      0x8000    // 1/2区间
#define Third_qtr 0xc000    // 3/4区间

#define No_of_symbols 257 // 字符总数0-256
#define EOF_symbol 256  // 自定义结束符
#define Max_freq 0x3fff // 最大频数总和

// freq[i]为字符频数，ans_freq[i]为频数前缀和
// ans_freq[i] = freq[0] + freq[1] + ... + freq[i - 1]
// ans_freq[0] = 0
int ans_freq[No_of_symbols + 1], freq[No_of_symbols + 1];

// 模型初始化
void start_model() {
    for (int i = 0; i <= No_of_symbols; ++i)
        ans_freq[i] = i, freq[i] = 1;
    freq[257] = 0; // 257不是字符
}

// 更新模型
void update_model(int symbol) {
    // 频数总和达到最大后，按1/2比例缩小
    if (ans_freq[No_of_symbols] == Max_freq) {
        int ans = 0;
        for (int i = 0; i <= No_of_symbols; ++i) {
            freq[i] = (freq[i] + 1) >> 1;   // 缩小
            ans_freq[i] = ans;  // 计算新的加和
            ans += freq[i];
        }
    }
    ++freq[symbol]; // 更新频数及加和
    for (int i = symbol + 1; i <= No_of_symbols; ++i)
        ++ans_freq[i];
}

#endif