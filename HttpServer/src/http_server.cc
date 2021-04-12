#include <iostream>
#include <string>
#include <string_view>
#include <mutex>
#include <memory>
#include <functional>
#include <fstream>
#include <chrono>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "http_server.h"
#include "thread_pool.h"

#pragma comment (lib, "Ws2_32.lib")


void HttpServer::CopyPort(const char* port) {
	int i = 0;
	while (port[i] != '\0') {
		m_port[i] = port[i];
		m_port[i + 1] = '\0';
		i++;
	}
}



//load neccessary dll
void HttpServer::LoadWinSockDll() {
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa_data) != 0) {
		std::cout << "Error loading ws2_32.lib\n";
		exit(EXIT_FAILURE);
	}
}



HttpServer::HttpServer(const char* port) {

	m_thread_pool = std::make_shared<ThreadPool>(20, &HttpServer::HandleHttpRequest, this);

	CopyPort(port);

	LoadWinSockDll();

	InitUriMap();

	struct addrinfo* address_lookup = nullptr;
	m_listen_socket = INVALID_SOCKET;
	ZeroMemory(&m_hints, sizeof(m_hints));
	m_hints.ai_family = AF_INET;
	m_hints.ai_socktype = SOCK_STREAM;
	m_hints.ai_protocol = IPPROTO_TCP;
	m_hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, m_port, &m_hints, &address_lookup) != 0) {
		std::cout << "Unable to get address info\n";
		exit(EXIT_FAILURE);
	}

	m_listen_socket = socket(address_lookup->ai_family, address_lookup->ai_socktype, address_lookup->ai_protocol);

	if (m_listen_socket == INVALID_SOCKET) {
		std::cout << "Unable to create socket\n";
		exit(EXIT_FAILURE);
	}

	if (bind(m_listen_socket, address_lookup->ai_addr, static_cast<int>(address_lookup->ai_addrlen)) == SOCKET_ERROR) {
		std::cout << "Unable to bind socket\n";
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(address_lookup);
}



//https://www.urlencoder.io/learn/
//finish adding all the strings
void HttpServer::InitUriMap() {
	m_uri_map["%20"] = ' '; m_uri_map["%21"] = '!';
	m_uri_map["%22"] = '"'; m_uri_map["%23"] = '#';
	m_uri_map["%24"] = '$'; m_uri_map["%25"] = '%';
	m_uri_map["%26"] = '&'; m_uri_map["%27"] = '\'';
	m_uri_map["%28"] = '('; m_uri_map["%29"] = ')';
	m_uri_map["%2A"] = '*'; m_uri_map["%2B"] = '+';
	m_uri_map["%2C"] = ','; m_uri_map["%2D"] = '-';
	m_uri_map["%2E"] = '.'; m_uri_map["%2F"] = '/';
	m_uri_map["%30"] = '0'; m_uri_map["%31"] = '1';
	m_uri_map["%32"] = '2'; m_uri_map["%33"] = '3';
	m_uri_map["%34"] = '4'; m_uri_map["%35"] = '5';
	m_uri_map["%36"] = '6'; m_uri_map["%37"] = '7';
	m_uri_map["%38"] = '8'; m_uri_map["%39"] = '9';
	m_uri_map["%3A"] = ':'; m_uri_map["%3B"] = ';';
	m_uri_map["%3C"] = '<'; m_uri_map["%3D"] = '=';
	m_uri_map["%3E"] = '>'; m_uri_map["%3F"] = '?';
	m_uri_map["%40"] = '@'; m_uri_map["%41"] = 'A';
	m_uri_map["%42"] = 'B'; m_uri_map["%43"] = 'C';
	m_uri_map["%44"] = 'D'; m_uri_map["%45"] = 'E';
	m_uri_map["%46"] = 'F'; m_uri_map["%47"] = 'G';
	m_uri_map["%48"] = 'H'; m_uri_map["%49"] = 'I';
	m_uri_map["%4A"] = 'J'; m_uri_map["%4B"] = 'K';
	m_uri_map["%4C"] = 'L'; m_uri_map["%4D"] = 'M';
	m_uri_map["%4E"] = 'N'; m_uri_map["%4F"] = 'O';
	m_uri_map["%50"] = 'P'; m_uri_map["%51"] = 'Q';
	m_uri_map["%52"] = 'R'; m_uri_map["%53"] = 'S';
	m_uri_map["%54"] = 'T'; m_uri_map["%55"] = 'U';
	m_uri_map["%56"] = 'V'; m_uri_map["%57"] = 'W';
	m_uri_map["%58"] = 'X'; m_uri_map["%59"] = 'Y';
	m_uri_map["%5A"] = 'Z'; m_uri_map["%5B"] = '[';
	m_uri_map["%5C"] = '\\'; m_uri_map["%5D"] = ']';
	m_uri_map["%5E"] = '^'; m_uri_map["%5F"] = '_';
	m_uri_map["%60"] = '`';
}



