#ifndef __02_CACHE_HPP__

#define __02_CACHE_HPP__

#include <lib_acl.hpp>

class cache_c {
public:
    // 根据建获取值
    int get(const char* key, acl::string& value) const;

    // 设置建的值
    int set(const char* key, const char* value, int timeout = -1) const;

    // 删除指定键值对
    int del(const char* key) const;
};

#endif