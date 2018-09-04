#include "drivers/spinnaker/driver.hpp"
#include "drivers/flycapture2/driver.hpp"
#include "gateway/camera-gateway.hpp"

#include <fstream>
#include <is/msgs/validate.hpp>
#include "options.pb.h"
#include "google/protobuf/util/json_util.h"

#include <opencv2/core.hpp>

using namespace is::camera;

Options load_options(int argc, char** argv) {
  Options options;
  std::string filename = argc > 1 ? std::string(argv[1]) : "options.json";
  is::info("Loading options from {}", filename);
  std::ifstream in(filename);
  std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  if (!google::protobuf::util::JsonStringToMessage(s, &options).ok())
    is::critical("Failed to load options from {}. Exiting...", filename);
  is::info("Options loaded: {}", options);
  auto validate_status = is::validate_message(options);
  if (validate_status.code() != is::wire::StatusCode::OK)
    is::critical("{}", validate_status);
  return options;
}

int main(int argc, char** argv) {
  auto op = load_options(argc, argv);
  
  cv::setNumThreads(op.parallelism());
  auto cam_infos = SpinnakerDriver::find_cameras();
  for (auto& info : cam_infos) {
    is::info("{}", info);
  }
  cam_infos = FlyCapture2Driver::find_cameras();
  for (auto& info : cam_infos) {
    is::info("{}", info);
  }

  // auto cam_info = std::find_if(cam_infos.begin(), cam_infos.end(),
  //                              [&](auto& c) { return c.ethernet().ip_address() == op.camera_ip(); });
  // if (cam_info == cam_infos.end()) {
  //   is::critical("Camera with IP {} not found.", op.camera_ip());
  // }

  // is::info("Connecting to cammera {}", op.camera_ip());
  // auto driver = std::make_unique<SpinnakerDriver>();
  // driver->connect(*cam_info);
  // driver->set_packet_delay(op.packet_delay());
  // driver->set_packet_size(op.packet_size());
  // driver->reverse_x(op.reverse_x());
  // driver->reverse_y(op.reverse_y());

  // CameraGateway gateway(std::move(driver));
  // gateway.run(op.broker_uri(), op.camera_id(), op.zipkin_host(), op.zipkin_port(), op.initial_config());

  return 0;
}