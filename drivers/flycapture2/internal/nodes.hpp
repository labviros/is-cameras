#pragma once

#include <is/msgs/common.pb.h>
#include <is/msgs/wire.pb.h>
#include <is/msgs/utils.hpp>
#include <is/wire/core/logger.hpp>
#include <string>
#include <tuple>

namespace is {
namespace camera {

using namespace is::wire;
using namespace is::common;

template <typename T>
struct OpRange {
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