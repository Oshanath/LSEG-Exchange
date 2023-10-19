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
