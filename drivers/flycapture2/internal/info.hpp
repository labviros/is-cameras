#pragma once

// #include <algorithm>
#include <bitset>
#include <iomanip>
#include <is/wire/core/logger.hpp>
#include <string>
#include <vector>
#include "FlyCapture2.h"

namespace fc = FlyCapture2;

std::string make_ip_address(fc::IPAddress const& fc_ip);
std::string make_mac_address(fc::MACAddress fc_mac);
unsigned int make_subnet_mask(fc::IPAddress const& fc_subnet);
uint32_t make_link_speed(fc::BusSpeed const& bus_speed);