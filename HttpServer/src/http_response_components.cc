#include <iostream>
#include <string>

#include "http_response_components.h"

http_response_components::http_response_components()
	: m_200_ok("200 OK"),
	m_404_not_found("404 Not Found"),
	m_connection_close("Connection: Close"),
	m_connection_keep_alive("Connection: Keep-Alive"),
	m_content_length_empty("Content-Length:"),
	m_server_name("Server: Carter's/2.0"),
	m_content_type_empty("Content-Type:"),
	m_xss_protection_0("x-xss-protection: 0"),
	m_http_version("HTTP/1.1"),
	m_500_server_error("500 Internal Server Error"),
	m_keep_alive_header_default("Keep-Alive: timeout=1, max=100"),
	m_keep_alive_header_empty("Keep-Alive:"),
	m_501_not_supported("501 Not Implemented")
{
}
