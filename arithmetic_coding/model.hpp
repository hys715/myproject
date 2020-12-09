#ifndef MODEL_H
#define MODEL_H

#define Code_value_bits 16  // 编码位数
#define Top_value 0xffff    // 区间上界
#define First_qtr 0x4000    // 1/4区间
#define Half      0x8000    // 1/2区间
#define Third_qtr 0xc000    // 3/4区间

#define No_of_chars 257 // 字符总数
#define EOF_symbol 257  // 自定义结束符
#define Max_freq 0x3fff // 最大频数总和

// freq[i]为字符频数，cum_freq[i]为频数加和
// cum_freq[i] = freq[i+1] + freq[i+2] + ... + freq[No_of_chars]
int cum_freq[No_of_chars + 1], freq[No_of_chars + 1];

// 模型初始化
void start_model() {
    for (int i = 0; i <= No_of_chars; ++i)
        cum_freq[i] = No_of_chars - i, freq[i] = 1;
    freq[0] = 0;
}

// 更新模型
void update_model(int symbol) {
    // 频数总和达到最大后，按1/2比例缩小
    if (cum_freq[0] == Max_freq) {
        int cum = 0;
        for (int i = No_of_chars; i >= 0; --i) {
            freq[i] = (freq[i] + 1) >> 1;   // 缩小
            cum_freq[i] = cum;  // 计算新的加和
            cum += freq[i];
        }
    }
    ++freq[symbol]; // 更新频数及加和
    for (int i = 0; i < symbol; ++i)
        ++cum_freq[i];
}

#endif