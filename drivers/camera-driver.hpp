#pragma once

#include <google/protobuf/wrappers.pb.h>
#include <google/protobuf/util/time_util.h>
#include <is/msgs/camera.pb.h>
#include <is/msgs/common.pb.h>
#include <is/msgs/wire.pb.h>
#include <is/msgs/image.pb.h>
#include "../msgs/camera-info.pb.h"

namespace is {

namespace pb {
using namespace google::protobuf::util;
using namespace google::protobuf;
}  // namespace pb

namespace camera {

using namespace is::wire;
using namespace is::common;
using namespace is::vision;

struct CameraDriver {
  virtual ~CameraDriver() = default;
  static std::vector<CameraInfo> find_cameras();

  // // Image Settings
  virtual Status set_resolution(Resolution const&) = 0;
  virtual Status set_image_format(ImageFormat const&) = 0;
  virtual Status set_color_space(ColorSpace const&) = 0;
  virtual Status set_region_of_interest(BoundingPoly const&) = 0;
  // // Sampling Settings
  virtual Status set_sampling_rate(pb::FloatValue const&) = 0;
  virtual Status set_delay(pb::FloatValue const&) = 0;
  // // Camera Settings
  virtual Status set_brightness(CameraSetting const&) = 0;
  virtual Status set_exposure(CameraSetting const&) = 0;
  virtual Status set_focus(CameraSetting const& focus) = 0;
  virtual Status set_sharpness(CameraSetting const&) = 0;
  virtual Status set_hue(CameraSetting const&) = 0;
  virtual Status set_saturation(CameraSetting const&) = 0;
  virtual Status set_gamma(CameraSetting const&) = 0;
  virtual Status set_shutter(CameraSetting const&) = 0;
  virtual Status set_gain(CameraSetting const&) = 0;
  virtual Status set_white_balance_bu(CameraSetting const&) = 0;
  virtual Status set_white_balance_rv(CameraSetting const&) = 0;
  virtual Status set_zoom(CameraSetting const& zoom) = 0;
  virtual Status set_iris(CameraSetting const& iris) = 0;

  // // Image Settings
  virtual Status get_resolution(Resolution* resolution) = 0;
  virtual Status get_image_format(ImageFormat* resolution) = 0;
  virtual Status get_color_space(ColorSpace* resolution) = 0;
  virtual Status get_region_of_interest(BoundingPoly* resolution) = 0;
  // // Sampling Settings
  virtual Status get_sampling_rate(pb::FloatValue* sampling_rate) = 0;
  virtual Status get_delay(pb::FloatValue* delay) = 0;
  // // Camera Settings
  virtual Status get_brightness(CameraSetting* brightness) = 0;
  virtual Status get_exposure(CameraSetting* exposure) = 0;
  virtual Status get_focus(CameraSetting* focus) = 0;
  virtual Status get_sharpness(CameraSetting* sharpness) = 0;
  virtual Status get_hue(CameraSetting* hue) = 0;
  virtual Status get_saturation(CameraSetting* saturation) = 0;
  virtual Status get_gamma(CameraSetting* gamma) = 0;
  virtual Status get_shutter(CameraSetting* shutter) = 0;
  virtual Status get_gain(CameraSetting* gain) = 0;
  virtual Status get_white_balance_bu(CameraSetting* white_balance_bu) = 0;
  virtual Status get_white_balance_rv(CameraSetting* white_balance_rv) = 0;
  virtual Status get_zoom(CameraSetting* zoom) = 0;
  virtual Status get_iris(CameraSetting* iris) = 0;

  virtual Status set_packet_delay(int const& packet_delay) = 0;
  virtual Status set_packet_size(int const& packet_size) = 0;
  virtual Status reverse_x(bool enable) = 0;
  virtual Status reverse_y(bool enable) = 0;
  virtual pb::Timestamp last_timestamp() = 0;
  virtual Image grab_image() = 0;
  virtual void connect(CameraInfo const& cam_info) = 0;
  virtual void start_capture() = 0;
  virtual void stop_capture() = 0;
};  // CameraDriver

}  // namespace camera
}  // namespace is