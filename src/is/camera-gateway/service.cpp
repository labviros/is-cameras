#include "is/camera-drivers/flycapture2/driver.hpp"
#include "is/camera-drivers/spinnaker/driver.hpp"
#include "is/camera-gateway/camera-gateway.hpp"

#include <fstream>
#include <is/msgs/validate.hpp>
#include "boost/variant.hpp"
#include "google/protobuf/util/json_util.h"
#include "conf/options.pb.h"

#include <opencv2/core.hpp>

using namespace is::camera;

CameraGatewayOptions load_options(int argc, char** argv) {
  CameraGatewayOptions options;
  std::string filename = argc > 1 ? std::string(argv[1]) : "options.json";
  is::info("Loading options from {}", filename);
  is::load(filename, &options);
  is::info("Options loaded: {}", options);
  is::validate_message(options);
  return options;
}

int main(int argc, char** argv) {
  auto op = load_options(argc, argv);

  cv::setNumThreads(op.parallelism());
  auto cdriver = op.camera_driver();
  std::vector<std::pair<CameraDrivers, CameraInfo>> cam_infos;
  if (op.camera_driver() == CameraDrivers::NOT_SPECIFIED || op.camera_driver() == CameraDrivers::FLYCAPTURE) {
    auto infos = FlyCapture2Driver::find_cameras();
    std::transform(infos.begin(), infos.end(), std::back_inserter(cam_infos),
                   [](auto& info) { return std::make_pair(CameraDrivers::FLYCAPTURE, info); });
  }
  if (op.camera_driver() == CameraDrivers::NOT_SPECIFIED || op.camera_driver() == CameraDrivers::SPINNAKER) {
    auto infos = SpinnakerDriver::find_cameras();
    std::transform(infos.begin(), infos.end(), std::back_inserter(cam_infos),
                   [](auto& info) { return std::make_pair(CameraDrivers::SPINNAKER, info); });
  }

  for (auto& info : cam_infos) {
    is::info("{} -> {}", CameraDrivers_Name(info.first), info.second);
  }

  auto pos = std::find_if(cam_infos.begin(), cam_infos.end(),
                          [&](auto& c) { return c.second.ethernet().ip_address() == op.camera_ip(); });
  if (pos == cam_infos.end()) {
    is::critical("Camera with IP {} not found.", op.camera_ip());
  }

  is::info("Connecting to camera {}", op.camera_ip());
  std::unique_ptr<CameraDriver> driver;
  if (pos->first == CameraDrivers::FLYCAPTURE)
    driver = std::make_unique<FlyCapture2Driver>();
  if (pos->first == CameraDrivers::SPINNAKER)
    driver = std::make_unique<SpinnakerDriver>();
  driver->connect(pos->second);
  driver->set_packet_delay(op.packet_delay());
  driver->set_packet_size(op.packet_size());
  driver->reverse_x(op.reverse_x());
  driver->reverse_y(op.reverse_y());
  CameraGateway gateway(driver.get());
  gateway.run(op.broker_uri(), op.camera_id(), op.zipkin_host(), op.zipkin_port(), op.initial_config());

  return 0;
}