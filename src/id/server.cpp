// ID服务器
// 实现服务器类
//
#include <unistd.h>
#include "common/proto.hpp"
#include "common/util.hpp"
#include "id/globals.hpp"
#include "id/service.hpp"
#include "id/server.hpp"

// 进程启动时被调用
void server_c::proc_on_init(void) {
	// MySQL地址表
	if (!cfg_maddrs || !strlen(cfg_maddrs))
		logger_fatal("mysql addresses is null");
	split(cfg_maddrs, g_maddrs);
	if (g_maddrs.empty())
		logger_fatal("mysql addresses is empty");

	// 主机名
	char hostname[256+1] = {};
	if (gethostname(hostname, sizeof(hostname) - 1))
		logger_error("call gethostname fail: %s", strerror(errno));
	g_hostname = hostname;

	// 最大偏移不能太小
	if (cfg_maxoffset < 10)
		logger_fatal("invalid maximum offset: %d < 10", cfg_maxoffset);

	// 打印配置信息
	logger("cfg_maddrs: %s, cfg_mtimeout: %d, cfg_maxoffset: %d",
		cfg_maddrs, cfg_mtimeout, cfg_maxoffset);
}

// 进程意图退出时被调用
// 返回true，进程立即退出，否则
// 若配置项ioctl_quick_abort非0，进程立即退出，否则
// 待所有客户机连接都关闭后，进程再退出
bool server_c::proc_exit_timer(size_t nclients, size_t nthreads) {
	if (!nclients || !nthreads) {
		logger("nclients: %lu, nthreads: %lu", nclients, nthreads);
		return true;
	}

	return false;
}

// 线程获得连接时被调用
bool server_c::thread_on_accept(acl::socket_stream* conn) {
	logger("connect, from: %s", conn->get_peer());
	return true;
}

// 与线程绑定的连接可读时被调用
bool server_c::thread_on_read(acl::socket_stream* conn) {
	// 接收包头
	char head[HEADLEN];
	if (conn->read(head, HEADLEN) < 0) {
		if (conn->eof())
			logger("connection has been closed, from: %s",
				conn->get_peer());
		else
			logger_error("read fail: %s, from: %s",
				acl::last_serror(), conn->get_peer());
		return false;
	}

	// 业务处理
	service_c service;
	return service.business(conn, head);
}

// 与线程绑定的连接超时时调用
bool server_c::thread_on_timeout(acl::socket_stream* conn) {
	logger("read timeout, from: %s", conn->get_peer());
	return true;
}

// 以上三个函数返回true，连接将继续保持，否则连接即被关闭

// 与线程绑定的连接即将关闭时被调用
void server_c::thread_on_close(acl::socket_stream* conn) {
	logger("client disconnect, from: %s", conn->get_peer());
}
