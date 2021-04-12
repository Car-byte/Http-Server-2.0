#ifndef SERVER_STATISTICS_H
#define SERVER_STATISTICS_H

#include <iostream>
#include <stdint.h>

class ServerStatistics {
public:

	ServerStatistics();

	void AddBytesReceived(unsigned bytes);
	void AddBytesSent(unsigned bytes);

	uint64_t GetBytesSent();
	uint64_t GetBytesReceived();

private:

	uint64_t m_bytes_sent;
	uint64_t m_bytes_received;
};


#endif // !SERVER_STATISTICS_H