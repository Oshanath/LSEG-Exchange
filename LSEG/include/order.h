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
};

#endif // ORDER_H