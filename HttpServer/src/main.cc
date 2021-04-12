#include <iostream>
#include <string>

#include "http_server.h"

int main() {

	HttpServer server("80");

	server.StartHttpServer();

	return 0;
}
