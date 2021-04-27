#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>

#include "server_logging.h"


ServerLogging::ServerLogging()
	: m_logger_level(LevelWarn())
{
	std::time_t current_time_local = std::time(0);
	struct tm current_time_utc;
	int results = gmtime_s(&current_time_utc, &current_time_local);
	if (results != 0) {
		std::cout << "Error getting time unable to create server logs\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_debug.open("debug-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_debug.fail()) {
		std::cout << "Unable to create or open debug logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_error.open("error-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_error.fail()) {
		std::cout << "Unable to create or open error logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_fatal.open("fatal-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_fatal.fail()) {
		std::cout << "Unable to create or open fatal logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_info.open("info-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_info.fail()) {
		std::cout << "Unable to create or open info logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_trace.open("trace-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_trace.fail()) {
		std::cout << "Unable to create or open trace logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_warn.open("warn-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_warn.fail()) {
		std::cout << "Unable to create or open warn logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_debug << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_debug << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';

	m_logging_file_error << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_error << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';

	m_logging_file_fatal << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_fatal << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';

	m_logging_file_info << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_info << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';

	m_logging_file_trace << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_trace << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';

	m_logging_file_warn << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_warn << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';
}



ServerLogging::ServerLogging(int level) 
	: m_logger_level(level)
{
	std::time_t current_time_local = std::time(0);
	struct tm current_time_utc;
	int results = gmtime_s(&current_time_utc, &current_time_local);
	if (results != 0) {
		std::cout << "Error getting time unable to create server logs\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_debug.open("debug-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_debug.fail()) {
		std::cout << "Unable to create or open debug logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_error.open("error-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_error.fail()) {
		std::cout << "Unable to create or open error logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_fatal.open("fatal-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_fatal.fail()) {
		std::cout << "Unable to create or open fatal logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_info.open("info-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_info.fail()) {
		std::cout << "Unable to create or open info logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_trace.open("trace-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_trace.fail()) {
		std::cout << "Unable to create or open trace logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_warn.open("warn-" +
		std::to_string(current_time_utc.tm_year + 1900) + "-" +
		std::to_string(current_time_utc.tm_mon + 1) + "-" +
		std::to_string(current_time_utc.tm_mday) + ".log",
		std::ios::out | std::ios::app);
	if (m_logging_file_warn.fail()) {
		std::cout << "Unable to create or open warn logging file\n";
		exit(EXIT_FAILURE);
	}

	m_logging_file_debug << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_debug << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';

	m_logging_file_error << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_error << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';

	m_logging_file_fatal << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_fatal << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';

	m_logging_file_info << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_info << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';

	m_logging_file_trace << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_trace << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';

	m_logging_file_warn << "[ " << current_time_utc.tm_hour << ' ' << current_time_utc.tm_min << ' ';
	m_logging_file_warn << current_time_utc.tm_sec << " ] " << "Server Started" << '\n';
}



ServerLogging::~ServerLogging() {
	m_logging_file_trace.flush();
	m_logging_file_debug.flush();
	m_logging_file_error.flush();
	m_logging_file_fatal.flush();
	m_logging_file_info.flush();
	m_logging_file_warn.flush();
}



void ServerLogging::SetLoggingLevel(int level) {
	m_logger_level = level;
}



void ServerLogging::Debug(std::string& str) {
	//if the logging level is set so the debug output is disabled
	if (LevelDebug() > m_logger_level) {
		return;
	}

	std::time_t current_time_local = std::time(0);
	struct tm time_utc;
	int results = gmtime_s(&time_utc, &current_time_local);
	if (results != 0) {
		std::scoped_lock(m_debug_mutex);
		m_logging_file_debug << "[ Time Error ] " << str << "\n";
	}
	else {
		std::scoped_lock(m_debug_mutex);
		m_logging_file_debug << "[ " << time_utc.tm_hour << ' ' << time_utc.tm_min << ' ';
		m_logging_file_debug << time_utc.tm_sec << " ] " << str << '\n';
	}
}



void ServerLogging::Warn(std::string& str) {
	//if the logging level is set so the debug output is disabled
	if (LevelWarn() > m_logger_level) {
		return;
	}

	std::time_t current_time_local = std::time(0);
	struct tm time_utc;
	int results = gmtime_s(&time_utc, &current_time_local);
	if (results != 0) {
		std::scoped_lock(m_warn_mutex);
		m_logging_file_warn << "[ Time Error ] " << str << '\n';
	}
	else {
		std::scoped_lock(m_warn_mutex);
		m_logging_file_warn << "[ " << time_utc.tm_hour << ' ' << time_utc.tm_min << ' ';
		m_logging_file_warn << time_utc.tm_sec << " ] " << str << '\n';
	}
}



void ServerLogging::Error(std::string& str) {
	//if the logging level is set so the error output is disabled
	if (LevelError() > m_logger_level) {
		return;
	}

	std::time_t current_time_local = std::time(0);
	struct tm time_utc;
	int results = gmtime_s(&time_utc, &current_time_local);
	if (results != 0) {
		std::scoped_lock(m_error_mutex);
		m_logging_file_error << "[ Time Error ] " << str << '\n';
	}
	else {
		std::scoped_lock(m_error_mutex);
		m_logging_file_error << "[ " << time_utc.tm_hour << ' ' << time_utc.tm_min << ' ';
		m_logging_file_error << time_utc.tm_sec << " ] " << str << '\n';
	}
}



void ServerLogging::Fatal(std::string& str) {
	//if the logging level is set so the fatal output is disabled
	if (LevelFatal() > m_logger_level) {
		return;
	}

	std::time_t current_time_local = std::time(0);
	struct tm time_utc;
	int results = gmtime_s(&time_utc, &current_time_local);
	if (results != 0) {
		std::scoped_lock(m_fatal_mutex);
		m_logging_file_fatal << "[ Time Error ] " << str << '\n';
	}
	else {
		std::scoped_lock(m_fatal_mutex);
		m_logging_file_fatal << "[ " << time_utc.tm_hour << ' ' << time_utc.tm_min << ' ';
		m_logging_file_fatal << time_utc.tm_sec << " ] " << str << '\n';
	}
}



void ServerLogging::Trace(std::string& str) {
	//if the logging level is set so the trace output is disabled
	if (LevelTrace() > m_logger_level) {
		return;
	}

	std::time_t current_time_local = std::time(0);
	struct tm time_utc;
	int results = gmtime_s(&time_utc, &current_time_local);
	if (results != 0) {
		std::scoped_lock(m_trace_mutex);
		m_logging_file_trace << "[ Time Error ] " << str << '\n';
	}
	else {
		std::scoped_lock(m_trace_mutex);
		m_logging_file_trace << "[ " << time_utc.tm_hour << ' ' << time_utc.tm_min << ' ';
		m_logging_file_trace << time_utc.tm_sec << " ] " << str << '\n';
	}
}



void ServerLogging::Info(std::string& str) {
	//if the logging level is set so the info output is disabled
	if (LevelInfo() > m_logger_level) {
		return;
	}

	std::time_t current_time_local = std::time(0);
	struct tm time_utc;
	int results = gmtime_s(&time_utc, &current_time_local);
	if (results != 0) {
		std::scoped_lock(m_info_mutex);
		m_logging_file_info << "[ Time Error ] " << str << '\n';
	}
	else {
		std::scoped_lock(m_info_mutex);
		m_logging_file_info << "[ " << time_utc.tm_hour << ' ' << time_utc.tm_min << ' ';
		m_logging_file_info << time_utc.tm_sec << " ] " << str << '\n';
	}
}



inline int ServerLogging::LevelOff() {
	return 0;
}



inline int ServerLogging::LevelFatal() {
	return 1;
}



inline int ServerLogging::LevelError() {
	return 2;
}



inline int ServerLogging::LevelWarn() {
	return 3;
}



inline int ServerLogging::LevelInfo() {
	return 4;
}



inline int ServerLogging::LevelDebug() {
	return 5;
}



inline int ServerLogging::LevelTrace() {
	return 6;
}



inline int ServerLogging::LevelAll() {
	return 7;
}