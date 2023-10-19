#include <iostream>
#include <fstream>
#include <vector>

#include "order.h"
#include "rapidcsv.h"
#include "sockpp/tcp_connector.h"

std::vector<Order> read_orders(std::string filename);

int main()
{
	std::vector<Order> orders = read_orders("orders.csv");

	std::cout << "Sample TCP echo client for 'sockpp' " << '\n' << std::endl;

	std::string host = "localhost";
	in_port_t port = 8080;
	sockpp::initialize();

	// Implicitly creates an inet_address from {host,port}
	// and then tries the connection.

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

	unsigned char s[4], sret[1];
	unsigned int number = orders.size() * sizeof(Order);
	for (int i = 0; i < 4; i++)
	{
		s[3 - i] = (number >> (i * 8)) & 0xFF;
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


	// Write data
	if (conn.write(orders.data(), number) != number) {
		std::cerr << "Error writing to the TCP stream: "
			<< conn.last_error_str() << std::endl;
	}

	n = conn.read_n(sret, 1);

	if (n != ssize_t(1)) {
		std::cerr << "Error reading from TCP stream: "
			<< conn.last_error_str() << std::endl;
	}

	std::cout << int(sret[0]) << std::endl;



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
