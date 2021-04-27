#ifndef SERVER_LOGGING_H
#define SERVER_LOGGING_H

#include <iostream>
#include <string>
#include <fstream>
#include <mutex>

class ServerLogging {
public:

	ServerLogging();
	ServerLogging(int level);
	~ServerLogging();

	void SetLoggingLevel(int level);

	inline int LevelOff();
	inline int LevelFatal();
	inline int LevelError();
	inline int LevelWarn();
	inline int LevelInfo();
	inline int LevelDebug();
	inline int LevelTrace();
	inline int LevelAll();

	void Warn(std::string& str);
	void Error(std::string& str);
	void Fatal(std::string& str);
	void Info(std::string& str);
	void Debug(std::string& str);
	void Trace(std::string& str);

private:

	int m_logger_level;
	
	std::mutex m_info_mutex;
	std::mutex m_warn_mutex;
	std::mutex m_debug_mutex;
	std::mutex m_fatal_mutex;
	std::mutex m_error_mutex;
	std::mutex m_trace_mutex;

	std::ofstream m_logging_file_info;
	std::ofstream m_logging_file_warn;
	std::ofstream m_logging_file_debug;
	std::ofstream m_logging_file_fatal;
	std::ofstream m_logging_file_error;
	std::ofstream m_logging_file_trace;
};



#endif // !SERVER_LOGGING_H