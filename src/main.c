#include "io.h"
#include "connections.h"
#include "logic.h"
// melo TCP
// goncalo UDP

int main(int argc, char const *argv[]) {
	int port;
	char ip[16];

	read_arguments(argc, (char**) argv, &port, ip);

	return 0;
}