HttpServer::~HttpServer() {
	if (shutdown(m_listen_socket, SD_BOTH) == SOCKET_ERROR) {
		std::cout << "Error shutting down socket: forcfully shutting it down\n";
		closesocket(m_listen_socket);
	}
	
	WSACleanup();
}



bool HttpServer::ServerShutingDown() {
	return m_thread_pool->m_shutdown;
}



bool HttpServer::ServerQueueEmpty() {
	return m_thread_pool->m_worker_queue.empty();
}



void HttpServer::StartHttpServer() {

	if (listen(m_listen_socket, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Failed to listen on socket\n";
		exit(EXIT_FAILURE);
	}

	std::cout << "Started server listening on port: " << m_port << "\n";

	SOCKET client_socket = INVALID_SOCKET;

	while (true) {
		client_socket = accept(m_listen_socket, NULL, NULL);
		if (client_socket != INVALID_SOCKET) {
			std::scoped_lock lock(m_thread_pool->m_worker_queue_mutex);
			m_thread_pool->m_worker_queue.push(client_socket);
			m_thread_pool->m_workers_condition_variable.notify_one();
		}
	}
}



void HttpServer::HandleHttpRequest() {

	while (!ServerShutingDown()) {

		SOCKET client_request_socket = INVALID_SOCKET;

		{
			//scoped lock is not supported for condition variable
			std::unique_lock lock(m_thread_pool->m_worker_queue_mutex);
			m_thread_pool->m_workers_condition_variable.wait(lock, [this]() {
				return !this->ServerQueueEmpty() || this->ServerShutingDown();
				});

			//end this thread
			if (ServerShutingDown())
				break;

			//get the next request in the pool
			client_request_socket = m_thread_pool->m_worker_queue.front();
			m_thread_pool->m_worker_queue.pop();
		}

		if (client_request_socket != INVALID_SOCKET &&
			client_request_socket != SOCKET_ERROR) {
			ParseRequest(client_request_socket);

			//close the connection with the client
			int results = closesocket(client_request_socket);
			if (results != 0) {
				std::scoped_lock lock(m_console_mutex);
				std::cout << "Error closing client socket\n";
			}
		}
	}
}



void HttpServer::ParseRequest(SOCKET request_socket) {

#if _DEBUG

	{
		std::scoped_lock lock(m_console_mutex);
		std::cout << "Thread " << std::this_thread::get_id() << " starting new task\n\n";
	}


#endif

	int default_array_size = 8000 + 1;
	u_long non_blocking_setting = 1;
	u_long blocking_setting = 0;

	//non blocking for reciving data but blocking for sending data
	int results = ioctlsocket(request_socket, FIONBIO, &non_blocking_setting);
	if (results != 0) {
		std::scoped_lock lock(m_console_mutex);
		std::cout << "Error setting socket to non-blocking\n";
	}

	bool request_ended = false;
	bool started_keep_alive_timer = false;

	std::chrono::time_point<std::chrono::system_clock> keep_alive_start_point;

	while (!request_ended) {

		if (started_keep_alive_timer) {
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - keep_alive_start_point).count() > 1100) {
				request_ended = true;
				continue;
			}
		}

		std::unique_ptr<char[]> request_buffer = std::make_unique<char[]>(default_array_size);

		ZeroMemory(request_buffer.get(), default_array_size);

		results = recv(request_socket, request_buffer.get(), default_array_size - 1, 0);

		if (results <= 0) {
			
			if (!started_keep_alive_timer) {
				started_keep_alive_timer = true;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			continue;
		}

		//dont trust that client put null
		request_buffer[results] = '\0';

		std::string http_request(request_buffer.get());
		std::string_view http_request_view(http_request);
		std::string client_ip;

		GetClientip(request_socket, client_ip);

		//client will close connection after our response
		if (!KeepAlive(http_request))
			request_ended = true;

		std::string http_response;

		//fulfill request
		SendToProperMethodFunction(http_response, http_request_view, !request_ended);

		//set socket to blocking for send
		results = ioctlsocket(request_socket, FIONBIO, &blocking_setting);
		if (results != 0) {
			std::scoped_lock lock(m_console_mutex);
			std::cout << "Error setting socket to blocking\n";
		}

		//send response
		int sent_bytes = send(request_socket, http_response.c_str(), http_response.size(), NULL);
		if (sent_bytes == SOCKET_ERROR) {
			std::scoped_lock lock(m_console_mutex);
			std::cout << "Error sending message\n";
		}

		//set the socket back to non_blocking for recv
		results = ioctlsocket(request_socket, FIONBIO, &non_blocking_setting);
		if (results != 0) {
			std::scoped_lock lock(m_console_mutex);
			std::cout << "Error setting socket to non-blocking\n";
		}

		if (!request_ended && !started_keep_alive_timer) {
			started_keep_alive_timer = true;
			keep_alive_start_point = std::chrono::system_clock::now();
		}

		{
			std::scoped_lock lock(m_statistics_mutex);
			m_server_statistics.AddBytesSent(sent_bytes);
			m_server_statistics.AddBytesReceived(http_request.size());
		}
	}

#if _DEBUG

	{
		std::scoped_lock lock(m_console_mutex);
		std::cout << "Thread " << std::this_thread::get_id() << " ending task\n\n";
	}

#endif
}



