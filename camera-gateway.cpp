#include "camera-gateway.hpp"
#include "ptgrey-driver.hpp"

int main() {
  std::string uri = "amqp://192.168.1.110:30000";
  is::camera::CameraGateway gateway("camera.0", uri, std::make_unique<is::camera::PtgreyDriver>());
  gateway.run();
}