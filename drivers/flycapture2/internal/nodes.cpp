#include "nodes.hpp"

namespace is {
namespace camera {

using namespace is::common;

Status set_gige_property(fc::GigECamera& camera, fc::GigEPropertyType type, int value) {
  fc::GigEProperty property;
  property.propType = type;
  property.value = value;

  auto error = camera.SetGigEProperty(&property);
  if (error != fc::PGRERROR_OK) {
    auto why = fmt::format("[GigE Property] {}", error.GetDescription());
    // return internal_error(StatusCode::INTERNAL_ERROR, why);
    return is::make_status(StatusCode::OK);
  }
  return is::make_status(StatusCode::OK);
}

Status set_property_abs(fc::GigECamera& camera, fc::PropertyType type, float value, bool is_ratio) {
  fc::PropertyInfo info(type);
  auto error = camera.GetPropertyInfo(&info);
  fc_assert_ok(error, type);
  if (is_ratio) {
    OpRange<float> range(info.absMin, info.absMax);
    value = range.to_value(value);
  }

  if (value > info.absMax || value < info.absMin) {
    auto msg = fmt::format("On {} property, value {} out of range. Current range: [{},{}]", get_property_name(type),
                           value, info.absMin, info.absMax);
    return internal_error(StatusCode::OUT_OF_RANGE, msg);
  }

  fc::Property property(type);
  property.absValue = value;
  property.onOff = true;
  property.absControl = true;
  property.autoManualMode = false;

  error = camera.SetProperty(&property);
  fc_assert_ok(error, type);
  return is::make_status(StatusCode::OK);
}

Status get_property_abs(fc::GigECamera& camera, fc::PropertyType type, float* value, bool is_ratio) {
  fc::Property property(type);
  auto error = camera.GetProperty(&property);
  fc_assert_ok(error, type);

  *value = property.absValue;
  if (is_ratio) {
    fc::PropertyInfo info(type);
    auto error = camera.GetPropertyInfo(&info);
    fc_assert_ok(error, type);
    OpRange<float> range(info.absMin, info.absMax);
    *value = range.to_ratio(*value);
  }
  return is::make_status(StatusCode::OK);
}

Status set_image_settings(fc::GigECamera& camera, fc::GigEImageSettings const& settings) {
  auto error = camera.SetGigEImageSettings(&settings);
  if (error != fc::PGRERROR_OK) {
    auto why = fmt::format("[SetImageSettings] {}", error.GetDescription());
    return internal_error(StatusCode::INTERNAL_ERROR, why);
  }
  return is::make_status(StatusCode::OK);
}

Status get_image_settings(fc::GigECamera& camera, fc::GigEImageSettings* settings) {
  auto error = camera.GetGigEImageSettings(settings);
  if (error != fc::PGRERROR_OK) {
    auto why = fmt::format("[GetImageSettings] {}", error.GetDescription());
    return internal_error(StatusCode::INTERNAL_ERROR, why);
  }
  return is::make_status(StatusCode::OK);
}

std::string get_property_name(fc::PropertyType type) {
  switch (type) {
  case fc::BRIGHTNESS: return "Brightness";
  case fc::AUTO_EXPOSURE: return "Exposure";
  case fc::SHARPNESS: return "Sharpness";
  case fc::WHITE_BALANCE: return "White Balance";
  case fc::HUE: return "Hue";
  case fc::SATURATION: return "Saturation";
  case fc::GAMMA: return "Gamma";
  case fc::SHUTTER: return "Shutter";
  case fc::GAIN: return "Gain";
  case fc::TRIGGER_DELAY: return "Delay";
  case fc::FRAME_RATE: return "Sampling Rate";
  default: return "Unknown Property";
  }
}

}  // namespace camera
}  // namespace is