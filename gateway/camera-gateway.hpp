#ifndef __IS_CAMERA_GATEWAY_HPP__
#define __IS_CAMERA_GATEWAY_HPP__

#include <google/protobuf/empty.pb.h>
#include <is/msgs/common.pb.h>
#include <is/msgs/wire.pb.h>
#include <zipkin/opentracing.h>
#include <chrono>
#include <is/msgs/utils.hpp>
#include <is/wire/core.hpp>
#include <is/wire/rpc.hpp>
#include <memory>
#include "../drivers/camera-driver.hpp"

#define is_assert_set(failable)                    \
  do {                                             \
    auto status = failable;                        \
    if (status.code() != is::wire::StatusCode::OK) \
      return status;                               \
  } while (0)

#define is_assert_get(failable, releaser)                                                                    \
  do {                                                                                                       \
    auto status = failable;                                                                                  \
    if (status.code() != is::wire::StatusCode::OK && status.code() != is::wire::StatusCode::UNIMPLEMENTED) { \
      return status;                                                                                         \
    }                                                                                                        \
    if (status.code() == is::wire::StatusCode::UNIMPLEMENTED) {                                              \
      releaser;                                                                                              \
    }                                                                                                        \
  } while (0)

namespace is {
namespace camera {

using namespace is::common;
using namespace is::vision;

struct CameraGateway {
  CameraGateway(std::unique_ptr<CameraDriver> impl);
  void run(std::string const& uri, unsigned int const& id, 
           std::string const& zipkin_host, uint32_t const& zipkin_port);

 private:
  Status set_configuration(CameraConfig const& config);
  Status get_configuration(FieldSelector const& field_selector, CameraConfig* camera_config);

  std::unique_ptr<CameraDriver> driver;
};

}  // namespace camera
}  // namespace is

#endif  // __IS_GW_CAMERA_HPP__