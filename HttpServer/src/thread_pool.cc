#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <queue>

#include <WinSock2.h>

#include "thread_pool.h"
#include "http_server.h"



ThreadPool::ThreadPool(int num_threads, std::function<void(HttpServer*)> thread_function, HttpServer* http_server_ptr)
	: m_shutdown(false)
{
	for (int i = 0; i < num_threads; i++) {
		m_workers.push_back(std::thread(thread_function, http_server_ptr));
	}
}



//wake up all threads and shutdown
ThreadPool::~ThreadPool() {
	m_shutdown = true;

	//there is no clear function for queue
	{
		std::scoped_lock lock(m_worker_queue_mutex);
		m_worker_queue = std::queue<SOCKET>();
	}

	m_workers_condition_variable.notify_all();

	for (int i = 0; i < m_workers.size(); i++) {
		if (m_workers[i].joinable()) {
			m_workers[i].join();
		}
	}
}