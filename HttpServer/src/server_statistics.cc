#include <stdint.h>

#include "server_statistics.h"


ServerStatistics::ServerStatistics() 
	: m_bytes_sent{ 0 }, m_bytes_received{0}
{
}



uint64_t ServerStatistics::GetBytesReceived(){
	return m_bytes_received;
}



uint64_t ServerStatistics::GetBytesSent() {
	return m_bytes_sent;
}



void ServerStatistics::AddBytesReceived(unsigned bytes) {
	m_bytes_received += bytes;
}



void ServerStatistics::AddBytesSent(unsigned bytes) {
	m_bytes_sent += bytes;
}