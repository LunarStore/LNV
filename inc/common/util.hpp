#ifndef __COMMON_UTIL_HPP__
#define __COMMON_UTIL_HPP__

#include <string>
#include <vector>

// long long数据类型主机序转网络序
void llton(long long ll, char* n);

// long long数据类型网络序转主机序
long long ntoll(const char* n);

// long 数据类型主机序转网络序
void lton(long l, char* n);

// long 数据类型网络序转主机序
long ntol(const char* n);

// short 数据类型主机序转网络序
void ston(short s, char* n);

// short 数据类型网络序转主机序
short ntos(const char* n);


// 字符串是否合法，24个英文字母 + 0-9十个阿拉伯数字，即是合法的。
int valid(const char* str);

// 以分号为分隔符，分割字符串
int split(const char* str, std::vector<std::string>& substrs);
#endif