#ifndef ORDER_PROCESSOR_H
#define ORDER_PROCESSOR_H

#include "order_book.h"
#include "order.h"

#include <vector>

inline void print_order_books(const std::vector<OrderBook>& order_books)
{
	for (int i = 0; i < order_books.size(); i++)
	{
		std::cout << "Order book for instrument " << Instrument(i) << ":\n";
		std::cout << order_books[i] << "\n";
	}
}

inline void process_orders(std::vector<Order>& orders)
{
	ReportGenerator report_generator;
	std::vector<OrderBook> order_books(Instrument::count, report_generator);

	for (auto& order : orders)
	{
		if(!order.rejected)
			order_books[order.instrument].add_order(order);
		else
			report_generator.generate_reject_report(order);
	}

	std::cout << "Reports:\n";
	std::cout << report_generator;
}

#endif // !ORDER_PROCESSOR_H
