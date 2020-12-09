# **文本压缩与解压**  

## **功能描述**  
<font size=3>对文本文件进行压缩和解压</font>  

---

## **算法分析**  

### **算术编码**  
算术编码的基本原理是将编码的消息表示成实数0和1之间的一个间隔，消息越长，编码表示它的间隔就越小，表示这一间隔所需的二进制位就越多。  
算术编码用到两个基本的参数：符号的概率和它的编码间隔。信源符号的概率决定压缩编码的效率，也决定编码过程中信源符号的间隔，而这些间隔包含在0到1之间。编码过程中的间隔决定了符号压缩后的输出。

### **例子**  
假设字符串为“ababc”，字符个数为5，可知P(a)=0.4，P(b)=0.4，P(c)=0.2。  
PS : <-表示属于  

---

**编码**  
(1)对第1个字符a，当前区间: [0,1)，按概率划分区间: a<-[0,0.4)，b<-[0.4,0.8)，c<-[0.8,1)，取区间[0,0.4)  
(2)对第2个字符b，当前区间: [0,0.4)，按概率划分区间: a<-[0,0.16)，b<-[0.16,0.32)，c<-[0.32,0.4)，取区间[0.16,0.32)  
(3)对第2个字符a，当前区间: [0.16,0.32)，按概率划分区间: a<-[0.16,0.224)，b<-[0.224,0.288)，c<-[0.288,0.32)，取区间[0.16,0.224)  
(4)对第2个字符b，当前区间: [0.16,0.224)，按概率划分区间: a<-[0.16,0.1856)，b<-[0.1856,0.2112)，c<-[0.2112,0.224)，取区间[0.1856,0.2112)  
(5)对第2个字符c，当前区间: [0.1856,0.2112)，按概率划分区间: a<-[0.1856,0.19584)，b<-[0.19584,0.20608)，c<-[0.20608,0.2112)，取区间[0.20608,0.2112)  
(6)最终区间[0.20608,0.2112)，任取区间的一个数如0.21，0.21就是编码结果  
编码的伪代码:  
```
low = 0, high = 1
encode_symbol(symbol)
    range = high - low
    high = low + range * HighRange(symbol)
    low = low + range * LowRange(symbol)
```

---

**解码**  
(1)0.21<-[0,0.4)，第1个字符为a  
(2)0.21<-[0.16,0.32)，第2个字符为b  
(3)0.21<-[0.16,0.224)，第3个字符为a  
(4)0.21<-[0.1856,0.2112)，第一个字符为b  
(5)0.21<-[0.20608,0.2112)，第一个字符为c  
(6)得到字符串“ababc”(预先知道字符数为5)  
解码的伪代码:  
```
decode_symbol(value)
    find symbol with value
    range = high - low
    high = low + range * HighRange(symbol)
    low = low + range * LowRange(symbol)
```

---

**特别说明**  
该算数编码方式采用固定统计模型，即预先获得字符频率，然后再进行编码。除固定统计模型外，还有自适应模型。  
**自适应模型 :**将所有字符频率初始化为1，在读取字符的同时修改该字符的频率，只需扫描一遍文本，这种方法适用范围更广，效率更高，但较为复杂，后面我们使用自适应模型  

---

### **计算机表示**  
现实中的小数是无穷位的，可以编码任意字符串，但计算机中的小数是有精度限制的，因此我们在计算机中不用小数来编码，而用整数来模拟小数，例如，我们可以用4个bit，0000-1111(二进制)，代表0.0000-0.1111(二进制)。  
这里我们继续用上面的例子来分析，区间用8个bit表示，同样，我们用频数来代表频率。  
一些说明:1000 0000(最大区间的一半)，0100 0000(最大区间的1/4)，1100 0000(最大区间的3/4)  

---

**为保证算法编码的准确性，我们需要做一些限制:**  
1)规定字符频数总和不能超过0100 0000，且区间长度不小于0100 0000，从而保证按概率缩小区间时，划分的最小区间单元(区间长度/频数总和)大于等于1  
2)当low和high都大于等于1000 0000，或者都小于1000 0000时，我们会将low和high同时左移，扩大区间，将移出的最高位存储起来  
3)当0100 0000 <= low < Half <= high < 1100 0000时，无法直接左移，需要对区间进行适当的放缩  
4)除2、3两种情况外，无需对区间进行操作  

---

