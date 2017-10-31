#ifndef __IS_CAMERA_GATEWAY_HPP__
#define __IS_CAMERA_GATEWAY_HPP__

#include <is/is.hpp>
#include "camera-driver.hpp"

namespace is {
namespace camera {

using namespace is::common;
using namespace is::vision;

struct CameraGateway {
  std::unique_ptr<CameraDriver> driver;

  CameraGateway(std::unique_ptr<CameraDriver> impl) : driver(std::move(impl)) {}

  void configure_sampling_settings(SamplingSettings const& sampling_settings) {
    switch (sampling_settings.rate_case()) {
    case SamplingSettings::RateCase::kFrequency:
      driver->set_sampling_rate(sampling_settings.frequency());
      break;
    case SamplingSettings::RateCase::kPeriod:
      driver->set_sampling_rate(1 / sampling_settings.period());
      break;
    case SamplingSettings::RateCase::RATE_NOT_SET: break;
    }
    if (sampling_settings.has_delay())
      driver->set_delay(sampling_settings.delay().value());
  }

  void configure_image_settings(ImageSettings const& image_settings) {
    if (image_settings.has_resolution())
      driver->set_resolution(image_settings.resolution());
    if (image_settings.has_color_space())
      driver->set_color_space(image_settings.color_space());
    if (image_settings.has_format())
      driver->set_image_format(image_settings.format());
    if (image_settings.has_region())
      driver->set_region_of_interest(image_settings.region());
  }

  void configure_camera_settings(CameraSettings const& camera_settings) {}

  void configure_camera(CameraConfig const& config) {
    if (config.has_sampling())
      configure_sampling_settings(config.sampling());
    if (config.has_image())
      configure_image_settings(config.image());
    if (config.has_camera())
      configure_camera_settings(config.camera());
  }

  Status set_configuration(CameraConfig const& config) {
    try {
      configure_camera(config);
      return make_status(StatusCode::OK);
    } catch (Status const& status) { return status; }
  }

  Status get_configuration(FieldSelector const& field_selector, CameraConfig* camera_config) {
    return make_status(StatusCode::OK);
  }

  void run(std::string const& uri, unsigned int const& id) {
    is::info("Trying to connect to {}", uri);
    auto channel = rmq::Channel::CreateFromUri(uri);
    is::ServiceProvider provider;

    provider.connect(channel);
    auto queue = provider.make_queue("CameraGateway", std::to_string(id));

    provider.delegate<pb::Empty, CameraConfig>(
        queue, "SetConfig", [this](CameraConfig const& config, pb::Empty*) -> Status {
          return this->set_configuration(config);
        });
    provider.delegate<CameraConfig, FieldSelector>(
        queue, "GetConfig",
        [this](FieldSelector const& field_selector, CameraConfig* camera_config) -> Status {
          return this->get_configuration(field_selector, camera_config);
        });

    is::info("Stating to capture");
    driver->start_capture();

    for (;;) {
      auto image = driver->grab_image();
      is::publish(channel, fmt::format("CameraGateway.{}.Frame", id), image);

      rmq::Envelope::ptr_t envelope;
      if (channel->BasicConsumeMessage(envelope, 1)) {
        provider.serve(envelope);
      }
    }
  }

};  // ::CameraGateway
}  // ::camera
}  // ::is

#endif  // __IS_GW_CAMERA_HPP__
