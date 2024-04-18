#include "tracker/service.hpp"
#include "common/proto.hpp"
#include "common/util.hpp"
#include "tracker/globals.hpp"
#include "tracker/db.hpp"

#include <algorithm>
#include <assert.h>
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
    long long expected = sizeof(storage_join_body_t);
    if (bodylen != expected) {
        error(conn, -1, "invalid body length: %lld != %lld",
            bodylen, expected);
        return false;
    }

    char body[bodylen];
    if (conn->read(body, bodylen) < 0) {
        logger_error("read fail: %s, bodylen: %lld, from: %s",
            acl::last_serror(), bodylen, conn->get_peer());
        return false;
    }

    // 解析包体
    storage_join_t sj;
    storage_join_body_t* sjb = (storage_join_body_t*) body;

    strcpy(sj.sj_version, sjb->sjb_version);
    strcpy(sj.sj_groupname, sjb->sjb_groupname);

    if (valid(sj.sj_groupname) != OK) {
        error(conn, -1, "invalid groupname: %s", sj.sj_groupname);
        return false;
    }

    strcpy(sj.sj_hostname, sjb->sjb_hostname);

    sj.sj_port = ntos(sjb->sjb_port);
    if (!sj.sj_port) {
        error(conn, -1, "invalid port: %u", sj.sj_port);
        return false;
    }

    sj.sj_stime = ntol(sjb->sjb_stime);
    sj.sj_jtime = ntol(sjb->sjb_jtime);

    logger("storage join version: %s, groupname: %s,"
        "hostname: %s, port: %u, stime: %s, jtime: %s",
        sj.sj_version, sj.sj_groupname,
        sj.sj_hostname, sj.sj_port,
        std::string(ctime(&sj.sj_stime)).c_str(), 
        std::string(ctime(&sj.sj_jtime)).c_str());

    if (join(&sj, conn->get_peer()) != OK) {
        error(conn, -1, "join into groups fail");
        return false;
    }
    return ok(conn);
}

// 处理来之存储服务器的心跳包
bool service_c::beat(acl::socket_stream* conn, long long bodylen) const {
    long long expected = sizeof(storage_beat_body);
    if (bodylen != expected) {
        error(conn, -1, "invalid body length: %lld != %lld",
            bodylen, expected);
        return false;
    }

    char body[bodylen];
    if (conn->read(body, bodylen) < 0) {
        logger_error("read fail: %s, bodylen: %lld, from: %s",
            acl::last_serror(), bodylen, conn->get_peer());
        return false;
    }

    // 解析包体
    // storage_join_t sj;
    storage_beat_body* sbb = (storage_beat_body*) body;

    char groupname[STORAGE_GROUPNAME_MAX + 1];
    strcpy(groupname, sbb->sbb_groupname);


    if (valid(groupname) != OK) {
        error(conn, -1, "invalid groupname: %s", groupname);
        return false;
    }

    char hostname[STORAGE_HOSTNAME_MAX + 1];
    strcpy(hostname, sbb->sbb_hostname);


    logger("storage beat, groupname: %s,"
        "hostname: %s",
        groupname, hostname);

    if (beat(groupname, hostname, conn->get_peer()) != OK) {
        error(conn, -1, "mark storage as active fail");
        return false;
    }
    return ok(conn);
}

// 处理来自客户端的获取存储服务器地址列表请求。
bool service_c::saddrs(acl::socket_stream* conn, long long bodylen) const {
    long long expected = APPID_SIZE + USERID_SIZE + FILEID_SIZE;
    if (bodylen != expected) {
        error(conn, -1, "invalid body length: %lld != %lld",
            bodylen, expected);
        return false;
    }

    char body[bodylen];
    if (conn->read(body, bodylen) < 0) {
        logger_error("read fail: %s, bodylen: %lld, from: %s",
            acl::last_serror(), bodylen, conn->get_peer());
        return false;
    }

    char appid[APPID_SIZE];
    memcpy(appid, body, APPID_SIZE);

    char userid[USERID_SIZE];
    memcpy(userid, body + APPID_SIZE, USERID_SIZE);

    char fileid[FILEID_SIZE];
    memcpy(fileid, body + APPID_SIZE + USERID_SIZE, FILEID_SIZE);

    logger("storage get saddrs, appid: %s,"
        "userid: %s, fileid: %s",
        appid, userid, fileid);
    if (saddrs(conn, appid, userid) != OK) {
        error(conn, -1, "get saddrs fail");
        return false;
    }

    return ok(conn);
}