#### **编码**
(1)**首先将所有字符频数初始化为1**，即a:1，b:1，c:1，total = 3，low=0000 0000，high=1111 1111  
特别强调一点，abc的顺序是固定的，后面我们会用频数和来缩小区间，频数加和如
sum_freq(a) = freq(a) = 1,  
sum_freq(b) = freq(a) + freq(b) = 2,  
sum_freq(c) = freq(a) + freq(b) + freq(c) = 3  
(2)**读取第1个字符a**(注意: 除法结果向下取整)  
low = 0000 0000，high = 1111 1111  
range = high - low + 1(加一是因为取的是闭区间[low,high]) = 1 0000 0000(二进制)  
high = low + range * sum_freq(a) / total - 1(减一是为了不取上界，因为上界属于字符b) = 0101 0100  
low = low + range * 0 / total = 0000 0000  
因为是自适应模型，我们调整字符频数，a:2，b:1，c:1，total:4  
sum_freq(a) = 2，sum_freq(b) = 3，sum_freq(c) = 4  
low和high都小于1000 0000，左移，low=0000 0000，high=1010 1000，bits(存储移出的位)=0  
*这里我们解释下，为什么这样做 : 我们把low和high看作0.0000 0000和0.0101 0100，后面我们只会对这个区间进行再划分，而小数点后第1位数已经固定是0了，就没必要再看它了，直接将0移出并存储起来*  
low和high符合4)，开始读取下一字符  
(3)**读取第2个字符b**  
low = 0000 0000，high = 1010 1000  
range = high - low + 1 = 1010 1001  
high = low + range * sum_freq(b) / total - 1 = 0111 1101  
low = low + range * sum_freq(a) / total = 0101 0100  
调整频数，a:2，b:2，c:1，total:5，sum_freq(a) = 2，sum_freq(b) = 4，sum_freq(c) = 5  
low和high都小于1000 0000，左移，low=1010 1000，high=1111 1010，bits=00  
low和high都大于等于1000 0000，左移，low=0101 0000，high=1111 0100，bits=001  
low和high符合4)，开始读取下一字符  
(4)**读取第3个字符a**  
low = 0101 1000，high = 1111 0100  
range = high - low + 1 = 1010 0101  
high = low + range * sum_freq(a) / total - 1 = 1010 0101  
low = low + range * 0 / total = 0101 0000  
调整频数，a:3，b:2，c:1，total:6，sum_freq(a) = 3，sum_freq(b) = 5，sum_freq(c) = 6  
low和high不符合3)  
但是low和high的最高位不一致，无法一起左移，这时候就需要将区间进行适当的放缩，如下图(First_qtr=0100 0000, Half=1000 0000, Third_qtr=1100 0000)    
![](https://github.com/hys715/myproject/blob/master/arithmetic_coding/image/extend.png)  
具体的做法为:  
low = (low - First_qtr) * 2 = 2 * low - Half = 0011 0000  
high = (high - First_qtr) * 2 + 1 = 2 * high - Half + 1 = 1100 1010  
这相当于将low向下移动(Half-low)个长度，high向上移动(high-Half+1)个长度，即区间放大两倍，记录放大次数cnt=1  
low和high符合4)，开始读取下一字符  
(5)**读取第4个字符b**  
low = 0011 0000，high = 1100 1010  
range = high - low + 1 = 1001 1011  
high = low + range * sum_freq(b) / total - 1 = 1011 0000   
low = low + range * sum_freq(a) / total = 0111 1101  
调整频数，a:3，b:3，c:1，total:7，sum_freq(a) = 3，sum_freq(b) = 6，sum_freq(c) = 7  
low和high符合3)，继续放缩  
low = 2 * low - Half = 0111 1010  
high = 2 * high - Half + 1 = 1110 0001  
cnt = 2  
low和high符合4)，开始读取下一字符  
(6)**读取第5个字符c**  
low = 0111 1010，high = 1110 0001  
range = high - low + 1 = 0110 1000  
high = low + range * sum_freq(c) / total - 1 = 1110 0001   
low = low + range * sum_freq(b) / total = 1100 1000  
调整频数，a:3，b:3，c:2，total:8，sum_freq(a) = 3，sum_freq(b) = 6，sum_freq(c) = 8  
low和high都大于等于1000 0000，左移，low=1001 0000，high=1100 0010，bits=0011  
同时，还需要将放大的两次进行还原，即将cnt清零，需要在移出位再加2个0，即bits=001100  
*因为字符c确定的区间在[1100 1000, 1110 0001]之间，还原时将该区间的点到1000 0000的距离缩小一半，得[1010 0100, 1011 0000]，再缩小一半，得[1001 0010, 1001 1000]，故需在原先的bits后加100(放大2次加取区间后放大1次，共3次，故需移出3位)  
通过这个，我们不难推出，当发生情况3)时，我们对区间进行适当放大，并记录次数，直到遇到情况1)时，将最高位(假设为bit)移入bits后，并在bits之后再添加cnt位!bit，即最高位为1，则补cnt个0，最高位为0，则补cnt个1*  
这时，low和high都大于等于1000 0000，左移，low=0010 0000，high=1000 0100，bits=0011001  
low和high符合4)，已无下一字符  
(7)**最后一步需要在该区间内确定一个数**，通过cnt加一(之前可能还没清零)，判断，若low<0100 0000，则bits后加0，再清除cnt，否则bits后加1，再清除cnt  
最终确定的bits=0011 0010 1(在最后的区间中)，因为文件最小是以字节写入的，bits已经超过8位，所以先将前8位写入文件中，最后一位1扩展成1000 0000，再写入文件中  

