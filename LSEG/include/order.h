#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <iostream>

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
		os << "ROSE";
		break;
	case LAVENDER:
		os << "LAVENDER";
		break;
	case LOTUS:
		os << "LOTUS";
		break;
	case TULIP:
		os << "TULIP";
		break;
	case ORCHID:
		os << "ORCHID";
		break;
	default:
		os << "UNKNOWN";
		break;
	}
	return os;
}

enum Side
{
	BUY = 1,
	SELL = 2
};

std::ostream& operator<<(std::ostream& os, const Side& side)
{
	os << int(side);
	return os;
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

	std::vector<char> serialize()
	{
		std::vector<char> data;
		
		for(int i = 0; i < client_order_id.length(); i++)
			data.push_back(client_order_id[i]);
		data.push_back('\0');

		data.push_back(instrument);

		data.push_back(side);

		data.resize(data.size() + 4);
		memcpy(data.data() + data.size() - 4, &quantity, 4);

		data.resize(data.size() + 4);
		memcpy(data.data() + data.size() - 4, &price, 4);

		for (int i = 0; i < trader_id.length(); i++)
			data.push_back(trader_id[i]);
		data.push_back('\0');

		std::vector<char> final_data;

		// first 4 bytes contain data size and then whole data vector
		final_data.resize(4 + data.size());
		int size = data.size();
		memcpy(final_data.data(), &size, 4);
		memcpy(final_data.data() + 4, data.data(), data.size());

		return final_data;
	}

	void deserialize(std::vector<char> data)
	{
		int i = 0;
		while (data[i] != '\0')
		{
			client_order_id += data[i];
			i++;
		}
		i++;

		instrument = Instrument(data[i]);
		i++;

		side = Side(data[i]);
		i++;

		memcpy(&quantity, data.data() + i, 4);
		i += 4;

		memcpy(&price, data.data() + i, 4);
		i += 4;

		while (data[i] != '\0')
		{
			trader_id += data[i];
			i++;
		}
		i++;
	}

	static std::vector<Order> deserialize_order_array(std::vector<char>& data)
	{
		std::vector<Order> orders;
		int i = 0;
		int order_count = 1;

		while (i < data.size())
		{
			int order_size = 0;
			memcpy(&order_size, data.data(), 4);

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