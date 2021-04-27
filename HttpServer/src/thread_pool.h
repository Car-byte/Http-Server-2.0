#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <iostream>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>

#include <WinSock2.h>

#include "http_server.h"

class ThreadPool {
public:

	friend class HttpServer;

	ThreadPool() = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool(const ThreadPool&) = delete;

	ThreadPool(int num_threads, std::function<void(HttpServer*)> thread_function, HttpServer* http_server_ptr);

	~ThreadPool();

private:

	std::vector<std::thread> m_workers;
	std::queue<SOCKET> m_worker_queue;

	std::condition_variable_any m_workers_condition_variable;
	std::mutex m_worker_queue_mutex;

	bool m_shutdown;
};



#endif // !THREAD_POOL_H