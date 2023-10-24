#include <set>
#include <iostream>

#include "order.h"

class OrderBook
{

public:
	inline void add_order(Order order)
	{
		if (order.side == BUY)
		{
			buy_orders.insert(BuyOrder(order));
		}
		else
		{
			sell_orders.insert(SellOrder(order));
		}
	}

private:
	std::set<BuyOrder> buy_orders;
	std::set<SellOrder> sell_orders;

	friend std::ostream& operator<<(std::ostream& os, const OrderBook& order_book);
};

std::ostream& operator<<(std::ostream& os, const OrderBook& order_book)
{
	os << "Buy orders:\n";
	for (auto& buy_order : order_book.buy_orders)
	{
		os << buy_order << "\n";
	}

	os << "\n";

	os << "Sell orders:\n";
	for (auto& sell_order : order_book.sell_orders)
	{
		os << sell_order << "\n";
	}

	return os;
}
