// Programmer : hys
// Date : 2020-11-12
// Objective : Structure and identification of compression and decompression
// Note : Position is from high to low

#ifndef FUNCTION_H
#define FUNCTION_H

// Set the pos-th position of the byte to 1
#define SET(byte, pos) (byte |= (1 << (pos ^ 7)))

// Set the pos-th position of the byte to 0
#define RESET(byte, pos) (byte &= (~(1 << (pos ^ 7))))

// Get the pos-th position of the byte
#define GET(byte, pos) ((byte & (1 << (pos ^ 7))) != 0)

// Identification of compression
struct fileHead {
    char flag[4]; // flag is "huff"
    char alphaVariety; // Variety of alpha
    char lastBitNum; // The number of significant bits of the last byte
    char unused[2]; // Unused space
};

// Alpha and frequence
struct alphafreq {
    char alpha;
    int freq;
};

#endif