// 处理来自客户机的获取组列表请求
bool service_c::groups(acl::socket_stream* conn) const {
    if ((errno = pthread_mutex_lock(&g_mutex))) {
        logger_error("call pthread_mutex_lock fail: %s",
            strerror(errno));
        return false;
    }

    acl::string gps;

    gps.format("COUNT OF GROUPS: %lu\n", g_groups.size());

    for (std::map<std::string, std::list<storage_info_t> >::const_iterator it_gp = g_groups.begin();
        it_gp != g_groups.end(); it_gp++) {
        acl::string gp;
        int active_count = 0;
        gp.format("GROUPNAME: %s\n"
        "COUNT OF STORAGES: %lu\n"
        "COUNT OF ACTIVE STORAGES: %s\n",
        it_gp->first.c_str(), it_gp->second.size(), "%d");

        for (std::list<storage_info_t>::const_iterator it_si = it_gp->second.begin();
            it_si != it_gp->second.end(); it_si++) {
            
            acl::string si, status;
            si.format("VERSION: %s\n"
            "HOSTNAME: %s\n"
            "ADDRESS: %s:%u\n"
            "STARTUP TIME: %s"
            "JOIN TIME: %s"
            "BEAT TIME: %s"
            "STATUS: ",
            it_si->si_version,
            it_si->si_hostname,
            it_si->si_addr, it_si->si_port,
            std::string(ctime(&it_si->si_stime)).c_str(),
            std::string(ctime(&it_si->si_jtime)).c_str(),
            std::string(ctime(&it_si->si_btime)).c_str());

            switch(it_si->si_status) {
                case STORAGE_STATUS_ACTIVE:
                    status = "ACTIVE";
                    active_count++;
                    break;
                case STORAGE_STATUS_OFFLINE:
                    status = "OFFLINE";
                    break;
                case STORAGE_STATUS_ONLINE:
                    status = "ONLINE";
                    break;
                default:
                    status = "UNKNOW";
            }

            si += status + "\n";

            gp += si;
        }
        gp.format(gp.c_str(), active_count);
        gps += gp;
    }

    gps = gps.left(gps.size() - 1);

    if ((errno = pthread_mutex_unlock(&g_mutex))) {
        logger_error("call pthread_mutex_unlock fail: %s",
            strerror(errno));
        return false;
    }

    long long bodylen = gps.size() + 1;
    long long resplen = HEADLEN + bodylen;
    char resp[resplen] = {};
    llton(bodylen, resp);
    resp[BODYLEN_SIZE] = CMD_TRACKER_REPLY;
    resp[BODYLEN_SIZE + COMMAND_SIZE] = 0;
    strcpy(resp + HEADLEN, gps.c_str());

    if (conn->write(resp, resplen) < 0) {
		logger_error("write fail: %s, resplen: %lld, to: %s",
			acl::last_serror(), resplen, conn->get_peer());
		return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////

int service_c::join(const storage_join_t* sj, const char* saddr) const {
    if ((errno = pthread_mutex_lock(&g_mutex))) {
        logger_error("call pthread_mutex_lock fail: %s",
            strerror(errno));
        return ERROR;
    }

    std::map<std::string, std::list<storage_info_t> >::iterator it_gp = 
        g_groups.find(sj->sj_groupname);
    
    if (it_gp != g_groups.end()) {
        
        std::list<storage_info_t>::iterator it_si;
        for ( it_si = it_gp->second.begin();
            it_si != it_gp->second.end();
            it_si++) {
            
            if (!strcmp(it_si->si_hostname, sj->sj_hostname) &&
                !strcmp(it_si->si_addr, saddr)) {
                strcpy(it_si->si_version, sj->sj_version);
                it_si->si_port = sj->sj_port;
                it_si->si_stime = sj->sj_stime;
                it_si->si_jtime = sj->sj_jtime;
                it_si->si_btime = sj->sj_jtime;
                it_si->si_status = STORAGE_STATUS_ACTIVE;

                break;
            }

        }
        // 不在列表

        if (it_si == it_gp->second.end()) {
            storage_info_t si;
            strcpy(si.si_version, sj->sj_version);
            strcpy(si.si_hostname, sj->sj_hostname);
            strcpy(si.si_addr, saddr);
            si.si_port = sj->sj_port;
            si.si_stime = sj->sj_stime;
            si.si_jtime = sj->sj_jtime;
            si.si_btime = sj->sj_jtime;
            si.si_status = STORAGE_STATUS_ACTIVE;

            it_gp->second.push_back(si);
        }

    } else {
		// 将待加入存储服务器所隶属的组加入组表
        auto res = g_groups.insert(std::make_pair(sj->sj_groupname, std::list<storage_info_t>()));

        assert(res.second);

        it_gp = res.first;
		// 将待加入存储服务器加入该组的存储服务器列表
		storage_info_t si;
		strcpy(si.si_version,  sj->sj_version);  // 版本
		strcpy(si.si_hostname, sj->sj_hostname); // 主机名
		strcpy(si.si_addr,     saddr);           // IP地址
		si.si_port   = sj->sj_port;              // 端口号
		si.si_stime  = sj->sj_stime;             // 启动时间
		si.si_jtime  = sj->sj_jtime;             // 加入时间
		si.si_btime  = sj->sj_jtime;             // 心跳时间
		si.si_status = STORAGE_STATUS_ONLINE;    // 状态
		it_gp->second.push_back(si);
    }


    if ((errno = pthread_mutex_unlock(&g_mutex))) {
        logger_error("call pthread_mutex_unlock fail: %s",
            strerror(errno));
        return ERROR;
    }
    return OK;
}

// 将存储服务器标为活动
int service_c::beat(const char* groupname, const char* hostname,
    const char* saddr) const {
    if ((errno = pthread_mutex_lock(&g_mutex))) {
        logger_error("call pthread_mutex_lock fail: %s",
            strerror(errno));
        return ERROR;
    }

    int rt = OK;

    std::map<std::string, std::list<storage_info_t> >::iterator it_gp = 
        g_groups.find(groupname);
    
    if (it_gp != g_groups.end()) {
        
        std::list<storage_info_t>::iterator it_si;
        for ( it_si = it_gp->second.begin();
            it_si != it_gp->second.end();
            it_si++) {
            
            if (!strcmp(it_si->si_hostname, groupname) &&
                !strcmp(it_si->si_addr, saddr)) {
                it_si->si_btime = time(nullptr);
                it_si->si_status = STORAGE_STATUS_ACTIVE;

                break;
            }

        }
        // 不在列表

        if (it_si == it_gp->second.end()) {
            logger_error("strage not found, groupname: %s, "
                "hostname: %s, saddr: %s", groupname, hostname, saddr);
            rt = ERROR;
        }

    } else {
		logger_error("group not found, groupname: %s", groupname);
		rt = ERROR;
    }


    if ((errno = pthread_mutex_unlock(&g_mutex))) {
        logger_error("call pthread_mutex_unlock fail: %s",
            strerror(errno));
        return ERROR;
    }
    return rt;
}

// 响应客户机存储服务器地址列表
int service_c::saddrs(acl::socket_stream* conn, 
    const char* appid, const char* userid) const {
	// 应用ID是否合法
	if (valid(appid) != OK) {
		error(conn, -1, "invalid appid: %s", appid);
		return ERROR;
	}

	// 应用ID是否存在
	if (std::find(g_appids.begin(), g_appids.end(),
		appid) == g_appids.end()) {
		error(conn, -1, "unknown appid: %s", appid);
		return ERROR;
	}

	// 根据用户ID获取其对应的组名
	std::string groupname;
	if (group_of_user(appid, userid, groupname) != OK) {
		error(conn, -1, "get groupname fail");
		return ERROR;
	}

	// 根据组名获取存储服务器地址列表
	std::string saddrs;
	if (saddrs_of_group(groupname.c_str(), saddrs) != OK) {
		error(conn, -1, "get storage address fail");
		return ERROR;
	}

	logger("appid: %s, userid: %s, groupname: %s, saddrs: %s",
		appid, userid, groupname.c_str(), saddrs.c_str());

	// |包体长度|命令|状态|组名|存储服务器地址列表|
	// |    8   |  1 |  1 |       包体长度        |
	// 构造响应
	long long bodylen = STORAGE_GROUPNAME_MAX + 1 + saddrs.size() + 1;
	long long resplen = HEADLEN + bodylen;
	char resp[resplen] = {};
	llton(bodylen, resp);
	resp[BODYLEN_SIZE] = CMD_TRACKER_REPLY;
	resp[BODYLEN_SIZE+COMMAND_SIZE] = 0;
	strncpy(resp + HEADLEN, groupname.c_str(), STORAGE_GROUPNAME_MAX);
	strcpy(resp + HEADLEN + STORAGE_GROUPNAME_MAX + 1, saddrs.c_str());

	// 发送响应
	if (conn->write(resp, resplen) < 0) {
		logger_error("write fail: %s, resplen: %lld, to: %s",
			acl::last_serror(), resplen, conn->get_peer());
		return ERROR;
	}

	return OK;
}

// 根据用户ID获取其对应的组名
int service_c::group_of_user(const char* appid,
    const char* userid, std::string& groupname) const {
    db_c db;
    if (db.connect() != OK) {
        return ERROR;
    }

    if (db.get(userid, groupname) != OK) {
        // 出错
        return ERROR;
    }

    if (groupname.empty()) {
        logger("groupname is empty, appid: %s, userid: %s, allocate one",
            appid, userid);
        
        std::vector<std::string> gps;
        if (db.get(gps) != OK) {
            return ERROR;
        }
        if (gps.empty()) {
            logger_error("groupnames is empty, appid: %s, userid: %s",
                appid, userid);
            return ERROR;
        }

        srand(time(nullptr));
        groupname = gps[rand() % gps.size()];

        if (db.set(appid, userid, groupname.c_str()) != OK) {
            return ERROR;
        }
    }

    return OK;
}

// 根据组名获取存储服务器地址列表
int service_c::saddrs_of_group(const char* groupname,
    std::string& saddrs) const {

	// 互斥锁加锁
	if ((errno = pthread_mutex_lock(&g_mutex))) {
		logger_error("call pthread_mutex_lock fail: %s",
			strerror(errno));
		return ERROR;
	}

	int rt = OK;

	// 根据组名在组表中查找特定组
	std::map<std::string, std::list<storage_info_t> >::iterator
		it_gp = g_groups.find(groupname);
	if (it_gp != g_groups.end()) { // 若找到该组
		if (!it_gp->second.empty()) { // 若该组的存储服务器列表非空
			// 在该组的存储服务器列表中，从随机位置开
			// 始最多抽取三台处于活动状态的存储服务器
			srand(time(NULL));
			int nsis = it_gp->second.size();
			int nrand = rand() % nsis;
			std::list<storage_info_t>::const_iterator it_si =
				it_gp->second.begin();
			int nacts = 0;
			for (int i = 0; i < nsis + nrand; ++i, ++it_si) {
				if (it_si == it_gp->second.end())
					it_si = it_gp->second.begin();

				logger("i: %d, nrand: %d, addr: %s, port: %u, "
					"status: %d", i, nrand, it_si->si_addr, it_si->si_port,
					it_si->si_status);

				if (i >= nrand && it_si->si_status ==
					STORAGE_STATUS_ACTIVE) {
					char saddr[256];
					sprintf(saddr, "%s:%d", it_si->si_addr, it_si->si_port);
					saddrs += saddr;
					saddrs += ";";
					if (++nacts >= 3)
						break;
				}
			}
			if (!nacts) { // 若没有处于活动状态的存储服务器
				logger_error("no active storage in group %s",
					groupname);
				rt = ERROR;
			}
		}
		else { // 若该组的存储服务器列表为空
			logger_error("no storage in group %s", groupname);
			rt = ERROR;
		}
	}
	else { // 若没有该组
		logger_error("not found group %s", groupname);
		rt = ERROR;
	}

	// 互斥锁解锁
	if ((errno = pthread_mutex_unlock(&g_mutex))) {
		logger_error("call pthread_mutex_unlock fail: %s",
			strerror(errno));
		return ERROR;
	}

	return rt;
}

////////////////////////////////////////////////////////////////////////
    // 应答成功
bool service_c::ok(acl::socket_stream* conn) const {
	// |包体长度|命令|状态|
	// |    8   |  1 |  1 |
	// 构造响应
	long long bodylen = 0;
	long long resplen = HEADLEN + bodylen;
	char resp[resplen] = {};
	llton(bodylen, resp);
	resp[BODYLEN_SIZE] = CMD_TRACKER_REPLY;
	resp[BODYLEN_SIZE+COMMAND_SIZE] = 0;

	// 发送响应
	if (conn->write(resp, resplen) < 0) {
		logger_error("write fail: %s, resplen: %lld, to: %s",
			acl::last_serror(), resplen, conn->get_peer());
		return false;
	}

	return true;
}
// 错误应答
bool service_c::error(acl::socket_stream* conn, short errnumb,
    const char* format, ...) const {
	// 错误描述
	char errdesc[ERROR_DESC_SIZE];
	va_list ap;
	va_start(ap, format);
	vsnprintf(errdesc, ERROR_DESC_SIZE, format, ap);
	va_end(ap);
	logger_error("%s", errdesc);
	acl::string desc;
	desc.format("[%s] %s", g_hostname.c_str(), errdesc);
	memset(errdesc, 0, sizeof(errdesc));
	strncpy(errdesc, desc.c_str(), ERROR_DESC_SIZE - 1);
	size_t desclen = strlen(errdesc);
	desclen += desclen != 0;

	// |包体长度|命令|状态|错误号|错误描述|
	// |    8   |  1 |  1 |   2  | <=1024 |
	// 构造响应
	long long bodylen = ERROR_NUMB_SIZE + desclen;
	long long resplen = HEADLEN + bodylen;
	char resp[resplen] = {};
	llton(bodylen, resp);
	resp[BODYLEN_SIZE] = CMD_TRACKER_REPLY;
	resp[BODYLEN_SIZE+COMMAND_SIZE] = STATUS_ERROR;
	ston(errnumb, resp + HEADLEN);
	if (desclen)
		strcpy(resp + HEADLEN + ERROR_NUMB_SIZE, errdesc);

	// 发送响应
	if (conn->write(resp, resplen) < 0) {
		logger_error("write fail: %s, resplen: %lld, to: %s",
			acl::last_serror(), resplen, conn->get_peer());
		return false;
	}

	return true;
}