#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <iostream>
#include <sstream>

using time_stamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;

enum Instrument
{
	ROSE,
	LAVENDER,
	LOTUS,
	TULIP,
	ORCHID,
	count
};

std::ostream& operator<<(std::ostream& os, const Instrument& instrument)
{
	switch (instrument)
	{
	case ROSE:
		os << "Rose";
		break;
	case LAVENDER:
		os << "Lavender";
		break;
	case LOTUS:
		os << "Lotus";
		break;
	case TULIP:
		os << "Tulip";
		break;
	case ORCHID:
		os << "Orchid";
		break;
	}
	return os;
}

std::pair<Instrument, bool> get_instrument(std::string& name)
{
	if (name == "Rose")
		return std::make_pair<Instrument,bool>(Instrument::ROSE, true);
	else if (name == "Lavender")
		return std::make_pair<Instrument, bool>(Instrument::LAVENDER, true);
	else if (name == "Lotus")
		return std::make_pair<Instrument, bool>(Instrument::LOTUS, true);
	else if (name == "Tulip")
		return std::make_pair<Instrument, bool>(Instrument::TULIP, true);
	else if (name == "Orchid")
		return std::make_pair<Instrument, bool>(Instrument::ORCHID, true);
	else
		return std::make_pair<Instrument, bool>(Instrument::ROSE, false);
}

enum Side
{
	BUY = 1,
	SELL = 2
};

std::pair<Side,bool> get_side(std::string& name)
{
	if (name == "1")
		return std::make_pair<Side, bool>(Side::BUY, true);
	else if (name == "2")
		return std::make_pair<Side, bool>(Side::SELL, true);
	else
		return std::make_pair<Side, bool>(Side::BUY, false);
}

std::ostream& operator<<(std::ostream& os, const Side& side)
{
	os << int(side);
	return os;
}

std::string deserialize_string(const std::vector<char>& data, int& i)
{
	std::stringstream ss;

	for(; i < data.size(); ++i)
	{
		if (data[i] == '\0')
			break;
		else
			ss << data[i];
	}
	i++;
	return ss.str();
}

struct Order 
{
	int order_id;
	std::string client_order_id;
	Instrument instrument;
	Side side;
	int quantity;
	float price;
	std::string trader_id;

	void deserialize(std::vector<char> data)
	{

		int i = 0;
		client_order_id = deserialize_string(data, i);

		std::string instrument_string = deserialize_string(data, i);
		instrument = get_instrument(instrument_string).first;

		std::string side_string = deserialize_string(data, i);
		side = get_side(side_string).first;

		std::string quantity_string = deserialize_string(data, i);
		quantity = std::stoi(quantity_string);

		std::string price_string = deserialize_string(data, i);
		price = std::stof(price_string);

		trader_id = deserialize_string(data, i);
	}

	static std::vector<Order> deserialize_order_array(std::vector<char>& data)
	{
		std::vector<Order> orders;
		int i = 0;
		int order_count = 1;

		while (i < data.size())
		{
			int order_size = 0;
			memcpy(&order_size, data.data() + i, 4);

			Order order;
			order.order_id = order_count++;
			order.deserialize(std::vector<char>(data.begin() + 4 + i, data.begin() + 4 + order_size + i));
			orders.push_back(order);

			i += (4 + order_size);
		}

		return orders;
	}
};

struct OrderBookEntry : public Order
{
	time_stamp ts;

	OrderBookEntry() = delete;

	OrderBookEntry(Order order) :
		Order(order),
		ts(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()))
	{}

	friend std::ostream& operator<<(std::ostream& os, const OrderBookEntry& order_book_entry);
};

std::ostream& operator<<(std::ostream& os, const OrderBookEntry& order_book_entry)
{
	os << order_book_entry.order_id << " " << order_book_entry.client_order_id << " " << 
		order_book_entry.instrument << " " << order_book_entry.side << " " << 
		order_book_entry.quantity << " " << order_book_entry.price << " " << 
		order_book_entry.trader_id << " " << order_book_entry.ts.time_since_epoch().count();
	return os;
}

struct BuyOrder : public OrderBookEntry
{
	BuyOrder() = delete;

	BuyOrder(Order order) :
		OrderBookEntry(order)
	{}

	friend bool operator < (const BuyOrder& lhs, const BuyOrder& rhs);
};

struct SellOrder : public OrderBookEntry
{
	SellOrder() = delete;

	SellOrder(Order order) :
		OrderBookEntry(order)
	{}

	friend bool operator < (const SellOrder& lhs, const SellOrder& rhs);
};

bool operator < (const BuyOrder& lhs, const BuyOrder& rhs)
{
	if (lhs.price != rhs.price)
		return lhs.price > rhs.price;
	else
		return lhs.ts < rhs.ts;
}

bool operator < (const SellOrder& lhs, const SellOrder& rhs)
{
	if (lhs.price != rhs.price)
		return lhs.price < rhs.price;
	else
		return lhs.ts < rhs.ts;
}

#endif // ORDER_H