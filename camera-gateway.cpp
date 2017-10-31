#include "camera-gateway.hpp"
#include "ptgrey-driver.hpp"
#include <string>

int main(int argc, char** argv) {
  std::string uri;
  std::string camera_ip;
  unsigned int id;
  
  is::po::options_description opts("Options");
  opts.add_options()("uri,u", is::po::value<std::string>(&uri)->required(), "amqp broker uri");
  opts.add_options()("camera-ip,c", is::po::value<std::string>(&camera_ip)->required(), "camera ip address");
  opts.add_options()("id,i", is::po::value<unsigned int>(&id)->default_value(0), "camera id");
  is::parse_program_options(argc, argv, opts);
  
  std::unique_ptr<is::camera::PtgreyDriver> driver(new is::camera::PtgreyDriver());
  driver->connect(camera_ip);
  is::camera::CameraGateway gateway(std::move(driver));
  gateway.run(uri, id);
}