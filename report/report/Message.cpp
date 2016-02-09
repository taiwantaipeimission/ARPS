#include "Message.h"
#include "Area.h"
#include <sstream>
#include <iomanip>

Message::Message()
{
}


Message::~Message()
{
}

int get_octet_value(std::wstring octet_rep, int octet_size)
{
	std::wstringstream conversion_stream;
	int octet_hex_value;

	conversion_stream.clear();
	conversion_stream << std::hex << octet_rep;
	conversion_stream >> octet_hex_value;

	return octet_hex_value;
}

int extract_octet_value(std::wstringstream& ss, int octet_size)
{
	wchar_t chars[256];
	std::wstring str;
	ss.get(chars, octet_size + 1);
	return get_octet_value((std::wstring)chars, octet_size);
}

std::wstring encode_phone_number(std::wstring ph_num_str)
{
	std::wstringstream ss(ph_num_str);
	ss.ignore(256, '+');
	std::wstring encoded = L"";
	int ph_num_octets = (ss.str().length() + 1) / 2;
	wchar_t orig[3] = L"";
	wchar_t swapped[3] = L"";
	for (int i = 0; i < ph_num_octets; i++)
	{
		ss.get(orig, 3);
		swapped[0] = orig[1] == EOF ? 'F' : orig[1];
		swapped[1] = orig[0];
		encoded += swapped;
	}
	return encoded;
}

std::wstring extract_phone_number(std::wstringstream& ss, int ph_num_length)
{
	std::wstring number = L"+";
	int ph_num_octets = (ph_num_length + 1) / 2;

	{
		//original semi-octet
		wchar_t orig[3] = L"";
		//swapped semi-octet
		wchar_t swapped[3] = L"";
		for (int i = 0; i < ph_num_octets; i++)
		{
			ss.get(orig, 3);
			swapped[0] = orig[1];
			swapped[1] = orig[0] == 'F' ? '\0' : orig[0];
			number += swapped;
		}
	}
	return number;
}

std::wstring extract_septets(std::wstringstream&ss, int length)
{
	std::vector<unsigned int> data;
	for (int i = 0; i < length; i++)
	{
		int octet_value = extract_octet_value(ss, 2);
		data.push_back(octet_value);
	}
	std::wstring decoded_data = L"";

	int offset = 0;
	int index = 0;
	unsigned int base = 0u;
	unsigned int adder = 0u;
	unsigned int result = 0u;
	while (index < length)
	{
		if (offset == 0)
		{
			base = data[index] & 0x7F;
			adder = 0u;
			result = base | adder;
			offset++;
			index++;
		}
		else if (offset == 7)
		{
			base = 0u;
			adder = (data[index - 1]) >> (8 - offset);
			result = base | adder;
			offset = 0;
		}
		else
		{
			base = ((data[index]) << offset) & 0x7F;
			adder = (data[index - 1]);
			int new_offset = 8 - offset;
			adder = adder >> new_offset;
			result = base | adder;
			offset++;
			index++;
		}
		decoded_data.push_back((wchar_t)result);
	}

	return decoded_data;
}

std::wstring Message::encode(std::wstring dest_number)
{
	std::wstringstream encoded(L"");
	std::wstring raw_pdu_contents = raw_pdu.substr(raw_pdu.length() - msg_length, msg_length);
	encoded << std::hex << "001100" << dest_number.length() << "91";
	std::wstring dest_number_encoded = encode_phone_number(dest_number);
	encoded << dest_number_encoded << std::setfill('0') << std::setw(2) << std::hex << data_coding << "AA" << msg_length;
	encoded << raw_pdu_contents;

	return encoded.str();
}

void Message::parse(std::wstring input, CompList* comp_list)
{
	int newline_loc = input.find(L"\n");
	input = input.substr(newline_loc + 1, input.size() - newline_loc);
	raw_pdu = input;

	std::wstringstream ss;
	ss.clear();
	ss.str(input);
	int service_center_number_length = extract_octet_value(ss, 2);
	int service_center_type = extract_octet_value(ss, 2);
	std::wstring service_center_number = extract_phone_number(ss, service_center_number_length * 2);
	int sender_number_length = extract_octet_value(ss, 2);
	int sender_number_type = extract_octet_value(ss, 2);
	sender_number = extract_phone_number(ss, sender_number_length);
	int protocol_id = extract_octet_value(ss, 2);
	data_coding = extract_octet_value(ss, 2);
	std::wstring time = extract_phone_number(ss, 14);
	msg_length = extract_octet_value(ss, 2);

	contents = L"";
	if (data_coding == 8)
	{
		for (int i = 0; i < (msg_length + 1) / 2; i++)
		{
			contents.push_back(extract_octet_value(ss, 4));		//Extract in 2-byte octet pairs
		}
	}
	else
	{
		contents = extract_septets(ss, msg_length);
	}

	if (comp_list && comp_list->areas.count(sender_number) > 0)
	{
		sender_name = comp_list->areas[sender_number].area_name;
	}
	else
		sender_name = sender_number;

	int type_start = contents.find(L"TYPE:");
	if (type_start != std::wstring::npos)
	{
		type_start = contents.find_first_not_of(' ', type_start + 5);
		int type_end = contents.find(L"\n", type_start);
		std::wstring type_str = contents.substr(type_start, type_end - type_start);
		if (type_str == L"REPORT")
			type = TYPE_REPORT;
		else if (type_str == L"ENGLISH")
			type = TYPE_REPORT_ENGLISH;
		else if (type_str == L"REFERRAL")
			type = TYPE_REFERRAL;
		else
			type = TYPE_UNKNOWN;
	}

	/*
	sender_number = L"";
	sender_name = L"";
	int cmgl_id_start = input.find(L"CMGL: ");
	if (cmgl_id_start != std::wstring::npos)
	{
		cmgl_id_start += 6;
		int cmgl_id_end = input.find(L",", cmgl_id_start);
		cmgl_id = input.substr(cmgl_id_start, cmgl_id_end - cmgl_id_start);
	}

	int number_start = input.find(L"+886");
	if (number_start != std::wstring::npos)
	{
		sender_number = input.substr(number_start, 13);
		if (comp_list && comp_list->areas.count(sender_number) > 0)
		{
			sender_name = comp_list->areas[sender_number].area_name;
		}
		else
			sender_name = sender_number;
	}

	int contents_start = input.find(L"\n", number_start) + 1;
	std::wstring raw_contents = input.substr(contents_start, input.find_last_of(L"\r\n") - contents_start - 1);
	
	contents = L"";
	int hex_value = 0;
	std::wstringstream ss;
	ss.str(L"");
	ss.clear();
	for (int i = 0; i + 3 < raw_contents.length(); i += 4)
	{
		ss.clear();
		std::wstring substr = raw_contents.substr(i, 4);
		ss << std::hex << substr;
		ss >> hex_value;
		contents.push_back(hex_value);
	}

	//change this later!

	int type_start = contents.find(L"TYPE:");
	if (type_start != std::wstring::npos)
	{
		type_start = contents.find_first_not_of(' ', type_start + 5);
		int type_end = contents.find(L"\n", type_start);
		std::wstring type_str = contents.substr(type_start, type_end - type_start);
		if (type_str == L"REPORT")
			type = TYPE_REPORT;
		else if (type_str == L"ENGLISH")
			type = TYPE_REPORT_ENGLISH;
		else if (type_str == L"REFERRAL")
			type = TYPE_REFERRAL;
		else
			type = TYPE_UNKNOWN;
	}*/
}