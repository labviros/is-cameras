#pragma once

#include <boost/bimap.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <is/msgs/utils.hpp>
#include <is/wire/core/logger.hpp>
#include <string>
#include <vector>
#include "../camera-driver.hpp"
#include "SpinGenApi/SpinnakerGenApi.h"
#include "Spinnaker.h"

#define is_assert_ok(failable)                     \
  do {                                             \
    auto status = failable;                        \
    if (status.code() != is::wire::StatusCode::OK) \
      return status;                               \
  } while (0)

namespace is {
namespace camera {

using namespace is::wire;
using namespace boost::bimaps;

class SpinnakerDriver : public CameraDriver {
 public:
  SpinnakerDriver();

  static std::vector<CameraInfo> find_cameras();
  void connect(CameraInfo const& cam_info);
  void start_capture() override;
  void stop_capture() override;
  Image grab_image() override;
  pb::Timestamp last_timestamp() override;

  Status set_image_format(ImageFormat const& imgf) override;
  Status get_image_format(ImageFormat* imgf) override;
  Status set_sampling_rate(pb::FloatValue const& rate) override;
  Status get_sampling_rate(pb::FloatValue* rate) override;
  Status set_color_space(ColorSpace const& color_space) override;
  Status get_color_space(ColorSpace* color_space) override;
  Status set_resolution(Resolution const& resolution) override;
  Status get_resolution(Resolution* resolution) override;
  Status set_region_of_interest(BoundingPoly const& roi) override;
  Status get_region_of_interest(BoundingPoly* roi) override;
  Status set_delay(pb::FloatValue const& delay) override;
  Status get_delay(pb::FloatValue* delay) override;
  Status set_shutter(CameraSetting const& shutter) override;
  Status get_shutter(CameraSetting* shutter) override;
  Status set_gain(CameraSetting const& gain) override;
  Status get_gain(CameraSetting* gain) override;
  Status set_brightness(CameraSetting const& brightness) override;
  Status get_brightness(CameraSetting* brightness) override;
  Status set_white_balance(CameraSetting const& wb, std::string const& type);
  Status get_white_balance(CameraSetting* wb, std::string const& type);
  Status set_white_balance_bu(CameraSetting const& wb) override;
  Status get_white_balance_bu(CameraSetting* wb) override;
  Status set_white_balance_rv(CameraSetting const& wb) override;
  Status get_white_balance_rv(CameraSetting* wb) override;
  Status set_sharpness(CameraSetting const& sharpness) override;
  Status get_sharpness(CameraSetting* sharpness) override;
  Status set_gamma(CameraSetting const& gamma) override;
  Status get_gamma(CameraSetting* gamma) override;
  Status set_exposure(CameraSetting const& exposure) override;
  Status get_exposure(CameraSetting* exposure) override;
  Status set_hue(CameraSetting const&) override;
  Status get_hue(CameraSetting* hue) override;
  Status set_saturation(CameraSetting const&) override;
  Status get_saturation(CameraSetting* saturation) override;
  Status set_focus(CameraSetting const& focus) override;
  Status get_focus(CameraSetting* focus) override;
  Status set_zoom(CameraSetting const& zoom) override;
  Status get_zoom(CameraSetting* zoom) override;
  Status set_iris(CameraSetting const& iris) override;
  Status get_iris(CameraSetting* iris) override;

  Status set_packet_delay(int const& packet_delay) override;
  Status set_packet_size(int const& packet_size) override;
  Status reverse_x(bool enable) override;
  Status reverse_y(bool enable) override;

 private:
  struct camera {};
  struct gateway {};
  typedef bimap<tagged<is::camera::ColorSpaces, gateway>, tagged<std::string, camera>> ColorSpaceBimap;

  Spinnaker::SystemPtr cam_system;
  Spinnaker::CameraList cam_list;
  Spinnaker::CameraPtr cam;
  int sensor_width, sensor_height, max_binning_h, max_binning_v, step_h, step_v;
  std::string resolution_info;

  bool is_capturing;
  ImageFormat image_format;
  is::pb::Timestamp timestamp;

  ColorSpaceBimap color_space_map;

  template <typename F, typename P>
  Status control_capture(F&& function, P const& value) {
    auto keep_capturing = this->is_capturing;
    if (this->is_capturing)
      this->stop_capture();
    auto status = function(value);
    if (keep_capturing)
      this->start_capture();
    return status;
  }

  Spinnaker::GenApi::INodeMap& node_map() const;
  std::vector<int> get_compression_parm();
};

}  // namespace camera
}  // namespace is