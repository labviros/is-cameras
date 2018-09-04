#include "nodes.hpp"

namespace is {
namespace camera {

using namespace is::wire;
using namespace is::common;

Status internal_error(StatusCode code, std::string const& why) {
  is::warn(why);
  return is::make_status(code, why);
}

Status writeability_error(std::string const& name) {
  auto why = fmt::format("[{}] Not available or not writable", name);
  return is::make_status(StatusCode::PERMISSION_DENIED, why);
}

Status readability_error(std::string const& name) {
  auto why = fmt::format("[{}] Not available or not readable", name);
  return is::make_status(StatusCode::PERMISSION_DENIED, why);
}

}  // namespace camera
}  // namespace is