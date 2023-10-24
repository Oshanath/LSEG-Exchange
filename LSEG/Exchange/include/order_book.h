#include <set>
#include <iostream>

#include "order.h"

class OrderBook
{

public:
	inline void add_order(Order order, bool pfill = false)
	{

		auto sell_top = sell_orders.begin();
		auto buy_top = buy_orders.begin();

		if (order.side == BUY)
		{
			if (sell_top == sell_orders.end() || order.price < sell_top->price)
			{
				// Normal Execution

				buy_orders.insert(BuyOrder(order));
				// generate report
			}
			else if (order.quantity != sell_top->quantity)
			{
				// PFill execution for either side

				if (order.quantity < sell_top->quantity)
				{
					// generate fill report for new buy order
					// generate pfill report for sell_top
					sell_top->quantity -= order.quantity;
				}
				else
				{
					// generate fill report for sell_top
					// generate pfill report for new buy order
					sell_orders.erase(sell_top);
					order.quantity = order.quantity - sell_top->quantity;
					add_order(order, true);
				}
			}
			else
			{
				// Fill execution

				// generate report
				// generate report
				sell_orders.erase(sell_top);
			}
		}
		else
		{
			if (buy_top == buy_orders.end() || order.price > buy_top->price)
			{
				// Normal Execution

				sell_orders.insert(SellOrder(order));
				// generate report
			}
			else if (order.quantity != buy_top->quantity)
			{
				// PFill execution for either side

				if (order.quantity < buy_top->quantity)
				{
					// generate fill report for new sell order
					// generate pfill report for buy_top
					buy_top->quantity -= order.quantity;
				}
				else
				{
					// generate fill report for buy_top
					// generate pfill report for new sell order
					buy_orders.erase(buy_top);
					order.quantity = order.quantity - buy_top->quantity;
					add_order(order, true);
				}
			}
			else
			{
				// Fill execution

				// generate report
				// generate report
				buy_orders.erase(buy_top);
			}
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
