#include "nodes.hpp"

namespace is {
namespace camera {

using namespace is::common;
namespace spn {
using namespace Spinnaker;
using namespace Spinnaker::GenApi;
using namespace Spinnaker::GenICam;
}  // namespace spn

bool is_writable(spn::INode* node) {
  if (!spn::IsAvailable(node))
    is::warn("[{}] Not available.", node->GetName());
  if (!spn::IsWritable(node))
    is::warn("[{}] Not writable.", node->GetName());
  return spn::IsAvailable(node) && spn::IsAvailable(node);
}

bool is_readable(spn::INode* node) {
  if (!spn::IsAvailable(node))
    is::warn("[{}] Not available.", node->GetName());
  if (!spn::IsReadable(node))
    is::warn("[{}] Not readable.", node->GetName());
  return spn::IsReadable(node);
}

Status set_op_bool(spn::INodeMap& node_map, std::string const& name, bool value) {
  spn::CBooleanPtr prop = node_map.GetNode(name.c_str());
  if (!is_writable(prop))
    return writeability_error(name);
  prop->SetValue(value);
  return is::make_status(StatusCode::OK);
}

Status get_op_bool(spn::INodeMap& node_map, std::string const& name, bool* value) {
  spn::CBooleanPtr prop = node_map.GetNode(name.c_str());
  if (!is_readable(prop))
    return readability_error(name);
  *value = prop->GetValue();
  return is::make_status(StatusCode::OK);
}

Status set_op_enum(spn::INodeMap& node_map, std::string const& name, std::string const& value) {
  spn::CEnumerationPtr prop = node_map.GetNode(name.c_str());
  if (!is_writable(prop))
    return writeability_error(name);
  spn::CEnumEntryPtr entry = prop->GetEntryByName(value.c_str());
  if (!is_readable(entry))
    return readability_error(fmt::format("{}->{}", name, value));
  prop->SetIntValue(entry->GetValue());
  return is::make_status(StatusCode::OK);
}

Status get_op_enum(spn::INodeMap& node_map, std::string const& name, std::string* value) {
  spn::CEnumerationPtr prop = node_map.GetNode(name.c_str());
  if (!is_readable(prop))
    return readability_error(name);
  auto v = prop->GetCurrentEntry()->GetSymbolic();
  *value = std::string(v.c_str());
  return is::make_status(StatusCode::OK);
}

Status set_op_int(spn::INodeMap& node_map, std::string const& name, int64_t value) {
  spn::CIntegerPtr prop = node_map.GetNode(name.c_str());
  if (!is_writable(prop))
    return writeability_error(name);
  auto pmin = prop->GetMin();
  auto pmax = prop->GetMax();
  if (value < pmin || value > pmax) {
    auto why = fmt::format("[{}] Value {} out of range. Current range: [{},{}]", name, value, pmin, pmax);
    return internal_error(StatusCode::OUT_OF_RANGE, why);
  }
  prop->SetValue(value);
  return is::make_status(StatusCode::OK);
}

Status get_op_int(spn::INodeMap& node_map, std::string const& name, int64_t* value) {
  spn::CIntegerPtr prop = node_map.GetNode(name.c_str());
  if (!is_readable(prop))
    return readability_error(name);
  *value = prop->GetValue();
  return is::make_status(StatusCode::OK);
}

Status minmax_op_int(spn::INodeMap& node_map, std::string const& name, OpRange<int64_t>* range) {
  spn::CIntegerPtr prop = node_map.GetNode(name.c_str());
  if (!is_readable(prop))
    return readability_error(name);
  range->min = prop->GetMin();
  range->max = prop->GetMax();
  return is::make_status(StatusCode::OK);
}

Status set_op_float(spn::INodeMap& node_map, std::string const& name, float value) {
  spn::CFloatPtr prop = node_map.GetNode(name.c_str());
  if (!is_writable(prop))
    return writeability_error(name);
  auto pmin = prop->GetMin();
  auto pmax = prop->GetMax();
  if (value < pmin || value > pmax) {
    auto why = fmt::format("[{}] Value {} out of range. Current range: [{},{}]", name, value, pmin, pmax);
    return internal_error(StatusCode::OUT_OF_RANGE, why);
  }
  prop->SetValue(value);
  return is::make_status(StatusCode::OK);
}

Status get_op_float(spn::INodeMap& node_map, std::string const& name, float* value) {
  spn::CFloatPtr prop = node_map.GetNode(name.c_str());
  if (!is_readable(prop))
    return readability_error(name);
  *value = prop->GetValue();
  return is::make_status(StatusCode::OK);
}

Status minmax_op_float(spn::INodeMap& node_map, std::string const& name, OpRange<float>* range) {
  spn::CFloatPtr prop = node_map.GetNode(name.c_str());
  if (!is_readable(prop))
    return readability_error(name);
  range->min = prop->GetMin(); 
  range->max = prop->GetMax();
  return is::make_status(StatusCode::OK);
}

Status set_op_str(spn::INodeMap& node_map, std::string const& name, std::string const& value) {
  spn::CStringPtr prop = node_map.GetNode(name.c_str());
  if (!is_writable(prop))
    return writeability_error(name);
  prop->SetValue(value.c_str());
  return is::make_status(StatusCode::OK);
}

Status get_op_str(spn::INodeMap& node_map, std::string const& name, std::string* value) {
  spn::CStringPtr prop = node_map.GetNode(name.c_str());
  if (!is_readable(prop))
    readability_error(name);
  *value = std::string(prop->GetValue());
  return is::make_status(StatusCode::OK);
}

Status execute_op(spn::INodeMap& node_map, std::string const& name) {
  spn::CCommandPtr prop = node_map.GetNode(name.c_str());
  prop->Execute();
  return is::make_status(StatusCode::OK);
}

}  // namespace camera
}  // namespace is