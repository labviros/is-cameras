#ifndef __CAMERA_DRIVER_HPP__
#define __CAMERA_DRIVER_HPP__

#include <is/msgs/common.pb.h>
#include <is/msgs/camera.pb.h>
#include <is/msgs/image.pb.h>

namespace is {
namespace camera {

using namespace is::common;
using namespace is::vision;

// This interface is required to be thread safe
struct CameraDriver {
  virtual ~CameraDriver() {}
  
  // Image Settings
  virtual void set_resolution(Resolution const&) = 0;
  virtual void set_image_format(ImageFormat const&) = 0;
  virtual void set_color_space(ColorSpace const&) = 0;
  virtual void set_region_of_interest(BoundingPoly const&) = 0;
  // Sampling Settings
  virtual void set_sampling_rate(float const&) = 0;
  virtual void set_delay(float const&) = 0;
  // Camera Settings
  virtual void set_brightness(CameraSetting const&) = 0;
  virtual void set_exposure(CameraSetting const&) = 0;
  virtual void set_sharpness(CameraSetting const&) = 0;
  virtual void set_hue(CameraSetting const&) = 0;
  virtual void set_saturation(CameraSetting const&) = 0;
  virtual void set_gamma(CameraSetting const&) = 0;
  virtual void set_shutter(CameraSetting const&) = 0;
  virtual void set_gain(CameraSetting const&) = 0;
  virtual void set_white_balance_bu(CameraSetting const&) = 0;
  virtual void set_white_balance_rv(CameraSetting const&) = 0;
  
  // Image Settings
  virtual Resolution get_resolution() = 0;
  virtual ImageFormat get_image_format() = 0;
  virtual ColorSpace get_color_space() = 0;
  virtual BoundingPoly get_region_of_interest() = 0;
  // Sampling Settings
  virtual float get_sampling_rate() = 0;
  virtual float get_delay() = 0;
  // Camera Settings
  virtual CameraSetting get_brightness() = 0;
  virtual CameraSetting get_exposure() = 0;
  virtual CameraSetting get_sharpness() = 0;
  virtual CameraSetting get_hue() = 0;
  virtual CameraSetting get_saturation() = 0;
  virtual CameraSetting get_gamma() = 0;
  virtual CameraSetting get_shutter() = 0;
  virtual CameraSetting get_gain() = 0;
  virtual CameraSetting get_white_balance_bu() = 0;
  virtual CameraSetting get_white_balance_rv() = 0;

  virtual Image grab_image() = 0;
  virtual void start_capture() = 0;
  virtual void stop_capture() = 0;
};  // CameraDriver

}  // camera
}  // is

#endif  // __CAMERA_DRIVER_HPP__
