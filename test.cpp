#include <is/is.hpp>
#include <opencv2/highgui.hpp>

#include <is/msgs/camera.pb.h>
#include <is/msgs/common.pb.h>
#include <is/msgs/image.pb.h>

using namespace is;
using namespace is::vision;
using namespace is::common;

int main(int argc, char** argv) {
  if (argc != 2)
    exit(1);
  
  auto entity = fmt::format("CameraGateway.{}", argv[1]);
  auto uri = "amqp://rmq.is:30000";
  auto channel = rmq::Channel::CreateFromUri(uri);
  auto tag = is::declare_queue(channel);

  // CameraConfig config;
  // config.mutable_image()->mutable_format()->set_format(ImageFormats::JPEG);
  // config.mutable_image()->mutable_color_space()->set_color_space(ColorSpaces::RGB);
  // auto resolution = config.mutable_image()->mutable_resolution();;
  // resolution->set_width(1288 / 1);
  // resolution->set_height(728 / 1);
  // config.mutable_sampling()->set_frequency(5.0);
  // config.mutable_sampling()->mutable_delay()->set_value(0.050);

  FieldSelector fields;
  fields.add_fields(CameraConfigFields::ALL);
  auto queue = is::declare_queue(channel);
  auto id = is::request(channel, queue, fmt::format("{}.GetConfig", entity), fields);
  
  auto envelope = is::consume_for(channel, pb::TimeUtil::SecondsToDuration(2));
  if (envelope != nullptr) {
    auto status = is::rpc_status(envelope);
    status.PrintDebugString();
    auto camera_config = is::unpack<CameraConfig>(envelope);
    if (camera_config) {
      camera_config->PrintDebugString();
    }
  }


  is::subscribe(channel, tag, fmt::format("{}.Frame", entity));
  while (1) {
    auto envelope = channel->BasicConsumeMessage(tag);
    auto image = is::unpack<Image>(envelope);
    if (image) {
      std::vector<unsigned char> data(image->data().begin(), image->data().end());
      cv::Mat frame = cv::imdecode(data, CV_LOAD_IMAGE_COLOR);
      cv::imshow("frame", frame);
      cv::waitKey(1);
    }
  }
}
