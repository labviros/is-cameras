#ifndef __PTGREY_DRIVER_HPP__
#define __PTGREY_DRIVER_HPP__

#include "camera-driver.hpp"

#include <mutex>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <flycapture/FlyCapture2.h>
#include <opencv2/core.hpp>

namespace is {
namespace camera {

namespace fc = FlyCapture2;

class PtgreyDriver : public CameraDriver {
  fc::PGRGuid* uid;
  fc::GigECamera camera;

 public:
  PtgreyDriver() : uid(new fc::PGRGuid()) {}
  ~PtgreyDriver() { delete uid; }

  void connect(std::string const& ip_address) {
    fc::BusManager bus;

    std::vector<std::string> fields;
    boost::split(fields, ip_address, boost::is_any_of("."), boost::token_compress_on);
    assert(fields.size() == 4);

    int ip = 0;
    for (auto&& field : fields) {
      ip = ip << 8;
      ip |= std::stoi(field);
    }

    auto error = bus.GetCameraFromIPAddress(fc::IPAddress(ip), uid);
    if (error != fc::PGRERROR_OK)
      is::log::critical("{} {} {}", error.GetFilename(), error.GetLine(), error.GetDescription());

    error = camera.Connect(uid);
    if (error != fc::PGRERROR_OK)
      is::log::critical("{} {} {}", error.GetFilename(), error.GetLine(), error.GetDescription());

    set_gige_property(fc::PACKET_DELAY, 6000);
    set_gige_property(fc::PACKET_SIZE, 1400);
  }

  void start_capture() override { camera.StartCapture(); }

  void stop_capture() override { camera.StopCapture(); }

  cv::Mat grab() {
    fc::Image buffer;
    auto error = camera.RetrieveBuffer(&buffer);
    if (error != fc::PGRERROR_OK)
      is::log::warn("{} {} {}", error.GetFilename(), error.GetLine(), error.GetDescription());

    cv::Mat frame(buffer.GetRows(), buffer.GetCols(), CV_8UC1, buffer.GetData(),
                  buffer.GetDataSize() / buffer.GetRows());

    buffer.ReleaseBuffer();
    return frame;
  }

  void set_sampling_rate(SamplingRate const& sr) override {
    if (sr.period)
      set_property_abs(fc::FRAME_RATE, 1000.0f / sr.period.get());
    if (sr.rate)
      set_property_abs(fc::FRAME_RATE, sr.rate.get());
  }

  void set_delay(Delay const& delay) override {
    set_property_abs(fc::TRIGGER_DELAY, static_cast<float>(delay.milliseconds) / 1000.0f);
  }

  void set_resolution(msg::camera::Resolution const& resolution) override {
    // Changing the size of the image or the pixel encoding
    // format requires the camera to be stopped and restarted.
    fc::Error error;
    camera.StopCapture();
    int scale = 1288 / resolution.width;
    if (scale == 1) {
      error = camera.SetGigEImagingMode(fc::MODE_0);  // max resolution
    } else if (scale == 2) {
      error = camera.SetGigEImagingMode(fc::MODE_1);  // max resolution / 2
    } else {
      error = camera.SetGigEImagingMode(fc::MODE_5);  // max resolution / 4
    }
    camera.StartCapture();

    if (error != fc::PGRERROR_OK)
      internal_error("Resolution", error);
  }

  void set_color_space(msg::camera::ColorSpace const& color_space) override {
    fc::GigEImageSettings settings;
    auto error = camera.GetGigEImageSettings(&settings);
    if (error != fc::PGRERROR_OK)
      internal_error("Color Space", error);

    std::string type = color_space.value;
    boost::algorithm::to_lower_copy(type);
    if (type == "gray") {
      settings.pixelFormat = fc::PIXEL_FORMAT_MONO8;
    } else if (type == "rgb") {
      settings.pixelFormat = fc::PIXEL_FORMAT_BGR;
    } else {
      auto msg = fmt::format("Invalid type \"{}\". Valid types: \"rgb\" and \"gray\"", type);
      throw std::runtime_error(msg);
    }

    camera.StopCapture();
    error = camera.SetGigEImageSettings(&settings);
    camera.StartCapture();

    if (error != fc::PGRERROR_OK)
      internal_error("Color Space", error);
  }

  void set_region_of_interest(msg::camera::RegionOfInterest const& roi) override {
    fc::GigEImageSettingsInfo info;
    auto error = camera.GetGigEImageSettingsInfo(&info);
    if (error != fc::PGRERROR_OK)
      internal_error("Region of Interest", error);

    fc::GigEImageSettings settings;
    error = camera.GetGigEImageSettings(&settings);
    if (error != fc::PGRERROR_OK)
      internal_error("Region of Interest", error);

    settings.offsetX = roi.x_offset;
    settings.offsetY = roi.y_offset;
    settings.height = std::min(info.maxHeight, roi.height);
    settings.width = std::min(info.maxWidth, roi.width);

    camera.StopCapture();
    error = camera.SetGigEImageSettings(&settings);
    camera.StartCapture();

    if (error != fc::PGRERROR_OK)
      internal_error("Region of Interest", error);
  }

