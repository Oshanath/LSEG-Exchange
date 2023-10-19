#include <iostream>
#include <fstream>
#include <vector>

#include "order.h"
#include "rapidcsv.h"
#include "sockpp/tcp_connector.h"

std::vector<Order> read_orders(std::string filename);

void send_data_size(sockpp::tcp_connector& conn, unsigned int size)
{

	unsigned char s[4], sret[1];
	unsigned int data_size = size;
	for (int i = 0; i < 4; i++)
	{
		s[3 - i] = (data_size >> (i * 8)) & 0xFF;
	}

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

void send_data(sockpp::tcp_connector& conn, std::vector<Order>& orders, unsigned int data_size)
{
	unsigned char sret[1];

	if (conn.write(orders.data(), data_size) != data_size) {
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
	std::vector<Order> orders = read_orders("orders.csv");
	std::string host = "localhost";
	in_port_t port = 8080;

	sockpp::initialize();
	sockpp::tcp_connector conn({ host, port }, std::chrono::seconds{ 5 });
	if (!conn) {
		std::cerr << "Error connecting to server at "
			<< sockpp::inet_address(host, port)
			<< "\n\t" << conn.last_error_str() << std::endl;
		return 1;
	}

	std::cout << "Created a connection from " << conn.address() << std::endl;

	// Set a timeout for the responses
	if (!conn.read_timeout(std::chrono::seconds(5))) {
		std::cerr << "Error setting timeout on TCP stream: "
			<< conn.last_error_str() << std::endl;
	}

	unsigned int data_size = orders.size() * sizeof(Order);
	send_data_size(conn, data_size);
	send_data(conn, orders, data_size);

	return (!conn) ? 1 : 0;
	
}

Instrument get_instrument(std::string&& name)
{
	if (name == "Rose")
		return Instrument::ROSE;
	else if (name == "Lavender")
		return Instrument::LAVENDER;
	else if (name == "Lotus")
		return Instrument::LOTUS;
	else if (name == "Tulip")
		return Instrument::TULIP;
	else if (name == "Orchid")
		return Instrument::ORCHID;
	else
		throw std::runtime_error("Unknown instrument");

}

std::vector<Order> read_orders(std::string filename)
{
	rapidcsv::Document doc(filename);
	std::vector<Order> orders;

	for (int i = 0; i < doc.GetRowCount(); i++)
	{
		Order order;
		order.order_id = doc.GetCell<std::string>(0, i);
		order.instrument = get_instrument(doc.GetCell<std::string>(1, i));
		order.side = Side(doc.GetCell<int>(2, i));
		order.quantity = doc.GetCell<int>(3, i);
		order.price = doc.GetCell<float>(4, i);
		order.trader_id = doc.GetCell<std::string>(5, i);
		orders.push_back(order);
	}

	return orders;
}
