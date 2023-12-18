#ifndef REPORT_GENERATOR_H
#define REPORT_GENERATOR_H

#include "order.h"

#include <string>
#include <iostream>
#include <chrono>
#include <sstream>

enum ReportExecutionStatus
{
	REPORT_NEW,
	REPORT_REJECTED,
	REPORT_FILL,
	REPORT_PFILL
};

inline std::ostream& operator<<(std::ostream& os, const ReportExecutionStatus& status)
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
	std::string side;
	ReportExecutionStatus status;
	int quantity;
	float price;
	std::string reason;
	time_stamp ts;

	inline Report(int order_id, std::string client_order_id, Instrument instrument, std::string side, ReportExecutionStatus status, 
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

	inline std::string to_string() const
	{
		auto tp = std::chrono::zoned_time{ std::chrono::current_zone(), ts }.get_local_time();
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

		std::stringstream os;

		os << "ord" << order_id << "," << client_order_id << "," << instrument << "," <<
			side << "," << status << "," << quantity << "," <<
			std::fixed << std::setprecision(2) << price << "," << reason << "," <<
			std::setprecision(4) << y <<
			std::setprecision(2) << unsigned(m) <<
			std::setprecision(2) << d << "-" <<
			std::setprecision(2) << h.count() <<
			std::setprecision(2) << M.count() <<
			std::setprecision(2) << s.count() << "." <<
			std::setprecision(3) << ms.count();
		return os.str();
	}
};

class ReportGenerator
{

public:
	inline void generate_normal_report(const OrderBookEntry* order)
	{
		reports.emplace_back(order->order_id, order->client_order_id, order->instrument, std::to_string(order->side), REPORT_NEW, order->quantity, order->price, "", order->ts);
	}

	inline void generate_fill_report(const OrderBookEntry* new_order, const OrderBookEntry* old_order, float price)
	{
		reports.emplace_back(new_order->order_id, new_order->client_order_id, new_order->instrument, std::to_string(new_order->side), REPORT_FILL,
			new_order->quantity, price, "", new_order->ts);

		reports.emplace_back(old_order->order_id, old_order->client_order_id, old_order->instrument, std::to_string(old_order->side), REPORT_FILL,
			old_order->quantity, price, "", old_order->ts);
	}

	inline void generate_pfill_report(const OrderBookEntry* fill_order, const OrderBookEntry* pfill_order, int quantity, float price)
	{
		reports.emplace_back(pfill_order->order_id, pfill_order->client_order_id, pfill_order->instrument, std::to_string(pfill_order->side),
			REPORT_PFILL, quantity, price, "", pfill_order->ts);

		reports.emplace_back(fill_order->order_id, fill_order->client_order_id, fill_order->instrument, std::to_string(fill_order->side),
			REPORT_FILL, quantity, price, "", fill_order->ts);
	}

	inline void generate_reject_report(Order order)
	{
		reports.emplace_back(order.order_id, order.client_order_id, order.instrument, order.side_string, REPORT_REJECTED, 
			order.quantity, order.price, order.error, 
			std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now()));
	}

	inline std::string to_string() const
	{
		std::stringstream os;

		for (auto& report : reports)
		{
			os << report.to_string() << "\n";
		}
		return os.str();
	}

	inline unsigned int get_count() const
	{
		return reports.size();
	}

	friend std::ostream& operator<<(std::ostream& os, const ReportGenerator& report_generator);

private:
	std::vector<Report> reports;
};

inline std::ostream& operator<<(std::ostream& os, const Report& report)
{
	os << report.to_string();
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const ReportGenerator& report_generator)
{
	os << report_generator.to_string();
	return os;
}

#endif // !REPORT_GENERATOR_H
