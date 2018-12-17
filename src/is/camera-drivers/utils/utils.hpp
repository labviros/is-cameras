#pragma once

#include <is/wire/core/status.hpp>
#include <is/wire/core/logger.hpp>
#include <string>

namespace is {
namespace camera {


template <typename T>
struct OpRange {
  OpRange() {}
  OpRange(T const& min, T const& max) : min(min), max(max) {}
  T min;
  T max;
  T to_ratio(T const& value) { return (value - min) / (max - min); }
  T to_value(T const& ratio) { return ratio * (max - min) + min; }
};

Status internal_error(StatusCode code, std::string const& why);
Status writeability_error(std::string const& name);
Status readability_error(std::string const& name);

}  // namespace camera
}  // namespace is