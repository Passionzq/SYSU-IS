#pragma once

#include <iostream>
#include <cstring>
#include <string>

using namespace std;

// 轮函数F
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))

// 轮函数G
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))

// 轮函数H
#define H(x, y, z) ((x) ^ (y) ^ (z))

// 轮函数I
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// 循环座椅n位
#define SHIFT_LEFT(num, n) (((num) << (n)) | ((num) >> (32-(n))))

// 每轮循环中F函数的迭代运算逻辑
#define FF(a, b, c, d, x, s, ac) { \
  (a) += F ((b), (c), (d)) + (x) + ac; \
  (a) = SHIFT_LEFT ((a), (s)); \
  (a) += (b); \
}

// 每轮循环中G函数的迭代运算逻辑
#define GG(a, b, c, d, x, s, ac) { \
  (a) += G ((b), (c), (d)) + (x) + ac; \
  (a) = SHIFT_LEFT ((a), (s)); \
  (a) += (b); \
}

// 每轮循环中H函数的迭代运算逻辑
#define HH(a, b, c, d, x, s, ac) { \
  (a) += H ((b), (c), (d)) + (x) + ac; \
  (a) = SHIFT_LEFT ((a), (s)); \
  (a) += (b); \
}

// 每轮循环中I函数的迭代运算逻辑
#define II(a, b, c, d, x, s, ac) { \
  (a) += I ((b), (c), (d)) + (x) + ac; \
  (a) = SHIFT_LEFT ((a), (s)); \
  (a) += (b); \
}

#define CPY memcpy
#define TC type_change
typedef unsigned char byte;
typedef unsigned int bit32;

// 用于补位字符串，最长512bits = 64bytes
static byte PADDING[64] = {
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 各轮循环中第 i 次迭代 (i = 1..16) 使用的 X[k] 的确定:
static bit32 X[64] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12,
  5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2,
  0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9
};

// 各次迭代运算 (1..64) 采用的 T 值的计算结果
static bit32 T[64] = {
  0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
  0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
  0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
  0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
  0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
  0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
  0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
  0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

// 各次迭代运算 (1..64) 采用的左循环移位的 s 值:
static bit32 s[64] = {
  7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
  5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
  4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
  6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

// 十六进制下的字符，用于打印
static char PRINT_CHAR[16] = {
  '0', '1', '2', '3',
  '4', '5', '6', '7',
  '8', '9', 'a', 'b',
  'c', 'd', 'e', 'f'
};

// ABCD寄存器（小端模式）
static bit32 state[4] = {
    0x67452301, 
    0xefcdab89,
    0x98badcfe,
    0x10325476
};

// 输入信息的长度，count[0]存储低位
static bit32 count[2] = {0, 0};

// 512位输入寄存器
static byte buffer[64];

// 信息摘要（输出结果）
static byte digest[16];

// 类型转换 0:bit32->byte, 1:byte->bit32
void type_change(byte* btyes, bit32* bit32s, size_t len, int judge);

// 轮函数
void round(byte block[64]);

// MD5主体过程
void MD5(byte* input, size_t len);

// 计算message digest
void calculate_digest(string msg);

// 打印结果
void print_ans(string msg);