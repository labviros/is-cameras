#pragma once

#include <is/msgs/common.pb.h>
#include <is/wire/core/status.hpp>
#include <is/msgs/utils.hpp>
#include <is/wire/core/logger.hpp>
#include <string>
#include "FlyCapture2.h"
#include "is/camera-drivers/utils/utils.hpp"

#define fc_assert_ok(error, type)                                                         \
  do {                                                                                    \
    if (error != fc::PGRERROR_OK) {                                                       \
      auto why = fmt::format("[{}] {}", get_property_name(type), error.GetDescription()); \
      return internal_error(StatusCode::INTERNAL_ERROR, why);                             \
    }                                                                                     \
  } while (0)

namespace is {
namespace camera {

using namespace is::common;
namespace fc = FlyCapture2;

Status set_gige_property(fc::GigECamera& camera, fc::GigEPropertyType type, int value);
Status set_property_auto(fc::GigECamera& camera, fc::PropertyType type);
Status get_property_auto(fc::GigECamera& camera, fc::PropertyType type, bool* is_auto);
Status set_property_abs(fc::GigECamera& camera, fc::PropertyType type, float value, bool is_ratio = false);
Status get_property_abs(fc::GigECamera& camera, fc::PropertyType type, float* value, bool is_ratio = false);
Status set_image_settings(fc::GigECamera& camera, fc::GigEImageSettings const& settings);
Status get_image_settings(fc::GigECamera& camera, fc::GigEImageSettings* settings);
std::string get_property_name(fc::PropertyType type);

}  // namespace camera
}  // namespace is