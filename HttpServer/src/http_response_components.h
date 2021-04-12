#ifndef HTTP_RESPONSE_COMPONENTS_H
#define HTTP_RESPONSE_COMPONENTS_H

#include <iostream>
#include <string>

struct http_response_components {
	http_response_components();

	const std::string m_server_name;
	const std::string m_200_ok;
	const std::string m_404_not_found;
	const std::string m_connection_close;
	const std::string m_content_type_empty;
	const std::string m_xss_protection_0;
	const std::string m_content_length_empty;
	const std::string m_connection_keep_alive;
	const std::string m_http_version;
	const std::string m_500_server_error;
	const std::string m_keep_alive_header_default;
	const std::string m_keep_alive_header_empty;
	const std::string m_501_not_supported;
};

typedef struct http_response_components HttpResponseComponents;

#endif // !HTTP_RESPONSE_COMPONENTS_H