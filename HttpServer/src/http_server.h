#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "thread_pool.h"
#include "server_statistics.h"
#include "http_response_components.h"
#include "server_logging.h"

class HttpServer {
public:

	HttpServer() = delete;
	HttpServer& operator=(const HttpServer&) = delete;
	HttpServer(const HttpServer&) = delete;

	HttpServer(const char* port);

	~HttpServer();

	void StartHttpServer();

	bool ServerShutingDown();

	bool ServerQueueEmpty();

private:

	//could move most of these functions to another header because lots of them
	//dont use any member data / functions

	void CopyPort(const char* port);
	void LoadWinSockDll();

	void HandleHttpRequest();

	std::string GetContentType(std::string& file_name);

	void ParseRequest(SOCKET client_request_socket);

	bool KeepAlive(const std::string_view& http_request);

	void GetClientip(SOCKET& client, std::string& client_ip);

	void SendToProperMethodFunction(std::string& response, std::string_view& request, bool keep_alive);

	void HandleGetRequest(std::string& response, std::string_view& request, bool keep_alive);
	void HandlePostRequest(std::string& response, std::string_view& request, bool keep_alive);
	void HandleHeadRequest(std::string& response, std::string_view& request, bool keep_alive);
	void HandlePutRequest(std::string& response, std::string_view& request, bool keep_alive);
	void HandleDeleteRequest(std::string& response, std::string_view& request, bool keep_alive);
	void HandleConnectRequest(std::string& response, std::string_view& request, bool keep_alive);
	void HandleOptionsRequest(std::string& response, std::string_view& request, bool keep_alive);
	void HandleTraceRequest(std::string& response, std::string_view& request, bool keep_alive);
	void HandlePatchRequest(std::string& response, std::string_view& request, bool keep_alive);
	void HandleUnknownRequest(std::string& response, std::string_view& request, bool keep_alive);

	void ConstructGetResponse(std::string& response, std::string& content_type, std::string& file_contents, bool keep_alive);

	void ConstructPostResponse(std::string& response, std::string& content_type, std::string& file_contents, bool keep_alive);

	int ReadFile(std::string& file_contents, const std::string& file_path);

	void GetMethod(std::string& method, std::string_view& http_request);

	void DecodeUri(std::string_view& uri, std::string& file_path, std::string& uri_contents);

	void SeperateUri(std::string_view& request, std::string_view& uri, int start_index);

	void DecodePostData(std::unordered_map<std::string, std::string>& decoded_data, std::string_view& uri);

	void InitUriMap();

	char m_port[5];
	WSADATA m_wsa_data;
	struct addrinfo m_hints;
	SOCKET m_listen_socket;
	
	ServerStatistics m_server_statistics;

	ServerLogging m_log;

	HttpResponseComponents m_http_header_parts;

	std::mutex m_console_mutex;
	std::mutex m_statistics_mutex;

	std::shared_ptr<ThreadPool> m_thread_pool;

	std::unordered_map<std::string, char> m_uri_map;
};



#endif //!http_server