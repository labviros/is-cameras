#include "info.hpp"

std::string make_ip_address(fc::IPAddress const& fc_ip) {
  return fmt::format("{}.{}.{}.{}", fc_ip.octets[0], fc_ip.octets[1], fc_ip.octets[2], fc_ip.octets[3]);
}

std::string make_mac_address(fc::MACAddress fc_mac) {
  return fmt::format("{:02X}:{:02X}:{:02X}:{:02X}:{:02X}:{:02X}", fc_mac.octets[0], fc_mac.octets[1], fc_mac.octets[2],
                     fc_mac.octets[3], fc_mac.octets[4], fc_mac.octets[5]);
}

unsigned int make_subnet_mask(fc::IPAddress const& fc_subnet) {
  auto mask = fmt::format("{:#x}{:02x}{:02x}{:02x}", fc_subnet.octets[0], fc_subnet.octets[1], fc_subnet.octets[2],
                          fc_subnet.octets[3]);
  std::bitset<32> bit_mask = std::stoul(mask, nullptr, 16);
  return bit_mask.count();
}

uint32_t make_link_speed(fc::BusSpeed const& bus_speed) {
  switch (bus_speed) {
  case fc::BusSpeed::BUSSPEED_S100: return 100;
  case fc::BusSpeed::BUSSPEED_S200: return 200;
  case fc::BusSpeed::BUSSPEED_S400: return 400;
  case fc::BusSpeed::BUSSPEED_S480: return 480;
  case fc::BusSpeed::BUSSPEED_S800: return 800;
  case fc::BusSpeed::BUSSPEED_S1600: return 1600;
  case fc::BusSpeed::BUSSPEED_S3200: return 3200;
  case fc::BusSpeed::BUSSPEED_S5000: return 5000;
  case fc::BusSpeed::BUSSPEED_10BASE_T: return 10;
  case fc::BusSpeed::BUSSPEED_100BASE_T: return 100;
  case fc::BusSpeed::BUSSPEED_1000BASE_T: return 1000;
  case fc::BusSpeed::BUSSPEED_10000BASE_T: return 10000;
  case fc::BusSpeed::BUSSPEED_S_FASTEST:
  case fc::BusSpeed::BUSSPEED_ANY:
  case fc::BusSpeed::BUSSPEED_SPEED_UNKNOWN: return 0;
  }
}
