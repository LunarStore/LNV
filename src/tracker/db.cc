#include "tracker/db.hpp"
#include "tracker/globals.hpp"
#include "tracker/cache.hpp"
#include <assert.h>
db_c::db_c(void): m_mysql(mysql_init(nullptr) ){
    if (!m_mysql) {
        logger_error("create dao fail: %s", mysql_error(m_mysql));
    }
}
db_c::~db_c(void) {
    if (m_mysql) {
        mysql_close(m_mysql);
        m_mysql = nullptr;
    }
}

// 连接数据库
int db_c::connect() {

    for (std::vector<std::string>::const_iterator itr = g_maddrs.begin();
        itr != g_maddrs.end(); itr++) {
        MYSQL* temp = mysql_real_connect(m_mysql, itr->c_str(), 
            "root", "198181", "tnv_trackerdb", 3306, nullptr, 0);
        if (temp != nullptr) {
            m_mysql = temp;
            return OK;
        }
    }

    logger_error("connect database fail: %s",
        mysql_error(m_mysql));
    assert(m_mysql != nullptr);

    return ERROR;
}
// 根据用户ID获取对应组名
int db_c::get(const char* userid, std::string& groupname) const {
    // 先从缓存中获取与用户ID对应的组名
    cache_c cache;
    acl::string key, value;

    key.format("userid:%s", userid);
    if (cache.get(key.c_str(), value) == OK) {
        groupname = value.c_str();
        return OK;
    }

    // 查数据库
    acl::string sql;
    sql.format("SELECT group_name FROM t_router WHERE userid='%s';",
        userid);

    if (mysql_query(m_mysql, sql.c_str())) {
        logger_error("query database fail: %s, sql: %s",
            mysql_error(m_mysql), sql.c_str());

        return ERROR;
    }
    // 获取结果
    MYSQL_RES* res = mysql_store_result(m_mysql);

    if (!res) {
        logger_error("result is null: %s, sql: %s",
            mysql_error(m_mysql), sql.c_str());

        return ERROR;
    }

    MYSQL_ROW row = mysql_fetch_row(res);

    if (!row) {
        logger_warn("result is empty: %s, sql: %s",
            mysql_error(m_mysql), sql.c_str());
    } else {
        groupname = row[0];

        cache.set(key, groupname.c_str());
    }

    mysql_free_result(res);
    // 将结果缓存到redis
    return OK;
}

// 设置用户ID和组名的对应关系
int db_c::set(const char* appid, const char* userid,
    const char* groupname) const {
    
    acl::string sql;
    sql.format("INSERT INTO t_router SET "
        "appid='%s', userid='%s', group_name='%s';",
        appid, userid, groupname);
    
    if (mysql_query(m_mysql, sql.c_str())) {
        logger_error("insert database fail: %s, sql: %s",
            mysql_error(m_mysql), sql.c_str());

        return ERROR;
    }

    MYSQL_RES* res = mysql_store_result(m_mysql);

    if (!res && mysql_field_count(m_mysql)) {
        logger_error("insert database fail: %s, sql: %s",
            mysql_error(m_mysql), sql.c_str());

        return ERROR;
    }

    mysql_free_result(res);
    return OK;
}

int db_c::get(std::vector<std::string>& groupnames) const {
    // 查数据库
    acl::string sql = "SELECT group_name FROM t_groups_info;";

    if (mysql_query(m_mysql, sql.c_str())) {
        logger_error("query database fail: %s, sql: %s",
            mysql_error(m_mysql), sql.c_str());

        return ERROR;
    }
    // 获取结果
    MYSQL_RES* res = mysql_store_result(m_mysql);

    if (!res) {
        logger_error("result is null: %s, sql: %s",
            mysql_error(m_mysql), sql.c_str());

        return ERROR;
    }

    MYSQL_ROW row = nullptr;

    while ((row = mysql_fetch_row(res)) != nullptr) {
        groupnames.push_back(row[0]);
    }
    
    mysql_free_result(res);
    return OK;
}