  void set_exposure(msg::camera::Exposure const& exposure) override {
    if (exposure.percent)
      set_property_abs(fc::AUTO_EXPOSURE, exposure.percent.get(), /*is_ratio*/ true);
    if (exposure.ev)
      set_property_abs(fc::AUTO_EXPOSURE, exposure.ev.get());
    if (exposure.auto_mode && exposure.auto_mode.get() == true)
      set_property_auto(fc::AUTO_EXPOSURE);
  }

  void set_gain(msg::camera::Gain const& gain) override {
    if (gain.percent)
      set_property_abs(fc::GAIN, gain.percent.get(), /*is_ratio*/ true);
    if (gain.db)
      set_property_abs(fc::GAIN, gain.db.get());
    if (gain.auto_mode && gain.auto_mode.get() == true)
      set_property_auto(fc::GAIN);
  }

  void set_shutter(msg::camera::Shutter const& shutter) override {
    if (shutter.percent)
      set_property_abs(fc::SHUTTER, shutter.percent.get(), /*is_ratio*/ true);
    if (shutter.ms)
      set_property_abs(fc::SHUTTER, shutter.ms.get());
    if (shutter.auto_mode && shutter.auto_mode.get() == true)
      set_property_auto(fc::SHUTTER);
  }

  void set_brightness(msg::camera::Brightness const&) override {}
  void set_white_balance(msg::camera::WhiteBalance const&) override {}

  SamplingRate get_sampling_rate() override {
    auto property = get_property(fc::FRAME_RATE);
    SamplingRate sr;
    sr.rate = property.absValue;
    return sr;
  }

  // virtual Expected<Resolution> get_resolution() = 0;
  // virtual Expected<Exposure> get_exposure() = 0;
  // virtual Expected<Gain> get_gain() = 0;
  // virtual Expected<ImageType> get_image_type() = 0;
  // virtual Expected<RegionOfInterest> get_region_of_interest() = 0;
  // virtual Expected<Shutter> get_shutter() = 0;
  // virtual Expected<WhiteBalance> get_white_balance() = 0;

 private:
  /**** GiGeProperties
   *
   * OnOff - Property is controllable if true
   * AutoManualMode - Camera control the feature by itself if true
   * OnePush - Camera control feature once then return to manual mode if true
   *
   ****/

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

  void internal_error(std::string const& id, fc::Error const& error) {
    auto error_msg = fmt::format("[{}] {}", id, error.GetDescription());
    is::log::warn(error_msg);
    throw std::runtime_error(error_msg);
  }

  void internal_error(fc::PropertyType type, fc::Error const& error) {
    auto error_msg = fmt::format("[{}] {}", get_property_name(type), error.GetDescription());
    is::log::warn(error_msg);
    throw std::runtime_error(error_msg);
  }

  void set_gige_property(fc::GigEPropertyType type, int value) {
    fc::GigEProperty property;
    property.propType = type;
    property.value = value;

    auto error = camera.SetGigEProperty(&property);
    /* IMPLEMENT ERROR HANDLING */
  }

  fc::Property get_property(fc::PropertyType type) {
    fc::Property property(type);
    auto error = camera.GetProperty(&property);
    if (error != fc::PGRERROR_OK)
      internal_error(type, error);
    return property;
  }

  void set_property_abs(fc::PropertyType type, float value, bool is_ratio = false) {
    fc::PropertyInfo info(type);
    auto error = camera.GetPropertyInfo(&info);
    if (error != fc::PGRERROR_OK)
      internal_error(type, error);

    if (is_ratio) {
      value = (info.absMax - info.absMin) * value + info.absMin;
    }

    if (value > info.absMax || value < info.absMin)
      internal_error(type, error);

    fc::Property property(type);
    property.absValue = value;
    property.onOff = true;
    property.absControl = true;
    property.autoManualMode = false;

    error = camera.SetProperty(&property);
    if (error != fc::PGRERROR_OK)
      internal_error(type, error);
  }

  void set_property_auto(fc::PropertyType type) {
    fc::Property property(type);
    property.onOff = true;
    property.autoManualMode = true;

    auto error = camera.SetProperty(&property);
    if (error != fc::PGRERROR_OK)
      internal_error(type, error);
  }

};  // PtgreyDriver

}  // camera
}  // is

#endif  // __PTGREY_DRIVER_HPP__
