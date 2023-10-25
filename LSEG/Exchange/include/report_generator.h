#include "order.h"

#include <string>
#include <iostream>
#include <chrono>

enum ReportExecutionStatus
{
	REPORT_NEW,
	REPORT_REJECTED,
	REPORT_FILL,
	REPORT_PFILL
};

std::ostream& operator<<(std::ostream& os, const ReportExecutionStatus& status)
{
	switch (status)
	{
	case REPORT_NEW:
		os << "New";
		break;
	case REPORT_REJECTED:
		os << "Rejected";
		break;
	case REPORT_FILL:
		os << "Fill";
		break;
	case REPORT_PFILL:
		os << "Pfill";
		break;
	}
	return os;
}

struct Report
{
	int order_id;
	std::string client_order_id;
	Instrument instrument;
	Side side;
	ReportExecutionStatus status;
	int quantity;
	float price;
	std::string reason;
	time_stamp ts;

	Report(int order_id, std::string client_order_id, Instrument instrument, Side side, ReportExecutionStatus status, 
		int quantity, float price, std::string reason, time_stamp ts)
		: order_id(order_id), 
		client_order_id(client_order_id), 
		instrument(instrument), 
		side(side), 
		status(status), 
		quantity(quantity), 
		price(price), 
		reason(reason), 
		ts(ts) {}
};

class ReportGenerator
{

public:
	inline void generate_normal_report(const OrderBookEntry* order)
	{
		reports.emplace_back(order->order_id, order->client_order_id, order->instrument, order->side, REPORT_NEW, order->quantity, order->price, "", order->ts);
	}

	inline void generate_fill_report(const OrderBookEntry* new_order, const OrderBookEntry* old_order, float price)
	{
		reports.emplace_back(new_order->order_id, new_order->client_order_id, new_order->instrument, new_order->side, REPORT_FILL, 
			new_order->quantity, price, "", new_order->ts);

		reports.emplace_back(old_order->order_id, old_order->client_order_id, old_order->instrument, old_order->side, REPORT_FILL, 
			old_order->quantity, price, "", old_order->ts);
	}

	inline void generate_pfill_report(const OrderBookEntry* fill_order, const OrderBookEntry* pfill_order, int quantity, float price)
	{
		reports.emplace_back(pfill_order->order_id, pfill_order->client_order_id, pfill_order->instrument, pfill_order->side, 
			REPORT_PFILL, quantity, price, "", pfill_order->ts);

		reports.emplace_back(fill_order->order_id, fill_order->client_order_id, fill_order->instrument, fill_order->side, 
			REPORT_FILL, quantity, price, "", fill_order->ts);
	}

	friend std::ostream& operator<<(std::ostream& os, const ReportGenerator& report_generator);

private:
	std::vector<Report> reports;
};

std::ostream& operator<<(std::ostream& os, const Report& report)
{

	auto tp = std::chrono::zoned_time{ std::chrono::current_zone(), report.ts }.get_local_time();
	auto dp = std::chrono::floor<std::chrono::days>(tp);
	std::chrono::year_month_day ymd{ dp };
	std::chrono::hh_mm_ss time{ std::chrono::floor<std::chrono::milliseconds>(tp - dp) };
	auto y = ymd.year();
	auto m = ymd.month();
	auto d = ymd.day();
	auto h = time.hours();
	auto M = time.minutes();
	auto s = time.seconds();
	auto ms = time.subseconds();

	os << "ord" << report.order_id << "," << report.client_order_id << "," << report.instrument << "," <<
		report.side << "," << report.status << "," << report.quantity << "," <<
		std::fixed << std::setprecision(2) << report.price << "," << report.reason << "," <<
		std::setprecision(4) << y << 
		std::setprecision(2) << unsigned(m) << 
		std::setprecision(2) << d << "-" << 
		std::setprecision(2) << h.count() << 
		std::setprecision(2) << M.count() << 
		std::setprecision(2) << s.count() << "." << 
		std::setprecision(3) << ms.count();
	return os;
}

std::ostream& operator<<(std::ostream& os, const ReportGenerator& report_generator)
{
	for (auto& report : report_generator.reports)
	{
		os << report << "\n";
	}
	return os;
}
