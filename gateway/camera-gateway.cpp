#include "camera-gateway.hpp"
#include <zipkin/opentracing.h>

namespace is {
namespace camera {

using namespace is::wire;
using namespace is::common;
using namespace is::vision;
using namespace std::chrono;
using namespace zipkin;
using namespace opentracing;

CameraGateway::CameraGateway(CameraDriver* impl) : driver(impl) {}

Status CameraGateway::set_configuration(CameraConfig const& config) {
  // TODO: receovery previous context
  if (config.has_image()) {
    auto& img_s = config.image();
    if (img_s.has_resolution())
      is_assert_set(driver->set_resolution(img_s.resolution()));
    if (img_s.has_color_space())
      is_assert_set(driver->set_color_space(img_s.color_space()));
    if (img_s.has_format())
      is_assert_set(driver->set_image_format(img_s.format()));
    if (img_s.has_region())
      is_assert_set(driver->set_region_of_interest(img_s.region()));
  }

  if (config.has_sampling()) {
    auto& smp_s = config.sampling();
    if (smp_s.has_frequency())
      is_assert_set(driver->set_sampling_rate(smp_s.frequency()));
    if (smp_s.has_delay())
      is_assert_set(driver->set_delay(smp_s.delay()));
  }

  if (config.has_camera()) {
    auto& cam_s = config.camera();
    if (cam_s.has_brightness())
      is_assert_set(driver->set_brightness(cam_s.brightness()));
    if (cam_s.has_exposure())
      is_assert_set(driver->set_exposure(cam_s.exposure()));
    if (cam_s.has_focus())
      is_assert_set(driver->set_focus(cam_s.focus()));
    if (cam_s.has_gain())
      is_assert_set(driver->set_gain(cam_s.gain()));
    if (cam_s.has_gamma())
      is_assert_set(driver->set_gamma(cam_s.gamma()));
    if (cam_s.has_hue())
      is_assert_set(driver->set_hue(cam_s.hue()));
    if (cam_s.has_iris())
      is_assert_set(driver->set_iris(cam_s.iris()));
    if (cam_s.has_saturation())
      is_assert_set(driver->set_saturation(cam_s.saturation()));
    if (cam_s.has_sharpness())
      is_assert_set(driver->set_sharpness(cam_s.sharpness()));
    if (cam_s.has_shutter())
      is_assert_set(driver->set_shutter(cam_s.shutter()));
    if (cam_s.has_white_balance_bu())
      is_assert_set(driver->set_white_balance_bu(cam_s.white_balance_bu()));
    if (cam_s.has_white_balance_rv())
      is_assert_set(driver->set_white_balance_rv(cam_s.white_balance_rv()));
    if (cam_s.has_zoom())
      is_assert_set(driver->set_zoom(cam_s.zoom()));
  }

  return is::make_status(StatusCode::OK);
}

Status CameraGateway::get_configuration(FieldSelector const& field_selector, CameraConfig* camera_config) {
  auto begin = field_selector.fields().begin();
  auto end = field_selector.fields().end();
  auto pos = std::find(begin, end, CameraConfigFields::ALL);

  FieldSelector _field_selector;
  if (pos != end) {
    // to prevent multiples get in case of all features requested
    _field_selector.add_fields(CameraConfigFields::IMAGE_SETTINGS);
    _field_selector.add_fields(CameraConfigFields::SAMPLING_SETTINGS);
    _field_selector.add_fields(CameraConfigFields::CAMERA_SETTINGS);
    begin = _field_selector.fields().begin();
    end = _field_selector.fields().end();
  }

  for (auto it = begin; it != end; ++it) {
    auto field = *it;
    if (field == CameraConfigFields::IMAGE_SETTINGS) {
      auto img_s = camera_config->mutable_image();
      is_assert_get(driver->get_resolution(img_s->mutable_resolution()), img_s->release_resolution());
      is_assert_get(driver->get_color_space(img_s->mutable_color_space()), img_s->release_color_space());
      is_assert_get(driver->get_image_format(img_s->mutable_format()), img_s->release_format());
      is_assert_get(driver->get_region_of_interest(img_s->mutable_region()), img_s->release_region());
    } else if (field == CameraConfigFields::SAMPLING_SETTINGS) {
      auto smp_s = camera_config->mutable_sampling();
      is_assert_get(driver->get_sampling_rate(smp_s->mutable_frequency()), smp_s->release_frequency());
      is_assert_get(driver->get_delay(smp_s->mutable_delay()), smp_s->release_delay());
    } else  if (field == CameraConfigFields::CAMERA_SETTINGS) {
      auto cam_s = camera_config->mutable_camera();
      is_assert_get(driver->get_shutter(cam_s->mutable_shutter()), cam_s->release_shutter());
      is_assert_get(driver->get_gain(cam_s->mutable_gain()), cam_s->release_gain());
      is_assert_get(driver->get_brightness(cam_s->mutable_brightness()), cam_s->release_brightness());
      is_assert_get(driver->get_white_balance_bu(cam_s->mutable_white_balance_bu()), cam_s->release_white_balance_bu());
      is_assert_get(driver->get_white_balance_rv(cam_s->mutable_white_balance_rv()), cam_s->release_white_balance_rv());
      is_assert_get(driver->get_sharpness(cam_s->mutable_sharpness()), cam_s->release_sharpness());
      is_assert_get(driver->get_gamma(cam_s->mutable_gamma()), cam_s->release_gamma());
      is_assert_get(driver->get_exposure(cam_s->mutable_exposure()), cam_s->release_exposure());
      is_assert_get(driver->get_hue(cam_s->mutable_hue()), cam_s->release_hue());
      is_assert_get(driver->get_saturation(cam_s->mutable_saturation()), cam_s->release_saturation());
      is_assert_get(driver->get_focus(cam_s->mutable_focus()), cam_s->release_focus());
      is_assert_get(driver->get_zoom(cam_s->mutable_zoom()), cam_s->release_zoom());
      is_assert_get(driver->get_iris(cam_s->mutable_iris()), cam_s->release_iris());
    }
  }

  return is::make_status(StatusCode::OK);
}

void CameraGateway::run(std::string const& uri, unsigned int const& id,
                        std::string const& zipkin_host, uint32_t const& zipkin_port, 
                        is::vision::CameraConfig const& initial_config) {
  is::info("Trying to connect to {}", uri);

  auto channel = is::Channel(uri);
  auto provider = is::ServiceProvider(channel);

  ZipkinOtTracerOptions zp_options;
  zp_options.service_name = fmt::format("CameraGateway.{}", id);
  zp_options.collector_host = zipkin_host;
  zp_options.collector_port = zipkin_port;
  auto tracer = makeZipkinOtTracer(zp_options);
  channel.set_tracer(tracer);
  
  auto log_interceptor = is::LogInterceptor();
  provider.add_interceptor(log_interceptor);

  this->set_configuration(initial_config);

  provider.delegate<CameraConfig, is::pb::Empty>(
      fmt::format("CameraGateway.{}.SetConfig", id),
      [this](Context*, CameraConfig const& config, is::pb::Empty*) -> Status {
        return this->set_configuration(config);
      });

  provider.delegate<FieldSelector, CameraConfig>(
      fmt::format("CameraGateway.{}.GetConfig", id),
      [this](Context*, FieldSelector const& field_selector, CameraConfig* camera_config) -> Status {
        return this->get_configuration(field_selector, camera_config);
      });

  is::info("Starting to capture");
  driver->start_capture();
  for (;;) {
    auto image = driver->grab_image();
    
    if (image.data().size() > 0) {
      auto im_msg = Message(image);
      auto timestamp = driver->last_timestamp();
      
      auto span = tracer->StartSpan("Frame", {opentracing::v1::StartTimestamp(is::to_system_clock(timestamp))});
      is::OtWriter ot_writer(&im_msg);
      tracer->Inject(span->context(), ot_writer);
      channel.publish(fmt::format("CameraGateway.{}.Frame", id), im_msg);
      span->Finish();

      auto ts_msg = Message(timestamp);
      channel.publish(fmt::format("CameraGateway.{}.Timestamp", id), ts_msg);
    }

    auto maybe_msg = channel.consume_for(seconds(0));
    if (maybe_msg) {
      provider.serve(*maybe_msg);
    }
  }
}

}  // namespace camera
}  // namespace is