void HttpServer::GetMethod(std::string& method, std::string_view& http_request) {

	//7 is max method size
	method.reserve(7);

	for (int i = 0; i < 7; ++i) {
		if (http_request[i] == '\0' || http_request[i] == ' ') {
			break;
		}
		method.push_back(http_request[i]);
	}
}



void HttpServer::SendToProperMethodFunction(std::string& response, std::string_view& http_request, bool keep_alive) {

	std::string method;
	GetMethod(method, http_request);

	//methods are case sensitive
	/*	"The Method token indicates the method to be performed on the
		resource identified by the Request-URI. The method is case-sensitive" RFC HTTP/1.1*/
	if (method == "GET") {
		HandleGetRequest(response, http_request, keep_alive);
	}
	else if (method == "POST") {
		HandlePostRequest(response, http_request, keep_alive);
	}
	else if (method == "HEAD") {
		HandleHeadRequest(response, http_request, keep_alive);
	}
	else if (method == "PUT") {
		HandlePutRequest(response, http_request, keep_alive);
	}
	else if (method == "DELETE") {
		HandleDeleteRequest(response, http_request, keep_alive);
	}
	else if (method == "CONNECT") {
		HandleConnectRequest(response, http_request, keep_alive);
	}
	else if (method == "OPTIONS") {
		HandleOptionsRequest(response, http_request, keep_alive);
	}
	else if (method == "TRACE") {
		HandleTraceRequest(response, http_request, keep_alive);
	}
	else if (method == "PATCH") {
		HandlePatchRequest(response, http_request, keep_alive);
	}
	else {
		HandleUnknownRequest(response, http_request, keep_alive);
	}
}



void HttpServer::HandleGetRequest(std::string& response, std::string_view& request, bool keep_alive) {

	std::string request_file_path;
	request_file_path.reserve(100);
	std::string file_contents;
	std::string uri_data;
	std::string_view uri;
	
	//4 is start index because GET is 3 characters
	SeperateUri(request, uri, 4);

	DecodeUri(uri, request_file_path, uri_data);

	int response_code = ReadFile(file_contents, request_file_path);

	std::string content_type;

	if (response_code == 404) {
		std::string file = "404.html";
		content_type = GetContentType(file);
	}
	else {
		content_type = GetContentType(request_file_path);
	}

	ConstructGetResponse(response, content_type, file_contents, keep_alive);
}



void HttpServer:: SeperateUri(std::string_view& http_request, std::string_view& uri, int start_index) {

	int last_index = 0;
	int i = start_index;

	while (http_request[i] != '\0' && http_request[i] != ' ') {
		i++;
	}

	uri = http_request.substr(start_index, i - start_index);
}



