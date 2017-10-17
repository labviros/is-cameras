#include <opencv2/highgui.hpp>
#include "ptgrey-driver.hpp"

int main(int argc, char** argv) {
  assert(argc == 2);

  is::camera::PtgreyDriver driver;
  driver.connect(argv[1]);

  is::msg::camera::SamplingRate sr;
  sr.rate = 20;

  is::msg::camera::Resolution resolution;
  resolution.width = 1288;
  resolution.height = 728;
  
  is::msg::camera::RegionOfInterest roi;
  roi.x_offset = 0;
  roi.y_offset = 0;
  roi.width = -1;
  roi.height = -1;
  
  driver.set_color_space(is::msg::camera::color_space::gray);
  driver.set_resolution(resolution);
  driver.set_region_of_interest(roi);
  driver.set_sampling_rate(sr);
  driver.start_capture();

  for (;;) {
    auto frame = driver.grab();
    cv::imshow("frame", frame);
    cv::waitKey(1);
  }
}
