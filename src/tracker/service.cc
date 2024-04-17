#include "tracker/service.hpp"
#include "common/proto.hpp"
#include "common/util.hpp"
#include "tracker/globals.hpp"
#include "tracker/db.hpp"

#include <algorithm>

bool service_c::business(acl::socket_stream* conn, const char* head) const {
    long long body_length = ntoll(head);
    int8_t command = head[BODYLEN_SIZE];
    int8_t status = head[BODYLEN_SIZE + COMMAND_SIZE];
    bool rt = false;
    if (body_length < 0) {
        error(conn, -1, "invalid body length: %lld < 0", body_length);
        return false;
    }

    logger("body_length: %lld, command: %d, status: %d",
        body_length, command, status);

    switch (command) {
        case CMD_TRACKER_JOIN:
            rt = join(conn, body_length);
            break;
        case CMD_TRACKER_BEAT:
        rt = beat(conn, body_length);
                break;
        case CMD_TRACKER_SADDRS:
        rt = saddrs(conn, body_length);
                break;
        case CMD_TRACKER_GROUPS:
        rt = groups(conn);
                break;
        default:
            error(conn, -1, "unknow command: %d", command);
            return false;

    }

    return rt;
}
////////////////////////////////////////////////////////////////////////
// 来自存储服务器的加入包
bool service_c::join(acl::socket_stream* conn, long long bodylen) const {

}

// 处理来之存储服务器的心跳包
bool service_c::beat(acl::socket_stream* conn, long long bodylen) const {

}

// 处理来自客户端的获取存储服务器地址列表请求。
bool service_c::saddrs(acl::socket_stream* conn, long long bodylen) const {

}

// 处理来自客户机的获取组列表请求
bool service_c::groups(acl::socket_stream* conn) const {
    
}

////////////////////////////////////////////////////////////////////////

int service_c::join(const storage_join_t* sj, const char* saddr) const {

}

// 将存储服务器标为活动
int service_c::beat(const char* groupname, const char hostname,
    const char* saddr) const {

}

// 响应客户机存储服务器地址列表
int service_c::saddrs(acl::socket_stream* conn, 
    const char* appid, const char* userid) const {

}

// 根据用户ID获取其对应的组名
int service_c::group_of_user(const char* appid,
    const char* userid, std::string& groupname) const {

}

// 根据组名获取存储服务器地址列表
int service_c::saddrs_of_group(const char* groupname,
    std::string& saddrs) const {

}

////////////////////////////////////////////////////////////////////////
    // 应答成功
bool service_c::ok(acl::socket_stream* conn) const {

}
// 错误应答
bool service_c::error(acl::socket_stream* conn, short errnumb,
    const char* format, ...) const {

}