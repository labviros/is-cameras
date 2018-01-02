#include <is/is.hpp>
#include <opencv2/highgui.hpp>

#include <is/msgs/camera.pb.h>
#include <is/msgs/common.pb.h>
#include <is/msgs/image.pb.h>

using namespace is;
using namespace is::vision;
using namespace is::common;

ColorSpaces make_color_space(std::string cs) {
  boost::algorithm::to_lower(cs);
  if (cs == "rgb")
    return ColorSpaces::RGB;
  else
    return ColorSpaces::GRAY;
}

int main(int argc, char** argv) {
  std::string uri;
  std::string camera_id;
  float fps;
  std::string color_space;

  is::po::options_description opts("Options");
  opts.add_options()("uri,u", is::po::value<std::string>(&uri)->required(), "amqp broker uri");
  opts.add_options()("camera-id,i", is::po::value<std::string>(&camera_id)->required(),
                     "camera id");
  opts.add_options()("fps,f", is::po::value<float>(&fps), "frame rate");
  opts.add_options()("color-space,c", is::po::value<std::string>(&color_space),
                     "color space [RGB/GRAY]");
  auto vm = is::parse_program_options(argc, argv, opts);

  CameraConfig config;
  if (vm.count("fps"))
    config.mutable_sampling()->set_frequency(fps);
  if (vm.count("color-space"))
    config.mutable_image()->mutable_color_space()->set_value(make_color_space(color_space));

  auto entity = fmt::format("CameraGateway.{}", camera_id);
  auto channel = rmq::Channel::CreateFromUri(uri);
  auto tag = is::declare_queue(channel);
  auto id = is::request(channel, tag, fmt::format("{}.SetConfig", entity), config);

  auto envelope = is::consume_for(channel, pb::TimeUtil::SecondsToDuration(5));
  if (envelope != nullptr) {
    auto status = is::rpc_status(envelope);
    if (status.code() == StatusCode::OK)
      is::info("{}", status);
    else
      is::warn("{}", status);
  }

  is::subscribe(channel, tag, fmt::format("{}.Frame", entity));
  while (1) {
    auto envelope = channel->BasicConsumeMessage(tag);
    auto image = is::unpack<Image>(envelope);
    if (image) {
      std::vector<unsigned char> data(image->data().begin(), image->data().end());
      cv::Mat frame = cv::imdecode(data, CV_LOAD_IMAGE_COLOR);
      cv::imshow(entity, frame);
      cv::waitKey(1);
    }
  }
}
