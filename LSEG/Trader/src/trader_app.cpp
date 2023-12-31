#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "string_order.h"
#include "rapidcsv.h"
#include "sockpp/tcp_connector.h"

std::pair<std::vector<Order>, bool> read_orders(std::string filename);

void send_data_size(sockpp::tcp_connector& conn, unsigned int size)
{

	unsigned char s[4], sret[1];

	unsigned int data_size = size;
	s[0] = data_size >> 24;
	s[1] = data_size >> 16;
	s[2] = data_size >> 8;
	s[3] = data_size;

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
	std::string host = "localhost";
	in_port_t port = 8083;

	while (true)
	{
		std::string filename;
		std::cout << "Enter filename: ";
		getline(std::cin, filename);
		std::pair<std::vector<Order>, bool> orders = read_orders(filename);
		
		if (!orders.second)
			continue;

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

		std::vector<char> data = serialize_data(orders.first);
		send_data_size(conn, data.size());
		send_data(conn, data);

		unsigned char report_size_buf[4];
		conn.read_n(report_size_buf, 4);
		size_t report_size = 0;
		report_size = (size_t(report_size_buf[0]) << 24) | (size_t(report_size_buf[1]) << 16) | (size_t(report_size_buf[2]) << 8) | size_t(report_size_buf[3]);
		
		std::cout << "report size = " << report_size << " " << report_size << "\n";

		std::vector<char> report_data;
		report_data.resize(report_size);
		conn.read_n(report_data.data(), report_size);
		std::string report = std::string(report_data.begin(), report_data.end());


		std::ofstream output_file(filename.substr(0, filename.length() - 4) + "_report.txt");
		output_file << report;
		output_file.close();

		std::string a;
		getline(std::cin, a);
	}

}

std::pair<std::vector<Order>, bool> read_orders(std::string filename)
{
	std::unique_ptr<rapidcsv::Document> doc;

	try
	{
		doc = std::make_unique<rapidcsv::Document>(filename);
	}
	catch (std::exception e)
	{
		std::cout << "Error reading file: " << e.what() << std::endl;
		return std::make_pair<std::vector<Order>, bool>(std::vector<Order>(), false);
	}

	std::vector<Order> orders;

	for (int i = 0; i < doc->GetRowCount(); i++)
	{
		Order order;
		order.client_order_id = doc->GetCell<std::string>(0, i);
		order.instrument = doc->GetCell<std::string>(1, i);
		order.side = doc->GetCell<std::string>(2, i);
		order.quantity = doc->GetCell<std::string>(3, i);
		order.price = doc->GetCell<std::string>(4, i);
		order.trader_id = doc->GetCell<std::string>(5, i);
		orders.push_back(order);
	}

	return std::make_pair<std::vector<Order>, bool>(std::move(orders), true);
}