void HttpServer::DecodeUri(std::string_view& uri, std::string& file_path, std::string& uri_decoded) {

	file_path.reserve(100);
	uri_decoded.reserve(100);
	bool on_file_path = true;
	std::string encoded_character;

	for (int i = 0; i < 200 && i < uri.size(); ++i) {
		if (uri[i] == ' ' || uri[i] == '\0') {
			break;
		}
		else if (uri[i] == '?') {
			on_file_path = false;
			continue;
		}

		if (on_file_path) {
			file_path.push_back(uri[i]);
		}
		else {
			
			//encoded character
			if (uri[i] == '%') {

				//the data sent by user is malformed
				if (i + 2 > uri.size()) {
					break;
				}

				encoded_character += uri[i];
				encoded_character += uri[i + 1];
				encoded_character += uri[i + 2];

				i += 2;

				//encoded character is not supported or input is malformed
				if (m_uri_map.find(encoded_character) == m_uri_map.end()) {
					encoded_character.clear();
					continue;
				}

				uri_decoded.push_back(m_uri_map[encoded_character]); 

				encoded_character.clear();
			}
			else {
				uri_decoded.push_back(uri[i]);
			}

		}
	}

}



void HttpServer::ConstructGetResponse(std::string& response, std::string& content_type, std::string& file_contents, bool keep_alive) {

	int64_t size_of_full_message = 0;
	size_of_full_message += m_http_header_parts.m_http_version.size();
	size_of_full_message += 1; //space
	size_of_full_message += m_http_header_parts.m_200_ok.size();
	size_of_full_message += 1; //end line
	size_of_full_message += m_http_header_parts.m_server_name.size();
	size_of_full_message += 1; //end line

	if (keep_alive) {
		size_of_full_message += m_http_header_parts.m_connection_keep_alive.size();
		size_of_full_message += 1; //end line
		size_of_full_message += m_http_header_parts.m_keep_alive_header_default.size();
		size_of_full_message += 1; //end line
	}
	else {
		size_of_full_message += m_http_header_parts.m_connection_close.size();
		size_of_full_message += 1; //end line
	}

	size_of_full_message += m_http_header_parts.m_content_type_empty.size();
	size_of_full_message += 1; //space
	size_of_full_message += content_type.size();
	size_of_full_message += 1; //end line
	size_of_full_message += m_http_header_parts.m_content_length_empty.size();
	size_of_full_message += 1; //space
	size_of_full_message += std::to_string(file_contents.size()).size();
	size_of_full_message += 2; //two end lines
	size_of_full_message += file_contents.size();

	response.reserve(size_of_full_message);

	response += m_http_header_parts.m_http_version;
	response += ' ';
	response += m_http_header_parts.m_200_ok;
	response += '\n';
	response += m_http_header_parts.m_server_name;
	response += '\n';

	if (keep_alive) {
		response += m_http_header_parts.m_connection_keep_alive;
		response += '\n';
		response += m_http_header_parts.m_keep_alive_header_default;
		response += '\n'; //end line
	}
	else {
		response += m_http_header_parts.m_connection_close;
		response += '\n';
	}

	response += m_http_header_parts.m_content_type_empty;
	response += ' ';
	response += content_type;
	response += '\n';
	response += m_http_header_parts.m_content_length_empty;
	response += ' ';
	response += std::to_string(file_contents.size());
	response += "\n\n";
	response += file_contents;
}




std::string HttpServer::GetContentType(std::string& file_name) {

	if (file_name == "/")
		return "text/html";

	int index = file_name.find_last_of(".");

	if (index == std::string::npos)
		return "text/plain";

	std::string extension;
	extension = file_name.substr(index);

	for (int i = 0; i < extension.size(); i++) {
		extension[i] = std::tolower(extension[i]);
	}

	if (extension == ".html")
		return "text/html";
	if (extension == ".jpg" || extension == ".jpeg")
		return "image/jpg";
	if (extension == ".css")
		return "text/css";
	if (extension == ".ico")
		return "image/x-icon";
	if (extension == ".js")
		return "application/javascript";
	if (extension == ".png")
		return "image/png";
	if (extension == ".json")
		return "application/json";
	if (extension == ".zip")
		return "application/zip";
	if (extension == ".xhtml")
		return "application/xhtml+xml";
	if (extension == ".wav")
		return "audio/wav";
	if (extension == ".txt")
		return "text/plain";
	if (extension == ".ttf")
		return "font/ttf";
	if (extension == ".tif" || extension == ".tiff")
		return "image/tiff";
	if (extension == ".tar")
		return "application/x-tar";
	if (extension == ".svg")
		return "image/svg+xml";
	if (extension == ".pdf")
		return "application/pdf";
	if (extension == ".bin")
		return "application/octet-stream";
	if (extension == ".gif")
		return "image/gif";

	return "text/plain";
}



