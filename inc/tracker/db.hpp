#ifndef __TRACKER_DB_HPP__

#define __TRACKER_DB_HPP__

#include <string>
#include <vector>
#include <mysql.h>

// 数据库访问类
class db_c {
public:
    db_c(void);
    ~db_c(void);

    // 连接数据库
    int connect();
    // 根据用户ID获取对应组名
    int get(const char* userid, std::string& groupname) const;

    // 设置用户ID和组名的对应关系
    int set(const char* appid, const char* userid,
        const char* groupname) const;

    int get(std::vector<std::string>& groupnames) const;
private:
    MYSQL* m_mysql;
};
#endif