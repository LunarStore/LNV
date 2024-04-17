#include "tracker/cache.hpp"
#include "tracker/globals.hpp"

// 根据建获取值
int cache_c::get(const char* key, acl::string& value) const {
    // 构造键
    acl::string tracker_key;
    tracker_key.format("%s:%s", TRACKER_REDIS_PREFIX, key);

    if (!g_rconns) {
        logger_warn("redis connection pool is null, key: %s",
            tracker_key.c_str());

        return ERROR;
    }

    // 从连接池中获取一个Redis连接
    acl::redis_client* rconn = (acl::redis_client*) g_rconns->peek();

    if (!rconn) {
        logger_warn("peek redis connection fail, key: %s", 
            tracker_key.c_str());

        return ERROR;
    }
    acl::redis redis;

    // 将连接设置给redis
    redis.set_client(rconn);

    if (!redis.get(tracker_key.c_str(), value)) {
        logger_warn("get cache fail, key: %s", 
            tracker_key.c_str());
        g_rconns->put(rconn, false);

        return ERROR;
    }
    // 空置
    if (value.empty()) {
        logger_warn("value is empty, key: %s", 
            tracker_key.c_str());
        g_rconns->put(rconn, false);

        return ERROR;
    }

    logger("get cache ok, key: %s, value: %s",
        tracker_key.c_str(),
        value.c_str());

    g_rconns->put(rconn, true);

    return OK;
}

// 设置建的值
int cache_c::set(const char* key, const char* value, int timeout /*= -1*/) const {
    // 构造键
    acl::string tracker_key;
    tracker_key.format("%s:%s", TRACKER_REDIS_PREFIX, key);

    if (!g_rconns) {
        logger_warn("redis connection pool is null, key: %s",
            tracker_key.c_str());

        return ERROR;
    }

    // 从连接池中获取一个Redis连接
    acl::redis_client* rconn = (acl::redis_client*) g_rconns->peek();

    if (!rconn) {
        logger_warn("peek redis connection fail, key: %s", 
            tracker_key.c_str());

        return ERROR;
    }
    acl::redis redis;

    // 将连接设置给redis
    redis.set_client(rconn);

    if (timeout < 0) {
        timeout = cfg_rtimeout;
    }

    if (!redis.setex(tracker_key.c_str(), value, timeout)) {
        logger_warn("set cache fail, key: %s, value: %s, timeout: %d", 
            tracker_key.c_str(),
            value,
            timeout);
        g_rconns->put(rconn, false);

        return ERROR;
    }

    logger("set cache ok, key: %s, value: %s, timeout: %d",
        tracker_key.c_str(),
        value,
        timeout);

    g_rconns->put(rconn, true);
    return OK;
}

// 删除指定键值对
int cache_c::del(const char* key) const {
    // 构造键
    acl::string tracker_key;
    tracker_key.format("%s:%s", TRACKER_REDIS_PREFIX, key);

    if (!g_rconns) {
        logger_warn("redis connection pool is null, key: %s",
            tracker_key.c_str());

        return ERROR;
    }

    // 从连接池中获取一个Redis连接
    acl::redis_client* rconn = (acl::redis_client*) g_rconns->peek();

    if (!rconn) {
        logger_warn("peek redis connection fail, key: %s", 
            tracker_key.c_str());

        return ERROR;
    }
    acl::redis redis;

    // 将连接设置给redis
    redis.set_client(rconn);

    if (!redis.del_one(tracker_key.c_str())) {
        logger_warn("del cache fail, key: %s", 
            tracker_key.c_str());
        g_rconns->put(rconn, false);

        return ERROR;
    }

    logger("del cache ok, key: %s",
        tracker_key.c_str());

    g_rconns->put(rconn, true);
    return OK;
}
