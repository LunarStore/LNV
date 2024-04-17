#include <iostream>
#include <endian.h>
#include <arpa/inet.h>
using namespace std;

bool isLittle(){
    uint16_t val = 0xff00;

    return *(uint8_t*)(&val) == 0;
}
/*
*   总结：
*       1、长的有符号数转换成短有符号数：
*            1）无论长的有符号数是正还是负，都采用截断的形式
*        2、短的有符号数转换成长有符号数：
*            1）短的为负，长的高位补1，保持为负
*            2）短的为正，长的高位补0，保持为正
*
*        有符号数的按位与和按位或：
*            和无符号的过程一样，最高位是否为1（即是否为负）不影响。
*
*        有符号的左移：
*            最高位保持不变，低位补0
*        有符号的右移：
*            最高位保持不变，根据最高位的情况，补0或补1.
**/

// long long数据类型主机序转网络序
void llton(int ll, char* n) {
    *n = ll;
    cout << "int:" << ll << "->" << std::hex << (uint16_t)(uint8_t)(*n) << std::dec << endl;

    cout << (ll & (1 << 31)) << " " << (ll & 0x80) << endl;
}

int main() {
    char buff[10];
    cout << isLittle() << " " << (__BYTE_ORDER == __LITTLE_ENDIAN) << std::endl;

    llton(0xffffff7f, buff);
    llton(0xfffffff1, buff);

    llton(0x7f, buff);
    llton(0xf1, buff);

    cout << ((int)1 >> 1) << endl;

    return 0;
}