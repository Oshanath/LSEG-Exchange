#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <iostream>

enum Instrument
{
	ROSE,
	LAVENDER,
	LOTUS,
	TULIP,
	ORCHID
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
	std::string order_id;
	Instrument instrument;
	Side side;
	int quantity;
	float price;
	std::string trader_id;

	std::vector<char> serialize()
	{
		std::vector<char> data;
		
		for(int i = 0; i < order_id.length(); i++)
			data.push_back(order_id[i]);
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
			order_id += data[i];
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

		while (i < data.size())
		{
			int order_size = 0;
			memcpy(&order_size, data.data(), 4);

			Order order;
			order.deserialize(std::vector<char>(data.begin() + 4 + i, data.begin() + 4 + order_size + i));
			orders.push_back(order);

			i += (4 + order_size);
		}

		return orders;
	}
};

#endif // ORDER_H