bool HttpServer::KeepAlive(const std::string_view& http_request) {

	int index = http_request.find("Connection: close");
	if (index != std::string::npos)
		return false;

	index = http_request.find("Connection: Close");
	if (index != std::string::npos)
		return false;

	index = http_request.find("Connection: CLOSE");
	if (index != std::string::npos)
		return false;

	return true;
}



void HttpServer::GetClientip(SOCKET& client_socket, std::string& client_ip) {

	struct sockaddr_in addr;
	int addr_size = sizeof(addr);

	ZeroMemory(&addr, sizeof(addr));

	if (getpeername(client_socket, (struct sockaddr*)&addr, (int*)&addr_size) != 0) {
		addr_size = -1;
		std::scoped_lock lock(m_console_mutex);
		std::cout << "Error getting peer name\n";
		return;
	}

	char ip[16] = { 0 };

	const char* ptr_buffer = inet_ntop(addr.sin_family, &addr.sin_addr, (char *)&ip, sizeof(ip));
	if (ptr_buffer == NULL) {
		std::scoped_lock lock(m_console_mutex);
		std::cout << "Error getting ip of client\n";
		addr_size = -1;
		return;
	}
	
	client_ip = ip;
}



int HttpServer::ReadFile(std::string& file_contents, const std::string& file_path) {

	std::string default_path = "./public";

	std::ifstream requested_file;

	if (file_path == "/") {
		requested_file.open(default_path + "/index.html", std::ios::binary | std::ios::in);
	}
	else {
		requested_file.open(default_path + file_path, std::ios::binary | std::ios::in);
	}

	if (requested_file.fail()) {

		std::ifstream file_404;
		file_404.open(default_path + "/404.html", std::ios::binary | std::ios::in);

		if (file_404.fail()) {
			file_contents = "Server Failure";
			return 500;
		}

		file_contents = std::move(std::string(std::istreambuf_iterator<char>(file_404), std::istreambuf_iterator<char>()));

		return 404;
	}

	file_contents = std::move(std::string(std::istreambuf_iterator<char>(requested_file), std::istreambuf_iterator<char>()));

	return 200;
}


//need to finsih implementing other methods
void HttpServer::HandlePostRequest(std::string& response, std::string_view& request, bool keep_alive) {

}



void HttpServer::HandleHeadRequest(std::string& response, std::string_view& request, bool keep_alive) {

}



void HttpServer::HandlePutRequest(std::string& response, std::string_view& request, bool keep_alive) {

}



void HttpServer::HandleDeleteRequest(std::string& response, std::string_view& request, bool keep_alive) {

}



void HttpServer::HandleConnectRequest(std::string& response, std::string_view& request, bool keep_alive) {

}



void HttpServer::HandleOptionsRequest(std::string& response, std::string_view& request, bool keep_alive) {

}



void HttpServer::HandleTraceRequest(std::string& response, std::string_view& request, bool keep_alive) {

}



void HttpServer::HandlePatchRequest(std::string& response, std::string_view& request, bool keep_alive) {

}



void HttpServer::HandleUnknownRequest(std::string& response, std::string_view& request, bool keep_alive) {

	int64_t size_of_full_message = 0;

	size_of_full_message += m_http_header_parts.m_http_version.size();
	size_of_full_message += 1; // space
	size_of_full_message += m_http_header_parts.m_501_not_supported.size();
	size_of_full_message += 1; // end line
	size_of_full_message += m_http_header_parts.m_server_name.size();
	size_of_full_message += 1; // endl line
	if (keep_alive) {
		size_of_full_message += m_http_header_parts.m_connection_keep_alive.size();
		size_of_full_message += 1; //end line
		size_of_full_message += m_http_header_parts.m_keep_alive_header_default.size();
	}
	else {
		size_of_full_message += m_http_header_parts.m_connection_close.size();
	}

	response.reserve(size_of_full_message);

	response += m_http_header_parts.m_http_version;
	response += ' ';
	response += m_http_header_parts.m_501_not_supported;
	response += '\n';
	response += m_http_header_parts.m_server_name;
	response += '\n';

	if (keep_alive) {
		response += m_http_header_parts.m_connection_keep_alive;
		response += '\n';
		response += m_http_header_parts.m_keep_alive_header_default;
	}
	else {
		response += m_http_header_parts.m_connection_close;
	}
}