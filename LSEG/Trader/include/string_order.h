#ifndef STRING_ORDER_H
#define STRING_ORDER_H

#include <string>
#include <iostream>
#include <vector>
#include <array>

inline void serialize_string(const std::string& string, std::vector<char>& data)
{
	for (int i = 0; i < string.length(); i++)
		data.push_back(string[i]);
	data.push_back('\0');
}

inline std::array<unsigned char, 4> serialize_int(int value)
{
	// Endianness independent
	std::array<unsigned char, 4> bytes;
	bytes[0] = value >> 24;
	bytes[1] = value >> 16;
	bytes[2] = value >> 8;
	bytes[3] = value;
	return bytes;
}

struct Order 
{
	std::string client_order_id;
	std::string instrument;
	std::string side;
	std::string quantity;
	std::string price;
	std::string trader_id;

	inline std::vector<char> serialize()
	{
		std::vector<char> data;
		
		serialize_string(client_order_id, data);
		serialize_string(instrument, data);
		serialize_string(side, data);
		serialize_string(quantity, data);
		serialize_string(price, data);
		serialize_string(trader_id, data);

		std::vector<char> final_data;

		// first 4 bytes contain data size and then whole data vector
		final_data.resize(4 + data.size());

		int size = data.size();
		std::array<unsigned char, 4> bytes = serialize_int(size);
		memcpy(final_data.data(), bytes.data(), 4);
		memcpy(final_data.data() + 4, data.data(), data.size());

		return final_data;
	}
};

#endif // STRING_ORDER_H