#include "camera-gateway.hpp"
#include <string>
#include "ptgrey-driver.hpp"

int main(int argc, char** argv) {
  std::string uri, camera_ip, zipkin_host;
  unsigned int id;
  int packet_delay, packet_size, zipkin_port, parallelism;

  is::po::options_description opts("Options");
  auto&& opt_add = opts.add_options();
  opt_add("uri,u", is::po::value<std::string>(&uri)->required(), "amqp broker uri");
  opt_add("camera-ip,c", is::po::value<std::string>(&camera_ip)->required(), "camera ip address");
  opt_add("id,i", is::po::value<unsigned int>(&id)->default_value(0), "camera id");
  opt_add("packet-delay,d", is::po::value<int>(&packet_delay)->default_value(6000),
          "packet delay [0~6250]");
  opt_add("packet-size,s", is::po::value<int>(&packet_size)->default_value(6500),
          "packet delay [576~9000]");
  opt_add("zipkin-host,z",
          is::po::value<std::string>(&zipkin_host)->default_value("zipkin.default"),
          "zipkin hostname");
  opt_add("zipkin-port,p", is::po::value<int>(&zipkin_port)->default_value(9411), "zipkin port");
  opt_add("parallelism", is::po::value<int>(&parallelism)->default_value(0), "number of threads");
  is::parse_program_options(argc, argv, opts);

  cv::setNumThreads(parallelism);
  auto driver = std::make_unique<is::camera::PtgreyDriver>();
  driver->connect(camera_ip);
  driver->set_packet_delay(packet_delay);
  driver->set_packet_size(packet_size);

  is::Tracer tracer(fmt::format("CameraGateway.{}", id), zipkin_host, zipkin_port);

  is::camera::CameraGateway gateway(std::move(driver));
  gateway.run(uri, id, tracer);
}