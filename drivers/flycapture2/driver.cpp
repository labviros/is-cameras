#include "driver.hpp"
#include <google/protobuf/util/message_differencer.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "internal/info.hpp"
#include "internal/nodes.hpp"

namespace is {
namespace camera {

namespace fc = FlyCapture2;
using namespace is::wire;

FlyCapture2Driver::FlyCapture2Driver() : uid(new fc::PGRGuid()), is_capturing(false) {
  this->color_space_map.insert(ColorSpaceBimap::value_type(ColorSpaces::GRAY, fc::PIXEL_FORMAT_MONO8));
  this->color_space_map.insert(ColorSpaceBimap::value_type(ColorSpaces::RGB, fc::PIXEL_FORMAT_RGB8));
}

FlyCapture2Driver::~FlyCapture2Driver() {
  delete this->uid;
}

std::vector<CameraInfo> FlyCapture2Driver::find_cameras() {
  fc::BusManager bus;
  unsigned int n_cameras;
  bus.GetNumOfCameras(&n_cameras);
  std::vector<CameraInfo> cam_infos;
  is::info("{} cameras found", n_cameras);
  std::vector<fc::CameraInfo> fc_cam_infos(n_cameras);
  auto error = fc::BusManager::DiscoverGigECameras(fc_cam_infos.data(), &n_cameras);
  if (error != fc::PGRERROR_OK)
    return cam_infos;
  for (auto& fc_cam_info : fc_cam_infos) {
    CameraInfo info;
    auto eth = info.mutable_ethernet();
    eth->set_ip_address(make_ip_address(fc_cam_info.ipAddress));
    eth->set_subnet_mask(make_subnet_mask(fc_cam_info.subnetMask));
    eth->set_mac_address(make_mac_address(fc_cam_info.macAddress));
    info.set_link_speed(make_link_speed(fc_cam_info.maximumBusSpeed));
    info.set_model_name(fc_cam_info.modelName);
    info.set_serial_number(std::to_string(fc_cam_info.serialNumber));
    cam_infos.push_back(info);
  }
  return cam_infos;
}

void FlyCapture2Driver::connect(CameraInfo const& cam_info) {
  fc::BusManager bus;
  auto error = bus.GetCameraFromSerialNumber(std::stoul(cam_info.serial_number()), this->uid);
  if (error != fc::PGRERROR_OK)
    is::critical("[Camera Initialize] {}", error.GetDescription());
  error = camera.Connect(this->uid);
  if (error != fc::PGRERROR_OK)
    is::critical("[Camera connections] {} ", error.GetDescription());
  // try {
  //   this->cam_system = spn::System::GetInstance();
  //   this->cam_list = this->cam_system->GetCameras();
  //   this->cam = this->cam_list.GetBySerial(cam_info.serial_number());
  //   this->cam->Init();
  // } catch (Spinnaker::Exception& e) {
  //   is::critical("[{}] {}", "Camera Initialize", e.what());
  // }

  // // Initial configuration
  // this->set_packet_size(1400);
  // this->set_packet_delay(6000);
  // this->reverse_x(false);
  // this->reverse_y(false);
  // set_op_enum(node_map(), "TriggerMode", "Off");
  // set_op_enum(node_map(), "TriggerSelector", "AcquisitionStart");

  // ImageFormat imgf;
  // imgf.set_format(ImageFormats::JPEG);
  // this->set_image_format(imgf);

  // pb::FloatValue sr;
  // sr.set_value(1.0);
  // this->set_sampling_rate(sr);
  // ColorSpace color_space;
  // color_space.set_value(ColorSpaces::GRAY);
  // this->set_color_space(color_space);
  // set_op_bool(node_map(), "IspEnable", true);  // necessery to pixel binning and sharpening works, just can set when
  // colorspace is GRAY
  // // fix-me: sometimes after enable ISP, shapening optinios aren't available to write.
  // set_op_bool(node_map(), "SharpeningEnable", true);
  // set_op_bool(node_map(), "SharpeningAuto", true);
  // set_op_float(node_map(), "Sharpening", 2.0);  // default camera value
  // set_op_bool(node_map(), "GammaEnable", true);
  // set_op_float(node_map(), "AutoExposureEVCompensation", 0.0);
  // set_op_enum(node_map(), "BinningHorizontalMode", "Average");
  // set_op_enum(node_map(), "BinningVerticalMode", "Average");
  // int64_t w = 0, h = 0;
  // get_op_int(node_map(), "SensorWidth", &w);
  // get_op_int(node_map(), "SensorHeight", &h);
  // this->sensor_width = w;
  // this->sensor_height = h;
  // OpRange<int64_t> binning_h, binning_v;
  // minmax_op_int(node_map(), "BinningHorizontal", &binning_h);
  // minmax_op_int(node_map(), "BinningVertical", &binning_v);
  // this->max_binning_h = binning_h.max;
  // this->max_binning_v = binning_v.max;
  // this->step_h = this->sensor_width / this->max_binning_h;
  // this->step_v = this->sensor_height / this->max_binning_v;
  // auto sc = std::max(std::min(this->max_binning_h, this->max_binning_v), 1);
  // while (sc > 0) {
  //   this->resolution_info =
  //       fmt::format("{} {}x{}", this->resolution_info, this->sensor_width / sc, this->sensor_height / sc);
  //   sc /= 2;
  // }
  // Resolution resolution;
  // resolution.set_width(this->sensor_width);
  // resolution.set_height(this->sensor_height);
  // this->set_resolution(resolution);
}

void FlyCapture2Driver::start_capture() {
  // try {
  //   this->cam->BeginAcquisition();
  //   this->is_capturing = true;
  // } catch (Spinnaker::Exception& e) {
  //   is::warn("[{}] {}", "Start Capture", e.what());
  // }
}

void FlyCapture2Driver::stop_capture() {
  // try {
  //   this->cam->EndAcquisition();
  //   this->is_capturing = false;
  // } catch (Spinnaker::Exception& e) {
  //   is::warn("[{}] {}", "Stop Capture", e.what());
  // }
}

Image FlyCapture2Driver::grab_image() {
  // spn::ImagePtr image;
  // try {
  //   image = this->cam->GetNextImage(3000);
  // } catch (...) {
  //   is::error("[Grab Image] Timeouted");
  //   this->stop_capture();
  //   this->start_capture();
  //   return Image();
  // }

  // if (image->IsIncomplete())
  //   is::warn("[Grab Image] Image incomplete");
  // this->timestamp = is::to_timestamp(std::chrono::system_clock::now());

  // auto rows = image->GetHeight();
  // auto cols = image->GetWidth();
  // auto data = image->GetData();
  // auto stride = image->GetStride();
  // auto pixel_format = image->GetPixelFormat();
  // cv::Mat frame;
  // if (pixel_format == spn::PixelFormatEnums::PixelFormat_Mono8)
  //   frame = cv::Mat(rows, cols, CV_8UC1, static_cast<unsigned char*>(data), stride);
  // else if (pixel_format == spn::PixelFormatEnums::PixelFormat_BGR8)
  //   frame = cv::Mat(rows, cols, CV_8UC3, static_cast<unsigned char*>(data), stride);
  // else {
  //   // throw std::runtime_error("[Grab Image] Bad image type");
  //   is::error("[Grab Image] Bad image type");
  //   return Image();
  // }
  // auto compression_parm = get_compression_parm();
  // std::vector<unsigned char> image_data;
  // cv::imencode(fmt::format(".{}", ImageFormats_Name(image_format.format())), frame, image_data, compression_parm);
  Image compressed;
  // auto compressed_data = compressed.mutable_data();
  // compressed_data->resize(image_data.size());
  // std::copy(image_data.begin(), image_data.end(), compressed_data->begin());
  return compressed;
}

pb::Timestamp FlyCapture2Driver::last_timestamp() {
  return this->timestamp;
}

Status FlyCapture2Driver::set_image_format(ImageFormat const& imgf) {
  if (imgf.has_compression()) {
    auto value = imgf.compression().value();
    if (value < 0.0 || value > 1.0) {
      auto why = fmt::format("Compression level equals to {} is out of range. Must be: [0.0,1.0]", value);
      return internal_error(StatusCode::OUT_OF_RANGE, why);
    }
  }
  this->image_format = imgf;
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_image_format(ImageFormat* imgf) {
  *imgf = this->image_format;
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_sampling_rate(pb::FloatValue const& rate) {
  // is_assert_ok(set_op_bool(node_map(), "AcquisitionFrameRateEnable", true));
  // is_assert_ok(set_op_float(node_map(), "AcquisitionFrameRate", rate.value()));
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_sampling_rate(pb::FloatValue* rate) {
  // float value = 0.0f;
  // is_assert_ok(get_op_float(node_map(), "AcquisitionFrameRate", &value));
  // rate->set_value(value);
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_color_space(ColorSpace const& color_space) {
  // ColorSpace current_color_space;
  // auto status = this->get_color_space(&current_color_space);
  // if (status.code() != StatusCode::OK) {
  //   auto why = fmt::format("Failed to read color space before set it.");
  //   return internal_error(StatusCode::INTERNAL_ERROR, why);
  // }
  // if (google::protobuf::util::MessageDifferencer::Equivalent(current_color_space, color_space)) {
  //   return is::make_status(StatusCode::OK);
  // }

  // auto function = [&](ColorSpace const& cs) -> Status {
  //   auto cs_gw = cs.value();
  //   auto pos = this->color_space_map.by<gateway>().find(cs_gw);
  //   if (pos == this->color_space_map.by<gateway>().end()) {
  //     auto why = fmt::format("Invalid type \"{}\". Valid types: \"RGB\" and \"GRAY\"", ColorSpaces_Name(cs_gw));
  //     return internal_error(StatusCode::INVALID_ARGUMENT, why);
  //   }
  //   auto cs_cam = pos->get<camera>();
  //   is_assert_ok(set_op_enum(node_map(), "PixelFormat", cs_cam));
  //   return is::make_status(StatusCode::OK);
  // };
  // return control_capture(function, color_space);
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_color_space(ColorSpace* color_space) {
  // std::string cs_cam;
  // is_assert_ok(get_op_enum(node_map(), "PixelFormat", &cs_cam));
  // auto pos = this->color_space_map.by<camera>().find(cs_cam);
  // if (pos == this->color_space_map.by<camera>().end())
  //   return internal_error(StatusCode::OUT_OF_RANGE, fmt::format("Color Space {} not recognized", cs_cam));
  // auto cs_gw = pos->get<gateway>();
  // color_space->set_value(cs_gw);
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_resolution(Resolution const& resolution) {
  // Resolution current_resolution;
  // auto status = this->get_resolution(&current_resolution);
  // if (status.code() != StatusCode::OK) {
  //   auto why = fmt::format("Failed to read resolution before set it.");
  //   return internal_error(StatusCode::INTERNAL_ERROR, why);
  // }
  // if (google::protobuf::util::MessageDifferencer::Equivalent(current_resolution, resolution)) {
  //   return is::make_status(StatusCode::OK);
  // }
  // // Changing the size of the image or the pixel encoding
  // // format requires the camera to be stopped and restarted.
  // auto function = [&](Resolution const& res) -> Status {
  //   auto width = res.width();
  //   auto height = res.height();
  //   auto equal_div = width / this->step_h == height / this->step_v;

  //   if (!(width % this->step_h == 0 && height % this->step_v == 0 && equal_div)) {
  //     auto why = fmt::format("{}x{} isn't a valid resolution. Choose betewen:{}", width, height,
  //     this->resolution_info); return internal_error(StatusCode::OUT_OF_RANGE, why);
  //   }
  //   // is_assert_ok(set_op_bool(node_map(), "IspEnable", true));  // just to ensure
  //   auto binning = this->sensor_width / width;
  //   is_assert_ok(set_op_int(node_map(), "OffsetX", 0));
  //   is_assert_ok(set_op_int(node_map(), "OffsetY", 0));
  //   is_assert_ok(set_op_int(node_map(), "BinningHorizontal", binning));
  //   is_assert_ok(set_op_int(node_map(), "BinningVertical", binning));
  //   is_assert_ok(set_op_int(node_map(), "Width", width));
  //   is_assert_ok(set_op_int(node_map(), "Height", height));
  //   return is::make_status(StatusCode::OK);
  // };
  // return control_capture(function, resolution);
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_resolution(Resolution* resolution) {
  // int64_t width = 0, height = 0;
  // is_assert_ok(get_op_int(node_map(), "Width", &width));
  // is_assert_ok(get_op_int(node_map(), "Height", &height));
  // resolution->set_width(width);
  // resolution->set_height(height);
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_region_of_interest(BoundingPoly const& roi) {
  // auto n_verticies = roi.vertices_size();
  // if (n_verticies < 2)
  //   return internal_error(StatusCode::INVALID_ARGUMENT, "Region of Interest must have at least 2 vertices");
  // if (n_verticies > 2)
  //   return internal_error(StatusCode::UNIMPLEMENTED, "Funtionality implemented just for BoundingPoly with 2
  //   vertices");

  // auto function = [&](BoundingPoly const& r) -> Status {
  //   auto top_left = r.vertices(0);
  //   auto bottom_right = r.vertices(1);
  //   int64_t width = bottom_right.x() - top_left.x();
  //   int64_t height = bottom_right.y() - top_left.y();
  //   int64_t max_width = 0, max_height = 0;
  //   is_assert_ok(get_op_int(node_map(), "WidthMax", &max_width));
  //   is_assert_ok(get_op_int(node_map(), "HeightMax", &max_height));
  //   is_assert_ok(set_op_int(node_map(), "Width", std::min(width, max_width)));
  //   is_assert_ok(set_op_int(node_map(), "Height", std::min(height, max_height)));
  //   OpRange<int64_t> offset_x, offset_y;
  //   is_assert_ok(minmax_op_int(node_map(), "OffsetX", &offset_x));
  //   is_assert_ok(minmax_op_int(node_map(), "OffsetY", &offset_y));
  //   is_assert_ok(set_op_int(node_map(), "OffsetX", std::min(static_cast<int64_t>(top_left.x()), offset_x.max)));
  //   is_assert_ok(set_op_int(node_map(), "OffsetY", std::min(static_cast<int64_t>(top_left.y()), offset_y.max)));
  //   return is::make_status(StatusCode::OK);
  // };
  // return control_capture(function, roi);
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_region_of_interest(BoundingPoly* roi) {
  // auto top_left = roi->add_vertices();
  // int64_t x = 0, y = 0;
  // is_assert_ok(get_op_int(node_map(), "OffsetX", &x));
  // is_assert_ok(get_op_int(node_map(), "OffsetY", &y));
  // top_left->set_x(x);
  // top_left->set_y(y);
  // auto bottom_right = roi->add_vertices();
  // int64_t w = 0, h = 0;
  // is_assert_ok(get_op_int(node_map(), "Width", &w));
  // is_assert_ok(get_op_int(node_map(), "Height", &h));
  // bottom_right->set_x(x + w);
  // bottom_right->set_y(y + h);
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_delay(pb::FloatValue const& delay) {
  /*
  float delay_us, min_delay, max_delay;
  std::tie(min_delay, max_delay, std::ignore) = nodes.float_op().get_range("TriggerDelay");
  std::modf(std::max(delay * 1e6f, min_delay), &delay_us);
  nodes.float_op().set("TriggerDelay", delay_us);
  */
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_delay(pb::FloatValue* delay) {
  delay->set_value(0.0f);
  // return nodes.float_op().get("TriggerDelay") * 1e-6f;
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_shutter(CameraSetting const& shutter) {
  // if (shutter.automatic())
  //   is_assert_ok(set_op_enum(node_map(), "ExposureAuto", "Continuous"));
  // else {
  //   is_assert_ok(set_op_enum(node_map(), "ExposureAuto", "Off"));
  //   float frame_rate = 0.0f;
  //   is_assert_ok(get_op_float(node_map(), "AcquisitionFrameRate", &frame_rate));
  //   auto period_us = 1e6 / frame_rate;
  //   is_assert_ok(set_op_float(node_map(), "ExposureTime", shutter.ratio() * period_us));
  // }
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_shutter(CameraSetting* shutter) {
  // std::string automatic_str;
  // is_assert_ok(get_op_enum(node_map(), "ExposureAuto", &automatic_str));
  // auto automatic = automatic_str != "Off";
  // float frame_rate = 0.0f;
  // is_assert_ok(get_op_float(node_map(), "AcquisitionFrameRate", &frame_rate));
  // auto period_us = 1e6 / frame_rate;
  // float value = 0.0f;
  // is_assert_ok(get_op_float(node_map(), "ExposureTime", &value));
  // shutter->set_automatic(automatic);
  // shutter->set_ratio(std::min(1.0, value / period_us));
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_gain(CameraSetting const& gain) {
  // if (gain.automatic()) {
  //   is_assert_ok(set_op_enum(node_map(), "GainAuto", "Continuous"));
  // } else {
  //   is_assert_ok(set_op_enum(node_map(), "GainAuto", "Off"));
  //   OpRange<float> range;
  //   is_assert_ok(minmax_op_float(node_map(), "Gain", &range));
  //   is_assert_ok(set_op_float(node_map(), "Gain", range.to_value(gain.ratio())));
  // }
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_gain(CameraSetting* gain) {
  // std::string automatic_str;
  // is_assert_ok(get_op_enum(node_map(), "GainAuto", &automatic_str));
  // auto automatic = automatic_str != "Off";
  // float value = 0.0f;
  // is_assert_ok(get_op_float(node_map(), "Gain", &value));
  // OpRange<float> range;
  // is_assert_ok(minmax_op_float(node_map(), "Gain", &range));
  // gain->set_automatic(automatic);
  // gain->set_ratio(range.to_ratio(value));
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_brightness(CameraSetting const& brightness) {
  // OpRange<float> range;
  // is_assert_ok(minmax_op_float(node_map(), "BlackLevel", &range));
  // is_assert_ok(set_op_float(node_map(), "BlackLevel", range.to_value(brightness.ratio())));
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_brightness(CameraSetting* brightness) {
  // float value = 0.0f;
  // is_assert_ok(get_op_float(node_map(), "BlackLevel", &value));
  // OpRange<float> range;
  // is_assert_ok(minmax_op_float(node_map(), "BlackLevel", &range));
  // brightness->set_automatic(false);
  // brightness->set_ratio(range.to_ratio(value));
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_white_balance(CameraSetting const& wb, std::string const& type) {
  // ColorSpace color_space;
  // is_assert_ok(this->get_color_space(&color_space));
  // if (color_space.value() != ColorSpaces::RGB)
  //   return internal_error(StatusCode::INTERNAL_ERROR, "White Balace availabe just on \'RGB\' color space");
  // if (wb.automatic())
  //   is_assert_ok(set_op_enum(node_map(), "BalanceWhiteAuto", "Continuous"));
  // else {
  //   is_assert_ok(set_op_enum(node_map(), "BalanceWhiteAuto", "Off"));
  //   is_assert_ok(set_op_enum(node_map(), "BalanceRatioSelector", type));
  //   OpRange<float> range;
  //   is_assert_ok(minmax_op_float(node_map(), "BalanceRatio", &range));
  //   is_assert_ok(set_op_float(node_map(), "BalanceRatio", range.to_value(wb.ratio())));
  // }
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_white_balance(CameraSetting* wb, std::string const& type) {
  // ColorSpace color_space;
  // is_assert_ok(this->get_color_space(&color_space));
  // if (color_space.value() != ColorSpaces::RGB)
  //   return internal_error(StatusCode::INTERNAL_ERROR, "White Balace availabe just on \'RGB\' color space");
  // std::string automatic_str;
  // is_assert_ok(get_op_enum(node_map(), "BalanceWhiteAuto", &automatic_str));
  // auto automatic = automatic_str != "Off";
  // is_assert_ok(set_op_enum(node_map(), "BalanceRatioSelector", type));
  // float value = 0.0f;
  // is_assert_ok(get_op_float(node_map(), "BalanceRatio", &value));
  // OpRange<float> range;
  // is_assert_ok(minmax_op_float(node_map(), "BalanceRatio", &range));
  // wb->set_automatic(automatic);
  // wb->set_ratio(range.to_ratio(value));
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_white_balance_bu(CameraSetting const& wb) {
  return this->set_white_balance(wb, "Blue");
}

Status FlyCapture2Driver::get_white_balance_bu(CameraSetting* wb) {
  return this->get_white_balance(wb, "Blue");
}

Status FlyCapture2Driver::set_white_balance_rv(CameraSetting const& wb) {
  return this->set_white_balance(wb, "Red");
}

Status FlyCapture2Driver::get_white_balance_rv(CameraSetting* wb) {
  return this->get_white_balance(wb, "Red");
}

Status FlyCapture2Driver::set_sharpness(CameraSetting const& sharpness) {
  /*
  if (sharpness.automatic()) {
    is_assert_ok(set_op_bool(node_map(), "SharpeningAuto", true));
  } else {
    is_assert_ok(set_op_bool(node_map(), "SharpeningAuto", false));
    OpRange<float> range;
    is_assert_ok(minmax_op_float(node_map(), "SharpeningThreshold", &range));
    is_assert_ok(
        set_op_float(node_map(), "SharpeningThreshold", range.to_value(sharpness.ratio())));
  }
  return is::make_status(StatusCode::OK);
  */
  return internal_error(StatusCode::UNIMPLEMENTED, "\'Sharpness\' property not implemented for this camera.");
}

Status FlyCapture2Driver::get_sharpness(CameraSetting* sharpness) {
  /*
  auto automatic = false;
  is_assert_ok(get_op_bool(node_map(), "SharpeningAuto", &automatic));
  float value;
  is_assert_ok(get_op_float(node_map(), "SharpeningThreshold", &value));
  OpRange<float> range;
  is_assert_ok(minmax_op_float(node_map(), "SharpeningThreshold", &range));
  sharpness->set_automatic(automatic);
  sharpness->set_ratio(range.to_ratio(value));
  return is::make_status(StatusCode::OK);
  */
  return internal_error(StatusCode::UNIMPLEMENTED, "\'Sharpness\' property not implemented for this camera.");
}

Status FlyCapture2Driver::set_gamma(CameraSetting const& gamma) {
  // OpRange<float> range;
  // is_assert_ok(minmax_op_float(node_map(), "Gamma", &range));
  // is_assert_ok(set_op_float(node_map(), "Gamma", range.to_value(gamma.ratio())));
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_gamma(CameraSetting* gamma) {
  // float value;
  // is_assert_ok(get_op_float(node_map(), "Gamma", &value));
  // OpRange<float> range;
  // is_assert_ok(minmax_op_float(node_map(), "Gamma", &range));
  // gamma->set_automatic(false);
  // gamma->set_ratio(range.to_ratio(value));
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_exposure(CameraSetting const& exposure) {
  // OpRange<float> range;
  // is_assert_ok(minmax_op_float(node_map(), "AutoExposureEVCompensation", &range));
  // is_assert_ok(set_op_float(node_map(), "AutoExposureEVCompensation", range.to_value(exposure.ratio())));
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::get_exposure(CameraSetting* exposure) {
  // float value;
  // is_assert_ok(get_op_float(node_map(), "AutoExposureEVCompensation", &value));
  // OpRange<float> range;
  // is_assert_ok(minmax_op_float(node_map(), "AutoExposureEVCompensation", &range));
  // exposure->set_automatic(false);
  // exposure->set_ratio(range.to_ratio(value));
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_hue(CameraSetting const&) {
  return internal_error(StatusCode::UNIMPLEMENTED, "\'HUE\' property not implemented for this camera.");
}

Status FlyCapture2Driver::get_hue(CameraSetting*) {
  return internal_error(StatusCode::UNIMPLEMENTED, "\'HUE\' property not implemented for this camera.");
}

Status FlyCapture2Driver::set_saturation(CameraSetting const&) {
  return internal_error(StatusCode::UNIMPLEMENTED, "\'Saturation\' property not implemented for this camera.");
}

Status FlyCapture2Driver::get_saturation(CameraSetting*) {
  return internal_error(StatusCode::UNIMPLEMENTED, "\'Saturation\' property not implemented for this camera.");
}

Status FlyCapture2Driver::set_focus(CameraSetting const&) {
  return internal_error(StatusCode::UNIMPLEMENTED, "\'Focus\' property not implemented for this camera.");
}

Status FlyCapture2Driver::get_focus(CameraSetting*) {
  return internal_error(StatusCode::UNIMPLEMENTED, "\'Focus\' property not implemented for this camera.");
}

Status FlyCapture2Driver::set_zoom(CameraSetting const&) {
  return internal_error(StatusCode::UNIMPLEMENTED, "\'Zoom\' property not implemented for this camera.");
}

Status FlyCapture2Driver::get_zoom(CameraSetting*) {
  return internal_error(StatusCode::UNIMPLEMENTED, "\'Zoom\' property not implemented for this camera.");
}

Status FlyCapture2Driver::set_iris(CameraSetting const&) {
  return internal_error(StatusCode::UNIMPLEMENTED, "\'Iris\' property not implemented for this camera.");
}

Status FlyCapture2Driver::get_iris(CameraSetting*) {
  return internal_error(StatusCode::UNIMPLEMENTED, "\'Iris\' property not implemented for this camera.");
}

Status FlyCapture2Driver::set_packet_delay(int const& packet_delay) {
  // auto function = [&](int const& pd) -> Status {
  //   is_assert_ok(set_op_int(node_map(), "GevSCPD", pd));
  //   return is::make_status(StatusCode::OK);
  // };
  // return control_capture(function, packet_delay);
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::set_packet_size(int const& packet_size) {
  // auto function = [&](int const& ps) -> Status {
  //   is_assert_ok(set_op_int(node_map(), "GevSCPSPacketSize", ps));
  //   return is::make_status(StatusCode::OK);
  // };
  // return control_capture(function, packet_size);
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::reverse_x(bool enable) {
  // auto function = [&](bool e) -> Status {
  //   is_assert_ok(set_op_bool(node_map(), "ReverseX", e));
  //   return is::make_status(StatusCode::OK);
  // };
  // return control_capture(function, enable);
  return is::make_status(StatusCode::OK);
}

Status FlyCapture2Driver::reverse_y(bool enable) {
  // auto function = [&](bool e) -> Status {
  //   is_assert_ok(set_op_bool(node_map(), "ReverseY", e));
  //   return is::make_status(StatusCode::OK);
  // };
  // return control_capture(function, enable);
  return is::make_status(StatusCode::OK);
}

std::vector<int> FlyCapture2Driver::get_compression_parm() {
  std::vector<int> parm;
  if (this->image_format.has_compression()) {
    auto value = this->image_format.compression().value();
    if (this->image_format.format() == ImageFormats::PNG) {
      parm.push_back(cv::IMWRITE_PNG_COMPRESSION);
      int level = value * (9 - 0) + 0;
      parm.push_back(level);
    } else if (this->image_format.format() == ImageFormats::JPEG) {
      parm.push_back(cv::IMWRITE_JPEG_QUALITY);
      int level = value * (100 - 0) + 0;
      parm.push_back(level);
    } else if (this->image_format.format() == ImageFormats::WebP) {
      parm.push_back(cv::IMWRITE_WEBP_QUALITY);
      int level = value * (100 - 1) + 1;
      parm.push_back(level);
    }
  }
  return parm;
}

}  // namespace camera
}  // namespace is