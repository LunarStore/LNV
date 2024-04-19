// 跟踪服务器
// 声明存储服务器状态检查线程类
//
#ifndef __TRACKER_STATUS_HPP__
#define __TRACKER_STATUS_HPP__

#include <lib_acl.hpp>
//
// 存储服务器状态检查线程类
//
class status_c: public acl::thread {
public:
	// 构造函数
	status_c(void);

	// 终止线程
	void stop(void);

protected:
	// 线程过程
	void* run(void);

private:
	// 检查存储服务器状态
	int check(void) const;

	bool m_stop; // 是否终止
};

#endif 