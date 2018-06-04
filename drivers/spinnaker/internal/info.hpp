#pragma once

#include <algorithm>
#include <bitset>
#include <iomanip>
#include <is/wire/core/logger.hpp>
#include <string>
#include <vector>

std::vector<std::string> split_address(std::string address);
std::string make_ip_address(int64_t integer_ip);
std::string make_mac_address(int64_t integer_mac);
unsigned int make_subnet_mask(int64_t integer_mask);