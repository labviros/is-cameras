#ifndef __IS_CAMERA_GATEWAY_HPP__
#define __IS_CAMERA_GATEWAY_HPP__

#include <is/is.hpp>
//#include <is/theora-encoder.hpp> // need to fix msgs

#include "camera-driver.hpp"

namespace is {
namespace camera {

using namespace is::msg::common;
using namespace is::msg::camera;

struct CameraGateway {
  std::unique_ptr<CameraDriver> driver;
  std::mutex mutex;

  Connection is;
  // TheoraEncoder encoder;

  ServiceProvider service;
  Configuration config_;

  CameraGateway(std::string const& name, std::string const& uri, std::unique_ptr<CameraDriver> impl)
      : driver(std::move(impl)), is(uri), service(name, make_channel(uri)) {}

  void set_configuration(Configuration config) {
    if (config.resolution)
      driver->set_resolution(config.resolution.get());
    if (config.sampling_rate)
      driver->set_sampling_rate(config.sampling_rate.get());
    if (config.color_space)
      driver->set_color_space(config.color_space.get());
    if (config.brightness)
      driver->set_brightness(config.brightness.get());
    if (config.exposure)
      driver->set_exposure(config.exposure.get());
    if (config.shutter)
      driver->set_shutter(config.shutter.get());
    if (config.gain)
      driver->set_gain(config.gain.get());
    if (config.white_balance)
      driver->set_white_balance(config.white_balance.get());
    // ...
  };

  Configuration get_configuration(std::vector<std::string> params) {
    Configuration config;
    for (auto param : params) {
      param.erase(std::remove(param.begin(), param.end(), '_'), param.end());  // remove snake_case
      std::transform(param.begin(), param.end(), param.begin(), ::tolower);    // to lower case
      if (param == "samplingrate") {
        config.sampling_rate = driver->get_sampling_rate();
      }
    }
    return config;
  }

  void merge_configuration(Configuration const&) {}

  void run() {
    service.expose("set_configuration", [this](is::Request request) -> is::Reply {
      std::lock_guard<std::mutex> lock(mutex);
      try {
        auto config = is::msgpack<Configuration>(request);
        set_configuration(config);
        merge_configuration(config);
        return is::msgpack(status::ok);
      } catch (std::runtime_error error) {
        set_configuration(config_);  // restore last valid state
        return is::msgpack(status::error(error.what()));
      }
    });

    service.expose("get_configuration", [this](is::Request request) -> is::Reply {
      std::lock_guard<std::mutex> lock(mutex);
      std::pair<optional<Configuration>, Status> reply;
      try {
        auto params = is::msgpack<std::vector<std::string>>(request);
        reply.first = get_configuration(params);
        reply.second = status::ok;
      } catch (std::runtime_error error) { reply.second = status::error(error.what()); }
      return is::msgpack(reply);
    });

    std::thread thread([this] { service.listen(); });
    thread.join();
  }

};  // ::CameraGateway
}  // ::camera
}  // ::is

#endif  // __IS_GW_CAMERA_HPP__