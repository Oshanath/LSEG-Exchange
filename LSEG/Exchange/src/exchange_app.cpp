#include <iostream>
#include <thread>
#include <vector>

#include "sockpp/tcp_acceptor.h"
#include "order.h"
#include "order_processor.h"

void connection_callback(sockpp::tcp_socket sock)
{
	ssize_t n;
	unsigned char data_size_buf[512];
	unsigned int size = 0;

	unsigned char return_buf[1];

	// read buffer size
	if (n = sock.read(data_size_buf, sizeof(data_size_buf)) == 4)
	{
		return_buf[0] = 1;
		sock.write_n(return_buf, 1);
	}
	else
	{
		return_buf[0] = 0;
		sock.write_n(return_buf, 1);
		throw std::runtime_error("Did not receive 4 bytes for the data size.");
	}

	// construct data size
	memcpy(&size, data_size_buf, 4);
	std::cout << "Data size = " << size << " bytes.\n";

	// read data
	unsigned char* data = new unsigned char[size];
	if ((n = sock.read(data, size)) == size)
	{
		return_buf[0] = 1;
		sock.write_n(return_buf, 1);
	}
	else
	{
		return_buf[0] = 0;
		sock.write_n(return_buf, 1);
		throw std::runtime_error("Did not receive the correct number of bytes for the data size.");
	}

	auto data_vec = std::vector<char>(data, data + size);
	std::vector<Order> orders = Order::deserialize_order_array(data_vec);
	delete[] data;

	for (auto& order : orders)
	{
		std::cout << order.order_id << " " << order.instrument << " " << order.side << " " << order.quantity << " " << order.price << " " << order.trader_id << std::endl;
	}

	process_orders(orders);
}

int main(int argc, char* argv[])
{
	in_port_t port = 8083;
	sockpp::initialize();
	sockpp::tcp_acceptor acc(port);

	if (!acc) {
		std::cerr << "Error creating the acceptor: " << acc.last_error_str() << std::endl;
		return 1;
	}
	std::cout << "Awaiting connections on port " << port << "..." << std::endl;

	while (true) {
		sockpp::inet_address peer;
		sockpp::tcp_socket sock = acc.accept(&peer);
		std::cout << "Received a connection request from " << peer << std::endl;

		if (!sock) {
			std::cerr << "Error accepting incoming connection: "
				<< acc.last_error_str() << std::endl;
		}
		else {
			std::thread thr(connection_callback, std::move(sock));
			thr.detach();
		}
	}

	return 0;
}