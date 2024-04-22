#ifndef __STORAGE_ID_HPP__

#define __STORAGE_ID_HPP__

class id_c {
public:
	// 从ID服务器获取与ID键向对应的值
	long get(char const* key) const;

private:
	// 向ID服务器发送请求，接收并解析响应，从中获得ID值
	long client(char const* requ, long long requlen) const;
};

#endif
