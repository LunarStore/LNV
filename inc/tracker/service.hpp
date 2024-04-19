#ifndef __TRACKER_SERVICE_HPP__
#define __TRACKER_SERVICE_HPP__

#include <lib_acl.hpp>
#include <common/types.hpp>

class service_c {
public:
    bool business(acl::socket_stream* conn, const char* head) const;
private:
    // 来自存储服务器的加入包
    bool join(acl::socket_stream* conn, long long bodylen) const;

    // 处理来之存储服务器的心跳包
    bool beat(acl::socket_stream* conn, long long bodylen) const;

    // 处理来自客户端的获取存储服务器地址列表请求。
    bool saddrs(acl::socket_stream* conn, long long bodylen) const;

    // 处理来自客户机的获取组列表请求
    bool groups(acl::socket_stream* conn) const;

    int join(const storage_join_t* sj, const char* saddr) const;

    // 将存储服务器标为活动
    int beat(const char* groupname, const char* hostname,
        const char* saddr) const;

    // 响应客户机存储服务器地址列表
    int saddrs(acl::socket_stream* conn, 
        const char* appid, const char* userid) const;

    // 根据用户ID获取其对应的组名
    int group_of_user(const char* appid,
        const char* userid, std::string& groupname) const;

    // 根据组名获取存储服务器地址列表
    int saddrs_of_group(const char* groupname,
        std::string& saddrs) const;

    // 应答成功
    bool ok(acl::socket_stream* conn) const;
    // 错误应答
    bool error(acl::socket_stream* conn, short errnumb,
        const char* format, ...) const;
};
#endif