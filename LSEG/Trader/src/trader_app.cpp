#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "string_order.h"
#include "rapidcsv.h"
#include "sockpp/tcp_connector.h"

std::vector<Order> read_orders(std::string filename);

void send_data_size(sockpp::tcp_connector& conn, unsigned int size)
{

	unsigned char s[4], sret[1];

	unsigned int data_size = size;
	memcpy(s, &data_size, 4);

	if (conn.write(s, 4) != 4) {
		std::cerr << "Error writing to the TCP stream: "
			<< conn.last_error_str() << std::endl;
	}

	ssize_t n = conn.read_n(sret, 1);

	if (n != ssize_t(1)) {
		std::cerr << "Error reading from TCP stream: "
			<< conn.last_error_str() << std::endl;
	}

	std::cout << int(sret[0]) << std::endl;
}

std::vector<char> serialize_data(std::vector<Order>& orders)
{
	std::vector<char> data;
	for (auto& order : orders)
	{
		auto order_data = order.serialize();
		data.insert(data.end(), order_data.begin(), order_data.end());
	}

	return data;
}

void send_data(sockpp::tcp_connector& conn, std::vector<char>& data)
{
	unsigned char sret[1];

	std::cout << "Data size is " << data.size() << " bytes.\n";

	if (conn.write(data.data(), data.size()) != data.size()) {
		std::cerr << "Error writing to the TCP stream: "
			<< conn.last_error_str() << std::endl;
	}

	ssize_t n = conn.read_n(sret, 1);

	if (n != ssize_t(1)) {
		std::cerr << "Error reading from TCP stream: "
			<< conn.last_error_str() << std::endl;
	}

	std::cout << int(sret[0]) << std::endl;
}

int main()
{
	std::vector<Order> orders = read_orders("example6.csv");
	std::string host = "localhost";
	in_port_t port = 8083;

	while (true)
	{
		sockpp::initialize();
		sockpp::tcp_connector conn({ host, port }, std::chrono::seconds{ 5 });
		if (!conn) {
			std::cerr << "Cannot connect to server at "
				<< sockpp::inet_address(host, port)
				<< "\n\t" << conn.last_error_str() << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(5));
			continue;
		}

		std::cout << "Created a connection from " << conn.address() << std::endl;

		// Set a timeout for the responses
		if (!conn.read_timeout(std::chrono::seconds(1))) {
			std::cerr << "Error setting timeout on TCP stream: "
				<< conn.last_error_str() << std::endl;
		}

		std::vector<char> data = serialize_data(orders);
		send_data_size(conn, data.size());
		send_data(conn, data);

		std::string a;
		getline(std::cin, a);
	}

}

std::vector<Order> read_orders(std::string filename)
{
	rapidcsv::Document doc(filename);
	std::vector<Order> orders;

	for (int i = 0; i < doc.GetRowCount(); i++)
	{
		Order order;
		order.client_order_id = doc.GetCell<std::string>(0, i);
		order.instrument = doc.GetCell<std::string>(1, i);
		order.side = doc.GetCell<std::string>(2, i);
		order.quantity = doc.GetCell<std::string>(3, i);
		order.price = doc.GetCell<std::string>(4, i);
		order.trader_id = doc.GetCell<std::string>(5, i);
		orders.push_back(order);
	}

	return orders;
}
