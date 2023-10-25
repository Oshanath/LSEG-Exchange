#include "order_book.h"
#include "order.h"

#include <vector>

inline void print_order_books(std::vector<OrderBook> order_books)
{
	for (int i = 0; i < order_books.size(); i++)
	{
		std::cout << "Order book for instrument " << Instrument(i) << ":\n";
		std::cout << order_books[i] << "\n";
	}
}

inline void process_orders(std::vector<Order> orders)
{
	ReportGenerator report_generator;
	std::vector<OrderBook> order_books(Instrument::count, report_generator);

	for (auto& order : orders)
	{
		order_books[order.instrument].add_order(order);
	}

	std::cout << "Reports:\n";
	std::cout << report_generator;
}