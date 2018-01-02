#ifndef __PTGREY_DRIVER_HPP__
#define __PTGREY_DRIVER_HPP__

#include <is/is.hpp>
#include "camera-driver.hpp"

#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <flycapture/FlyCapture2.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

namespace is {
namespace camera {

namespace fc = FlyCapture2;
using namespace is::common;
using namespace is::vision;

class PtgreyDriver : public CameraDriver {
  fc::PGRGuid* uid;
  fc::GigECamera camera;

  ImageFormat image_format;
  fc::PixelFormat pixel_format;
  bool is_capturing;
  is::pb::Timestamp timestamp;

 public:
  PtgreyDriver() : uid(new fc::PGRGuid()), is_capturing(false) {}
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
      is::critical("{} {} {}", error.GetFilename(), error.GetLine(), error.GetDescription());

    error = camera.Connect(uid);
    if (error != fc::PGRERROR_OK)
      is::critical("{} {} {}", error.GetFilename(), error.GetLine(), error.GetDescription());

    ColorSpace color_space;
    color_space.set_value(ColorSpaces::GRAY);
    set_color_space(color_space);
    ImageFormat image_format;
    image_format.set_format(ImageFormats::JPEG);
    set_image_format(image_format);
    set_sampling_rate(5.0);
  }

  Image grab_image() override {
    fc::Image image;
    Defer clean_image([&] { image.ReleaseBuffer(); });
    auto error = camera.RetrieveBuffer(&image);
    if (error != fc::PGRERROR_OK)
      is::warn("[Grab Image] {}", error.GetDescription());

    timestamp = is::current_time();

    fc::Image buffer;
    Defer clean_buffer([&] {
      if (pixel_format == fc::PIXEL_FORMAT_BGR)
        buffer.ReleaseBuffer();
    });
    cv::Mat frame;
    if (pixel_format == fc::PIXEL_FORMAT_MONO8) {
      frame = cv::Mat(image.GetRows(), image.GetCols(), CV_8UC1, image.GetData(),
                      image.GetDataSize() / image.GetRows());
    } else if (pixel_format == fc::PIXEL_FORMAT_RGB8) {
      error = image.Convert(fc::PIXEL_FORMAT_BGR, &buffer);
      if (error != fc::PGRERROR_OK)
        internal_error("Grab Image", error);

      frame = cv::Mat(buffer.GetRows(), buffer.GetCols(), CV_8UC3, buffer.GetData(),
                      buffer.GetDataSize() / buffer.GetRows());
    } else {
      throw std::runtime_error("[Grab Image] Bad image type");
    }

    std::vector<unsigned char> image_data;
    cv::imencode(fmt::format(".{}", ImageFormats_Name(image_format.format())), frame, image_data);
    Image compressed;
    auto compressed_data = compressed.mutable_data();
    compressed_data->resize(image_data.size());
    std::copy(image_data.begin(), image_data.end(), compressed_data->begin());
    return compressed;
  }

  pb::Timestamp last_timestamp() override { return this->timestamp; }

  void start_capture() override {
    auto error = camera.StartCapture();
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, "Start Capture", error);
    is_capturing = true;
  }

