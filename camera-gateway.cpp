#include "camera-gateway.hpp"
#include <string>
#include "ptgrey-driver.hpp"

int main(int argc, char** argv) {
  std::string uri;
  std::string camera_ip;
  unsigned int id;
  int packet_delay;
  int packet_size;
  std::string zipkin_host;
  int zipkin_port;

  is::po::options_description opts("Options");
  opts.add_options()("uri,u", is::po::value<std::string>(&uri)->required(), "amqp broker uri");
  opts.add_options()("camera-ip,c", is::po::value<std::string>(&camera_ip)->required(),
                     "camera ip address");
  opts.add_options()("id,i", is::po::value<unsigned int>(&id)->default_value(0), "camera id");
  opts.add_options()("packet-delay,d", is::po::value<int>(&packet_delay)->default_value(6000),
                     "packet delay [0~6250]");
  opts.add_options()("packet-size,s", is::po::value<int>(&packet_size)->default_value(6500),
                     "packet delay [576~9000]");
  opts.add_options()("zipkin-host,z",
                     is::po::value<std::string>(&zipkin_host)->default_value("zipkin.default"),
                     "zipkin hostname");
  opts.add_options()("zipkin-port,p", is::po::value<int>(&zipkin_port)->default_value(9411),
                     "zipkin port");
  is::parse_program_options(argc, argv, opts);

  auto driver = std::make_unique<is::camera::PtgreyDriver>();
  driver->connect(camera_ip);
  driver->set_packet_delay(packet_delay);
  driver->set_packet_size(packet_size);

  is::Tracer tracer(fmt::format("CameraGateway.{}", id), zipkin_host, zipkin_port);

  is::camera::CameraGateway gateway(std::move(driver));
  gateway.run(uri, id, tracer);
}