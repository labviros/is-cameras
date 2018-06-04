#include "info.hpp"

std::vector<std::string> split_address(std::string address) {
  assert(address.size() == 0 || address.size() == 10 ||
         address.size() == 14);  // check if is IP or MAC address or empty
  std::vector<std::string> fields;
  if (address.size() > 0) {
    std::transform(address.begin(), address.end(), address.begin(), ::toupper);
    for (auto it = address.begin() + 2; it != address.end(); it += 2) {
      fields.push_back(std::string(it, it + 2));
    }
  }
  return fields;
}

std::string make_ip_address(int64_t integer_ip) {
  auto address = fmt::format("0x{:08x}", integer_ip);
  auto fields = split_address(address);
  if (fields.size() == 0)
    return "0.0.0.0";
  auto hex_to_intstr = [](auto s) { return std::to_string(std::stoul("0x" + s, nullptr, 16)); };
  return std::accumulate(fields.begin() + 1, fields.end(), hex_to_intstr(*fields.begin()),
                         [&](auto a, auto b) { return a + "." + hex_to_intstr(b); });
}

std::string make_mac_address(int64_t integer_mac) {
  auto address = fmt::format("0x{:012x}", integer_mac);
  auto fields = split_address(address);
  if (fields.size() == 0)
    return "00:00:00:00:00:00";
  return std::accumulate(fields.begin() + 1, fields.end(), *fields.begin(), [](auto a, auto b) { return a + ":" + b; });
}

unsigned int make_subnet_mask(int64_t integer_mask) {
  auto mask = fmt::format("{:#x}", integer_mask);
  std::bitset<32> bit_mask = std::stoul(mask, nullptr, 16);
  return bit_mask.count();
}