  void stop_capture() override {
    auto error = camera.StopCapture();
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, "Stop Capture", error);
    is_capturing = false;
  }

  void set_sampling_rate(float const& rate) override { set_property_abs(fc::FRAME_RATE, rate); }

  void set_delay(float const& delay) override { set_property_abs(fc::TRIGGER_DELAY, delay); }

  void set_resolution(Resolution const& resolution) override {
    // Changing the size of the image or the pixel encoding
    // format requires the camera to be stopped and restarted.
    auto keep_capturing = is_capturing;
    if (is_capturing)
      stop_capture();
    fc::Error error;
    int scale = 1288 / resolution.width();
    if (scale == 1) {
      error = camera.SetGigEImagingMode(fc::MODE_0);  // max resolution
    } else if (scale == 2) {
      error = camera.SetGigEImagingMode(fc::MODE_1);  // max resolution / 2
    } else {
      error = camera.SetGigEImagingMode(fc::MODE_5);  // max resolution / 4
    }
    if (keep_capturing)
      start_capture();
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INVALID_ARGUMENT, "Resolution", error);
  }

  void set_color_space(ColorSpace const& cs) override {
    auto color_space = cs.value();
    fc::GigEImageSettings settings;
    auto error = camera.GetGigEImageSettings(&settings);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, "Color Space", error);

    if (to_pixel_format.find(color_space) == to_pixel_format.end()) {
      auto msg = fmt::format("Invalid type \"{}\". Valid types: \"RGB\" and \"GRAY\"",
                             ColorSpaces_Name(color_space));
      internal_error(StatusCode::INVALID_ARGUMENT, msg);
    }

    settings.pixelFormat = to_pixel_format[color_space];
    error = set_gige_settings(settings);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INVALID_ARGUMENT, "Color Space", error);

    pixel_format = settings.pixelFormat;
  }

  void set_image_format(ImageFormat const& imgf) override { image_format = imgf; }

  void set_region_of_interest(BoundingPoly const& roi) override {
    auto n_verticies = roi.vertices_size();
    if (n_verticies < 2)
      internal_error(StatusCode::INVALID_ARGUMENT,
                     "Region of Interest must have at least 2 vertices");
    if (n_verticies > 2)
      internal_error(StatusCode::UNIMPLEMENTED,
                     "Funtionality implemented just for BoundingPoly with 2 vertices");

    fc::GigEImageSettingsInfo info;
    auto error = camera.GetGigEImageSettingsInfo(&info);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, "Region of Interest", error);

    fc::GigEImageSettings settings;
    error = camera.GetGigEImageSettings(&settings);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, "Region of Interest", error);

    auto top_left = roi.vertices(0);
    auto bottom_right = roi.vertices(1);
    auto width = static_cast<unsigned int>(bottom_right.x() - top_left.x());
    auto height = static_cast<unsigned int>(bottom_right.y() - top_left.y());
    settings.offsetX = top_left.x();
    settings.offsetY = top_left.y();
    settings.width = std::min(info.maxWidth, width);
    settings.height = std::min(info.maxHeight, height);

    error = set_gige_settings(settings);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INVALID_ARGUMENT, "Region of Interest", error);
  }

  void set_packet_delay(int const& packet_delay) {
    this->set_gige_property(fc::PACKET_DELAY, packet_delay);
  }

  void set_packet_size(int const& packet_size) {
    this->set_gige_property(fc::PACKET_SIZE, packet_size);
  }

  void set_exposure(CameraSetting const& exposure) override {
    /*
    if (exposure.percent)
      set_property_abs(fc::AUTO_EXPOSURE, exposure.percent.get(), true);
    if (exposure.ev)
      set_property_abs(fc::AUTO_EXPOSURE, exposure.ev.get());
    if (exposure.auto_mode && exposure.auto_mode.get() == true)
      set_property_auto(fc::AUTO_EXPOSURE);
    */
  }

  void set_gain(CameraSetting const& gain) override {
    /*
    if (gain.percent)
      set_property_abs(fc::GAIN, gain.percent.get(), true);
    if (gain.db)
      set_property_abs(fc::GAIN, gain.db.get());
    if (gain.auto_mode && gain.auto_mode.get() == true)
      set_property_auto(fc::GAIN);
    */
  }

  void set_shutter(CameraSetting const& shutter) override {
    /*
    if (shutter.percent)
      set_property_abs(fc::SHUTTER, shutter.percent.get(), true);
    if (shutter.ms)
      set_property_abs(fc::SHUTTER, shutter.ms.get());
    if (shutter.auto_mode && shutter.auto_mode.get() == true)
      set_property_auto(fc::SHUTTER);
    */
  }

  void set_brightness(CameraSetting const&) override {}
  void set_sharpness(CameraSetting const&) override {}
  void set_hue(CameraSetting const&) override {}
  void set_saturation(CameraSetting const&) override {}
  void set_gamma(CameraSetting const&) override {}
  void set_white_balance_bu(CameraSetting const&) override {}
  void set_white_balance_rv(CameraSetting const&) override {}

  ColorSpace get_color_space() override {
    ColorSpace color_space;
    try {
      color_space.set_value(to_color_space.at(pixel_format));
    } catch (...) { internal_error(StatusCode::OUT_OF_RANGE, "Color Space not recognized"); }
    return color_space;
  }

  Resolution get_resolution() override {
    fc::Mode mode;
    auto error = camera.GetGigEImagingMode(&mode);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, "Resolution", error);
    Resolution resolution;
    if (mode == fc::MODE_0) {
      resolution.set_width(1288);
      resolution.set_height(728);
    } else if (mode == fc::MODE_1) {
      resolution.set_width(1288 / 2);
      resolution.set_height(728 / 2);
    } else if (mode == fc::MODE_5) {
      resolution.set_width(1288 / 4);
      resolution.set_height(728 / 4);
    } else {
      internal_error(StatusCode::INVALID_ARGUMENT, "Invalid camera mode received");
    }
    return resolution;
  }

  BoundingPoly get_region_of_interest() override {
    fc::GigEImageSettings settings;
    auto error = camera.GetGigEImageSettings(&settings);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, "Region of Interest", error);

    BoundingPoly roi;
    auto top_left = roi.add_vertices();
    top_left->set_x(settings.offsetX);
    top_left->set_y(settings.offsetY);
    auto bottom_right = roi.add_vertices();
    bottom_right->set_x(settings.offsetX + settings.width);
    bottom_right->set_y(settings.offsetY + settings.height);
    return roi;
  }

  float get_sampling_rate() override {
    auto property = get_property(fc::FRAME_RATE);
    return property.absValue;
  }

  float get_delay() override {
    auto property = get_property(fc::TRIGGER_DELAY);
    return property.absValue;
  }

  ImageFormat get_image_format() override { return image_format; }

  CameraSetting get_brightness() override {
    CameraSetting x;
    return x;
  }

  CameraSetting get_exposure() override {
    CameraSetting x;
    return x;
  }

  CameraSetting get_sharpness() override {
    CameraSetting x;
    return x;
  }

  CameraSetting get_hue() override {
    CameraSetting x;
    return x;
  }

  CameraSetting get_saturation() override {
    CameraSetting x;
    return x;
  }

  CameraSetting get_gamma() override {
    CameraSetting x;
    return x;
  }

  CameraSetting get_shutter() override {
    CameraSetting x;
    return x;
  }

  CameraSetting get_gain() override {
    CameraSetting x;
    return x;
  }

  CameraSetting get_white_balance_bu() override {
    CameraSetting x;
    return x;
  }

  CameraSetting get_white_balance_rv() override {
    CameraSetting x;
    return x;
  }

 private:
  /**** GiGeProperties
   *
   * OnOff - Property is controllable if true
   * AutoManualMode - Camera control the feature by itself if true
   * OnePush - Camera control feature once then return to manual mode if true
   *
   ****/
  struct Defer {
    std::function<void()> on_exit;
    Defer(std::function<void()>&& f) noexcept : on_exit(std::move(f)) {}
    Defer(Defer const&) = delete;
    Defer() = default;
    Defer(Defer&& defer) : on_exit(std::move(defer.on_exit)) {}
    ~Defer() { on_exit(); }
  };

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

  void internal_error(StatusCode code, std::string const& why) {
    is::warn(why);
    throw is::make_status(code, why);
  }

  void internal_error(StatusCode code, std::string const& why, fc::Error const& error) {
    auto error_msg = fmt::format("[{}] {}", why, error.GetDescription());
    is::warn(error_msg);
    throw is::make_status(code, error_msg);
  }

  void internal_error(StatusCode code, fc::PropertyType type, fc::Error const& error) {
    auto error_msg = fmt::format("[{}] {}", get_property_name(type), error.GetDescription());
    is::warn(error_msg);
    throw is::make_status(code, error_msg);
  }

  void internal_error(std::string const& id, fc::Error const& error) {
    auto error_msg = fmt::format("[{}] {}", id, error.GetDescription());
    is::warn(error_msg);
    throw std::runtime_error(error_msg);
  }

  void internal_error(fc::PropertyType type, fc::Error const& error) {
    auto error_msg = fmt::format("[{}] {}", get_property_name(type), error.GetDescription());
    is::warn(error_msg);
    throw std::runtime_error(error_msg);
  }

  fc::Error set_gige_settings(fc::GigEImageSettings const& settings) {
    auto keep_capturing = is_capturing;
    if (is_capturing)
      stop_capture();
    auto error = camera.SetGigEImageSettings(&settings);
    if (keep_capturing)
      start_capture();
    return error;
  }

  void set_gige_property(fc::GigEPropertyType type, int value) {
    fc::GigEProperty property;
    property.propType = type;
    property.value = value;

    auto error = camera.SetGigEProperty(&property);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, "GigE Property", error);
  }

  fc::Property get_property(fc::PropertyType type) {
    fc::Property property(type);
    auto error = camera.GetProperty(&property);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, type, error);
    return property;
  }

  void set_property_abs(fc::PropertyType type, float value, bool is_ratio = false) {
    fc::PropertyInfo info(type);
    auto error = camera.GetPropertyInfo(&info);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, type, error);

    if (is_ratio) {
      value = (info.absMax - info.absMin) * value + info.absMin;
    }

    if (value > info.absMax || value < info.absMin) {
      auto msg = fmt::format("On {} property, value {} out of range. Current range: [{},{}]",
                             get_property_name(type), value, info.absMin, info.absMax);
      internal_error(StatusCode::OUT_OF_RANGE, msg);
    }

    fc::Property property(type);
    property.absValue = value;
    property.onOff = true;
    property.absControl = true;
    property.autoManualMode = false;

    error = camera.SetProperty(&property);
    if (error != fc::PGRERROR_OK)
      internal_error(StatusCode::INTERNAL_ERROR, type, error);
  }

  void set_property_auto(fc::PropertyType type) {
    fc::Property property(type);
    property.onOff = true;
    property.autoManualMode = true;

    auto error = camera.SetProperty(&property);
    if (error != fc::PGRERROR_OK)
      internal_error(type, error);
  }

  std::map<ColorSpaces, fc::PixelFormat> to_pixel_format{
      {ColorSpaces::RGB, fc::PIXEL_FORMAT_RGB8}, {ColorSpaces::GRAY, fc::PIXEL_FORMAT_MONO8}};

  std::map<fc::PixelFormat, ColorSpaces> to_color_space{
      {fc::PIXEL_FORMAT_RGB8, ColorSpaces::RGB}, {fc::PIXEL_FORMAT_MONO8, ColorSpaces::GRAY}};

};  // PtgreyDriver

}  // namespace camera
}  // namespace is

#endif  // __PTGREY_DRIVER_HPP__
