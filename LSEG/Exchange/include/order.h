#ifndef ORDER_H
#define ORDER_H

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

using time_stamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;

enum Instrument
{
	ROSE,
	LAVENDER,
	LOTUS,
	TULIP,
	ORCHID,
	count,
	INVALID_INSTRUMENT
};

inline std::ostream& operator<<(std::ostream& os, const Instrument& instrument)
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
	case INVALID_INSTRUMENT:
		os << "";
		break;
	}
	return os;
}

inline Instrument get_instrument(const std::string& name)
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
		return Instrument::INVALID_INSTRUMENT;
}

enum Side
{
	BUY = 1,
	SELL = 2,
	INVALID_SIDE = 3
};

inline Side get_side(const std::string& side)
{
	if (side == "1")
		return Side::BUY;
	else if (side == "2")
		return Side::SELL;
	else
		return Side::INVALID_SIDE;
}

inline std::ostream& operator<<(std::ostream& os, const Side& side)
{
	os << int(side);
	return os;
}

inline std::string deserialize_string(const std::vector<char>& data, int& i)
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
	std::string side_string;
	int quantity;
	float price;
	std::string trader_id;
	std::string error;
	bool rejected;

	inline void deserialize_and_validate(std::vector<char> data)
	{

		int i = 0;
		rejected = false;

		client_order_id = deserialize_string(data, i);
		if (!validate_string(client_order_id))
		{
			rejected = true;
			error = "Client order ID not found";
		}

		std::string instrument_string = deserialize_string(data, i);
		instrument = get_instrument(instrument_string);
		if (instrument == Instrument::INVALID_INSTRUMENT)
		{
			rejected = true;
			error = "Invalid instrument";
		}

		side_string = deserialize_string(data, i);
		side = get_side(side_string);
		if (side == Side::INVALID_SIDE)
		{
			rejected = true;
			error = "Invalid side";
		}

		std::string quantity_string = deserialize_string(data, i);
		std::tuple<bool, int, std::string> quantity_validation_result = validate_quantity(quantity_string);
		if (!std::get<0>(quantity_validation_result))
		{
			rejected = true;
			error = std::get<2>(quantity_validation_result);
		}
		quantity = std::get<1>(quantity_validation_result);

		std::string price_string = deserialize_string(data, i);
		std::tuple<bool, float, std::string> price_validation_result = validate_price(price_string);
		if (!std::get<0>(price_validation_result))
		{
			rejected = true;
			error = std::get<2>(price_validation_result);
		}
		price = std::get<1>(price_validation_result);

		trader_id = deserialize_string(data, i);
		if (!validate_string(trader_id))
		{
			rejected = true;
			error = "Trader ID not found";
		}
	}

	inline static std::vector<Order> deserialize_order_array(const std::vector<char>& data)
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
			order.deserialize_and_validate(std::vector<char>(data.begin() + 4 + i, data.begin() + 4 + order_size + i));
			orders.push_back(order);

			i += (4 + order_size);
		}

		return orders;
	}

private:
	inline bool validate_string(const std::string& client_order_id) const
	{
		if (client_order_id.empty())
			return false;

		return (!std::all_of(client_order_id.begin(), client_order_id.end(), [](unsigned char c) { return std::isspace(c); }));
	}

	inline std::tuple<bool, int, std::string> validate_quantity(const std::string& quantity_string) const
	{
		std::string message("Invalid size");

		bool is_int = std::all_of(quantity_string.begin(), quantity_string.end(), [](unsigned char c) { return std::isdigit(c) || c == '-';  });

		if (!is_int)
			return std::make_tuple<bool, int, std::string>(false, 0, std::move(message));

		int quantity = std::stoi(quantity_string);

		if (quantity % 10 != 0 || quantity <= 10 || quantity >= 1000)
			return std::make_tuple<bool, int, std::string>(false, std::move(quantity), std::move(message));

		return std::make_tuple<bool, int, std::string>(true, std::move(quantity), "");
	}

	inline std::tuple<bool, float, std::string> validate_price(const std::string& price_string) const
	{
		std::string message("Invalid price");

		bool is_float = std::all_of(price_string.begin(), price_string.end(), [](unsigned char c) { return std::isdigit(c) || c == '.' || c == '-';  });

		if (!is_float)
			return std::make_tuple<bool, float, std::string>(false, 0, std::move(message));

		float price = std::stof(price_string);

		if (price <= 0.0f)
			return std::make_tuple<bool, float, std::string>(false, std::move(price), std::move(message));

		return std::make_tuple<bool, float, std::string>(true, std::move(price), "");
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

inline std::ostream& operator<<(std::ostream& os, const OrderBookEntry& order_book_entry)
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

inline bool operator < (const BuyOrder& lhs, const BuyOrder& rhs)
{
	if (lhs.price != rhs.price)
		return lhs.price > rhs.price;
	else
		return lhs.ts < rhs.ts;
}

inline bool operator < (const SellOrder& lhs, const SellOrder& rhs)
{
	if (lhs.price != rhs.price)
		return lhs.price < rhs.price;
	else
		return lhs.ts < rhs.ts;
}

#endif // ORDER_H