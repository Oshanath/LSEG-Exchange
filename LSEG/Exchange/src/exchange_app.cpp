#include <iostream>
#include <thread>
#include <vector>

#include "sockpp/tcp_acceptor.h"
#include "order.h"

void run_echo(sockpp::tcp_socket sock)
{
	ssize_t n;
	unsigned char buf[512];
	unsigned int size = 0;

	// read buffer size
	if (n = sock.read(buf, sizeof(buf)) == 4)
	{
		buf[0] = 1;
		sock.write_n(buf, 1);
	}

	for (int i = 0; i < 4; i++)
	{
		size <<= 8;
		size |= (unsigned char)buf[i];
	}
	std::cout << "received " << size << "\n";

	unsigned char* data = new unsigned char[size];
	if ((n = sock.read(data, size)) == size)
	{
		buf[0] = 1;
		sock.write_n(buf, 1);
	}

	std::vector<Order> orders(size / sizeof(Order));
	memcpy(orders.data(), data, size);

	delete[] data;

	for (auto& order : orders)
	{
		std::cout << order.order_id << " " << order.instrument << " " << order.side << " " << order.quantity << " " << order.price << " " << order.trader_id << std::endl;
	}

}

int main(int argc, char* argv[])
{
	std::cout << "Sample TCP echo server for 'sockpp' " << '\n' << std::endl;

	in_port_t port = 8080;
	sockpp::initialize();
	sockpp::tcp_acceptor acc(port);

	if (!acc) {
		std::cerr << "Error creating the acceptor: " << acc.last_error_str() << std::endl;
		return 1;
	}
	std::cout << "Awaiting connections on port " << port << "..." << std::endl;

	while (true) {
		sockpp::inet_address peer;

		// Accept a new client connection
		sockpp::tcp_socket sock = acc.accept(&peer);
		std::cout << "Received a connection request from " << peer << std::endl;

		if (!sock) {
			std::cerr << "Error accepting incoming connection: "
				<< acc.last_error_str() << std::endl;
		}
		else {
			// Create a thread and transfer the new stream to it.
			std::thread thr(run_echo, std::move(sock));
			thr.detach();
		}
	}

	return 0;
}