---

#### **解码**  
存储在文件中的数为0011 0010、1000 0000  
(1)**先取前8位，value=0011 0010**  
low=0000 0000, high=1111 1111, a:1, b:1, c:1, total:3  
确定区间[0000 0000, 0101 0100]，得到字符a  
(2)**再取8位，放置在byte=1000 0000**  
区间不符合情况4)，区间扩展[0000 0000, 1010 1000]  
value左移，将byte最高位置于value最低位，byte左移，即value = value << 1 + byte & 1000 0000，byte = byte << 1  
value=0110 0101，byte=0000 000  
low=0000 0000, high=0101 0100, a:2, b:1, c:1, total:4  
确定区间[0101 0100, 0111 1101]，得到字符b  
(3)**区间不符合情况4)，区间扩展[1010 1000, 1111 1010]**  
value和byte左移，value=1100 1010，byte=0000 00  
(4)**区间不符合情况4)，区间扩展[0101 0000， 1111 0100]**  
value和byte左移，value=1001 0100，byte=0000 0  
确定区间[0101 0000, 1010 0101]，得到字符a  
(5)**区间符合情况3)，进行适当放大**  
low=0011 0000, high=1100 1010, value=1010 1000, byte=0000  
确定区间[0111 1101, 1011 0000]，得到字符b  
(6)**区间符合情况3)，进行适当放大**  
low=0111 1010, high=1110 0001, value=1101 0000, byte=000  
确定区间[1100 1000, 1110 0001]，得到字符c  
得到字符串“ababc”  

---

*考虑到编码的频率范围，实际上采用的是16位的数据，即low=0x0000，high=0xffff，确保区间不小于0x4000，同时频率和不超过0x3fff，当然，存储移出并写入文件的bits还是8位*  
*当频率和超过0x3fff时，我们可以将所有字符频率减小一半，这不会影响编码的正确性，因为编码和解码都是动态变化的，在编码或者解码同一个字符时，各字符概率在编码和解码时一一对应*  
*在代码实现中，high左移时最低为同时补1，这仅仅是为了更好地扩大区间，并不会有影响*  
*在编码和解码过程中，我们并不存储字符个数(字符多的话，内存开销相对较大)，而是引入一个结束标识EOF_symbol，在编码完所有字符后，对结束标识编码，解码时识别到该标识就停止*  

---

## **代码实现**  
**源文件 :** *ziptxt.cpp unziptxt.cpp*  
**头文件 :** *model.hpp bits.hpp*  

- **model.hpp :** *概率模型，负责统计字符频数*  
```
#ifndef MODEL_H
#define MODEL_H

#define Code_value_bits 16  // 编码位数
#define Top_value 0xffff    // 区间上界
#define First_qtr 0x4000    // 1/4区间
#define Half      0x8000    // 1/2区间
#define Third_qtr 0xc000    // 3/4区间

#define No_of_chars 256 // 字符总数
#define EOF_symbol 256  // 自定义结束符
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
```

- **bits.hpp :** *对字节的 bit位进行操作，辅助二进制文件流写入读出*  
```
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
```

- **ziptxt.cpp :** *压缩源文件*  
```
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
    if (!fin.is_open() || !fout.is_open()) return -1;

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
```

- **unziptxt.cpp :** *解压源文件*  
```
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
    srcFile += ".txt", tarFile += ".arc";
    ifstream fin(srcFile, ios::binary);
    ofstream fout(tarFile, ios::binary);
    if (!fin.is_open() || !fout.is_open()) return -1;

    // 正式解码
    start_model();
    start_input_bits();
    start_decode(fin);
    while (true) {
        int ch, symbol;
        symbol = decode_symbol(fin);
        if (symbol == -1) return -1;
        if (symbol == EOF_symbol) break;
        ch = symbol - 1;
        fout.write((char*)&ch, sizeof(char));
        update_model(symbol);
    }

    fin.close();
    fout.close();
    return 0;
}
```

---

## **链接**  
<https://blog.csdn.net/adam_tu/article/details/7696455>  
<https://segmentfault.com/a/1190000011561822>  
<https://blog.csdn.net/qq_35413770/article/details/88060493>