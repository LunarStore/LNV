#ifndef __TRACKER_SERVER_HPP__

#define __TRACKER_SERVER_HPP__
#include <lib_acl.hpp>
#include "tracker/status.hpp"

class server_c: public acl::master_threads {
protected:
    void proc_on_init(void) override;
    bool proc_exit_timer(size_t nclients, size_t threads) override;
    void proc_on_exit(void) override;

	/**
	 * 纯虚函数：当某个客户端连接有数据可读或关闭或出错时调用此函数
	 * @param stream {socket_stream*}
	 * @return {bool} 返回 false 则表示当函数返回后需要关闭连接，
	 *  否则表示需要保持长连接，如果该流出错，则应用应该返回 false
	 */
	virtual bool thread_on_read(acl::socket_stream* stream) override;


	/**
	 * 当线程池中的某个线程获得一个连接时的回调函数，子类可以做一些
	 * 初始化工作，该函数是在主线程的线程空间中运行
	 * @param stream {socket_stream*}
	 * @return {bool} 如果返回 false 则表示子类要求关闭连接，而不
	 *  必将该连接再传递至 thread_main 过程
	 */
	virtual bool thread_on_accept(acl::socket_stream* stream) override;

	/**
	 * 当某个网络连接的 IO 读写超时时的回调函数，如果该函数返回 true 则
	 * 表示继续等待下一次读写，否则则希望关闭该连接
	 * @param stream {socket_stream*}
	 * @return {bool} 如果返回 false 则表示子类要求关闭连接，否则则要求
	 *  继续监听该连接
	 */
	virtual bool thread_on_timeout(acl::socket_stream* stream) override;

	/**
	 * 当与某个线程绑定的连接关闭时的回调函数
	 * @param stream {socket_stream*}
	 * 注：当在 thread_on_accept 返回 false 后流关闭时该函数并不会
	 * 被调用
	 */
	virtual void thread_on_close(acl::socket_stream* stream) override;
private:
    status_c* m_status; // 存储服务器状态检查线程
};

#endif