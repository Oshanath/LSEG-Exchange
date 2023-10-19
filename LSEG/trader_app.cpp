#include <iostream>
#include <fstream>
#include <vector>

#include "order.h"
#include "rapidcsv.h"

std::vector<Order> read_orders(std::string filename);

int main()
{
	std::vector<Order> orders = read_orders("orders.csv");
	for (auto& order : orders)
	{
		std::cout << order.order_id << " " << order.instrument << " " << order.side << " " << order.quantity << " " << order.price << " " << order.trader_id << std::endl;
	}
}

std::vector<Order> read_orders(std::string filename)
{
	rapidcsv::Document doc(filename);
	std::vector<Order> orders;

	for (int i = 0; i < doc.GetRowCount(); i++)
	{
		Order order;
		order.order_id = doc.GetCell<std::string>("order_id", i);
		order.instrument = doc.GetCell<Instrument>("instrument", i);
		order.side = doc.GetCell<Side>("side", i);
		order.quantity = doc.GetCell<int>("quantity", i);
		order.price = doc.GetCell<float>("price", i);
		order.trader_id = doc.GetCell<std::string>("trader_id", i);
		orders.push_back(order);
	}

	return orders;
}
