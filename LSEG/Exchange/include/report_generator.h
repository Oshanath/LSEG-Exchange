#include "order.h"

#include <string>
#include <iostream>

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
};

class ReportGenerator
{

public:
	inline void generate_normal_report(const OrderBookEntry* order)
	{
		
	}

	inline void generate_fill_report(const OrderBookEntry* new_order, const OrderBookEntry* old_order)
	{

	}

	inline void generate_pfill_report(const OrderBookEntry* fill_order, const OrderBookEntry* pfill_order, int quantity)
	{

	}

private:
	std::vector<Report> reports;
};
