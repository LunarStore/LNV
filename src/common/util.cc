
#include <string.h>
#include <stdlib.h>

#include "common/types.hpp"
#include "common/util.hpp"

// long long数据类型主机序转网络序
void llton(long long ll, char* n) {
    for (int i = 0; i < sizeof(ll); i++) {
        n[i] = ll >> (8 * (sizeof(ll) - 1 - i));
    }
}

// long long数据类型网络序转主机序
long long ntoll(const char* n) {
    long long ll = 0;
    for (int i = 0; i < sizeof(ll); i++) {
        ll  = ll | ((long long)(unsigned char)n[i] << (8 * (sizeof(ll)) - 1 - i));
    }

    return ll;
}

// long 数据类型主机序转网络序
void lton(long l, char* n) {
    for (int i = 0; i < sizeof(l); i++) {
        n[i] = l >> (8 * (sizeof(l) - 1 - i));
    }
}

// long 数据类型网络序转主机序
long ntol(const char* n) {
    long l = 0;
    for (int i = 0; i < sizeof(l); i++) {
        l  = l | ((long)(unsigned char)n[i] << (8 * (sizeof(l)) - 1 - i));
    }

    return l;
}

// short 数据类型主机序转网络序
void ston(short s, char* n) {
    for (int i = 0; i < sizeof(s); i++) {
        n[i] = s >> (8 * (sizeof(s) - 1 - i));
    }
}

// short 数据类型网络序转主机序
short ntos(const char* n) {
    short s = 0;
    for (int i = 0; i < sizeof(s); i++) {
        s  = s | ((short)(unsigned char)n[i] << (8 * (sizeof(s)) - 1 - i));
    }

    return s;
}


// 字符串是否合法，24个英文字母 + 0-9十个阿拉伯数字，即是合法的。
int valid(const char* str) {
    if (!str) {
        return ERROR;
    }
    size_t len = strlen(str);
    if (!len) {
        return ERROR;
    }

    for (size_t i = 0; i < len; i++) {
        if (str[i] >= 'a' && str[i] <= 'z' ||
            str[i] >= 'A' && str[i] <= 'Z' ||
            str[i] >= '0' && str[i] <= '9') {
            continue;
        }
        return ERROR;
    }

    return OK;
}

// 以分号为分隔符，分割字符串
int split(const char* str, std::vector<std::string>& substrs) {
    int front = 0, back = 0;

    if (!str) return ERROR;
    size_t len = strlen(str);
    if (!len) return ERROR;
    char* ptr = (char *)malloc(len + 1);
    strcpy(ptr, str);

    for (char* sub = strtok(ptr, ";"); sub != nullptr; sub = strtok(nullptr, ";")) {

        substrs.push_back(sub);
    }

    free(ptr);
    return OK;
}
