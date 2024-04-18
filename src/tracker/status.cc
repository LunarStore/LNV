// 跟踪服务器
// 实现存储服务器状态检查线程类
//
#include <unistd.h>
#include "tracker/globals.hpp"
#include "tracker/status.hpp"

// 构造函数
status_c::status_c(void): m_stop(false) {
}

// 终止线程
void status_c::stop(void) {
	m_stop = true;
}

// 线程过程
void* status_c::run(void) {
	for (time_t last = time(NULL); !m_stop; sleep(1)) {
		time_t now = time(NULL); // 现在

		// 若现在距离最近一次检查存储服务器状态已足够久
		if (now - last >= cfg_interval) {
			check(); // 检查存储服务器状态
			last = now; // 更新最近一次检查时间
		}
	}

	return NULL;
}

// 检查存储服务器状态
int status_c::check(void) const {
	// 现在

	// 互斥锁加锁

	// 遍历组表中的每一个组

		// 遍历该组中的每一台存储服务器

			// 若该存储服务器心跳停止太久

				// 则将其状态标记为离线

	// 互斥锁解锁

	return OK;
}