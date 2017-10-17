#ifndef __CAMERA_DRIVER_HPP__
#define __CAMERA_DRIVER_HPP__

#include "./msgs/camera.hpp"

namespace is {
namespace camera {

using namespace msg::common;
using namespace msg::camera;

struct CameraDriver {
  virtual ~CameraDriver() {}

  virtual void set_sampling_rate(SamplingRate const&) = 0;
  virtual void set_delay(Delay const&) = 0;
  virtual void set_resolution(Resolution const&) = 0;
  virtual void set_exposure(Exposure const&) = 0;
  virtual void set_gain(Gain const&) = 0;
  virtual void set_color_space(ColorSpace const&) = 0;
  virtual void set_region_of_interest(RegionOfInterest const&) = 0;
  virtual void set_shutter(Shutter const&) = 0;
  virtual void set_white_balance(WhiteBalance const&) = 0;
  virtual void set_brightness(Brightness const&) = 0;

  virtual SamplingRate get_sampling_rate() = 0;
  // virtual Expected<Delay> get_delay() = 0;
  // virtual Expected<Resolution> get_resolution() = 0;
  // virtual Expected<Exposure> get_exposure() = 0;
  // virtual Expected<Gain> get_gain() = 0;
  // virtual Expected<ImageType> get_image_type() = 0;
  // virtual Expected<RegionOfInterest> get_region_of_interest() = 0;
  // virtual Expected<Shutter> get_shutter() = 0;
  // virtual Expected<WhiteBalance> get_white_balance() = 0;

  virtual void start_capture() = 0;
  virtual void stop_capture() = 0;
};  // DriverInterface

}  // camera
}  // is

#endif  // __CAMERA_DRIVER_HPP__
