#ifndef __STORAGE_DB_HPP__
#define __STORAGE_DB_HPP__
#include <string>
#include <mysql.h>


class db_c {
public:
    db_c();
    ~db_c();

    int connect();

    int get(const char* appid, const char* userid, const char* fileid,
        std::string& filepath, long long* filesize) const;

    int set(const char* appid, const char* userid, const char* fileid,
        const char* filepath, long long filesize) const;

    int del(const char* appid, const char* userid, const char* fileid) const;

private:
    std::string table_of_user(const char* userid) const;

	unsigned int hash(const char* buf, size_t len) const;
private:

	MYSQL* m_mysql; // MySQL对象
};

#endif