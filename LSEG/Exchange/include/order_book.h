#include <set>
#include <iostream>
#include <memory>

#include "order.h"
#include "execution_type.h"

class OrderBook
{

public:
	inline void add_order(Order order, bool pfill = false)
	{

		ExecutionType execution_type = get_execution_type(order);

		switch (execution_type)
		{
			case NORMAL:
				if (order.side == BUY)
				{
					buy_orders.emplace(order);
				}
				else
				{
					sell_orders.emplace(order);
				}

				break;

			case FILL:
				if (order.side == BUY)
				{
					sell_orders.erase(sell_orders.begin());
				}
				else
				{
					buy_orders.erase(buy_orders.begin());
				}

				break;

			case NEW_PFILL:
				if (order.side == BUY)
				{
					order.quantity -= sell_orders.begin()->quantity;
					sell_orders.erase(sell_orders.begin());
				}
				else
				{
					order.quantity -= buy_orders.begin()->quantity;
					buy_orders.erase(buy_orders.begin());
				}
				add_order(order, true);

				break;

			case OLD_PFILL:
				if (order.side == BUY)
				{
					Order sell_order = *sell_orders.begin();
					sell_order.quantity -= order.quantity;
					sell_orders.erase(sell_orders.begin());
					sell_orders.insert(sell_order);
				}
				else
				{
					Order buy_order = *buy_orders.begin();
					buy_order.quantity -= order.quantity;
					buy_orders.erase(buy_orders.begin());
					buy_orders.insert(buy_order);
				}

				break;
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const OrderBook& order_book);

private:
	std::set<BuyOrder> buy_orders;
	std::set<SellOrder> sell_orders;

	bool is_aggressive(Order& order)
	{
		auto sell_top = sell_orders.begin();
		auto buy_top = buy_orders.begin();

		if (order.side == BUY && (sell_top == sell_orders.end() || order.price < sell_top->price))
			return false;
		
		else if (order.side == SELL && (buy_top == buy_orders.end() || order.price > buy_top->price))
			return false;

		return true;
	}

	ExecutionType get_execution_type(Order& new_order)
	{

		if (!is_aggressive(new_order))
		{
			return NORMAL;
		}

		const OrderBookEntry* matching_order;

		if (new_order.side == BUY)
			matching_order = &(*sell_orders.begin());
		else
			matching_order = &(*buy_orders.begin());

		if (new_order.quantity != matching_order->quantity)
		{
			if (new_order.quantity < matching_order->quantity)
				return OLD_PFILL;
			else
				return NEW_PFILL;
		}
		else
		{
			return FILL;
		}
	}

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
