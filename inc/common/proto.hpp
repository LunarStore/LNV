// 公共模块
// 定义与报文规约有关的宏和数据类型

#ifndef __COMMON_PROTO_HPP__
#define __COMMON_PROTO_HPP__

#include "common/types.hpp"
/*
*   |包体长（8）|命令（1）|状态（1）|包体（包体长度）|
*/
#define BODYLEN_SIZE 8  // 包体长度字节数
#define COMMAND_SIZE 1  // 命令字节数
#define STATUS_SIZE 1   // 状态字节数
#define HEADLEN (BODYLEN_SIZE + COMMAND_SIZE + STATUS_SIZE) // 包头长

/*
*   |包体长度（8）|命令（1）|状态（1）|错误号（2）|错误描述（<=1024）|
*/
#define ERROR_NUMB_SIZE 2    // 错误号字节数
#define ERROR_DESC_SIZE 1024    // 错误描述最大字节数

/*
*   |包体长度（8）|命令（1）|状态（1）|应用ID（16）|用户ID（256)|文件ID（128）|
*/
#define APPID_SIZE   16 // 应用ID最大字节数(含结尾空字符)
#define USERID_SIZE 256 // 用户ID最大字节数(含结尾空字符)
#define FILEID_SIZE 128 // 文件ID最大字节数(含结尾空字符)

typedef struct storage_join_body {
    char sjb_version[STORAGE_VERSION_MAX + 1];  // 版本
    char sjb_groupname[STORAGE_GROUPNAME_MAX + 1]; // 组名 
    char sjb_hostname[STORAGE_HOSTNAME_MAX + 1]; // 主机名
    char sjb_port[sizeof(in_port_t)]; // 端口号
    char sjb_stime[sizeof(time_t)]; // 启动时间
    char sjb_jtime[sizeof(time_t)]; // 加入时间
} storage_join_body_t;

typedef struct storage_beat_body {
    char sbb_groupname[STORAGE_GROUPNAME_MAX + 1]; // 组名 
    char sbb_hostname[STORAGE_HOSTNAME_MAX + 1]; // 主机名
} storage_beat_body_t;

// 命令

#define CMD_TRACKER_JOIN 10 // 存储服务器向跟踪服务器发送加入包
#define CMD_TRACKER_BEAT 11 // 存储服务器向跟踪服务器发送心跳包
#define CMD_TRACKER_SADDRS 12// 客户机从跟踪服务器获取存储服务器的地址列表
#define CMD_TRACKER_GROUPS 13 // 客户机从跟踪服务器获取组列表

#define CMD_ID_GET 40 // 存储服务器从ID服务器获取ID

#define CMD_STORAGE_UPLOAD 70 // 客户机向存储服务器上传文件
#define CMD_STORAGE_FILESIZE 71 // 客户机向存储服务器询问文件大小
#define CMD_STORAGE_DOWNLOAD 72 // 客户机从存储服务器下载文件
#define CMD_STORAGE_DELETE 73 // 客户机散出存储服务器的文件

#define CMD_TRACKER_REPLY 100 // 跟踪服务器应答
#define CMD_ID_REPLY      101 // ID服务器应答
#define CMD_STORAGE_REPLY 102 // 存储服务器应答

#endif