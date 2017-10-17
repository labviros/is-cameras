#ifndef __IS_MSG_CAMERA_HPP__
#define __IS_MSG_CAMERA_HPP__

#include <is/msgs/common.hpp>
#include <is/packer.hpp>

#include <boost/optional.hpp>

namespace is {
namespace msg {
namespace camera {

using namespace is::msg::common;
using namespace boost;

struct TheoraPacket {
  bool new_header;
  std::vector<unsigned char> data;  // Packet binary data
  IS_DEFINE_MSG(new_header, data);
};

struct CompressedImage {
  std::string format;               // Image format: ".png", ".jpg"
  std::vector<unsigned char> data;  // Image binary data
  IS_DEFINE_MSG(format, data);
};

struct RegionOfInterest {
  unsigned int x_offset;  // Leftmost pixel of the ROI
  unsigned int y_offset;  // Topmost pixel of the ROI
  unsigned int height;    // Height of ROI
  unsigned int width;     // Width of ROI
  IS_DEFINE_MSG(x_offset, y_offset, height, width);
};

struct Resolution {
  unsigned int height;
  unsigned int width;
  IS_DEFINE_MSG(height, width);
};

struct ColorSpace {
  std::string value;
  IS_DEFINE_MSG(value);
};

namespace color_space {
const ColorSpace rgb{"rgb"};
const ColorSpace gray{"gray"};
}

struct ImageFormat {
  std::string format;
  optional<float> compression_level;
  IS_DEFINE_MSG(format, compression_level);
};

struct Brightness {
  optional<float> percent;
  optional<float> value;
  optional<bool> auto_mode;
  IS_DEFINE_MSG(percent, value, auto_mode);
};

struct Exposure {
  optional<float> percent;
  optional<float> ev;
  optional<bool> auto_mode;
  IS_DEFINE_MSG(percent, ev, auto_mode);
};

struct Sharpness {
  optional<float> percent;
  optional<float> value;
  optional<bool> auto_mode;
  IS_DEFINE_MSG(percent, value, auto_mode);
};

struct Hue {
  optional<float> percent;
  optional<float> degree;
  optional<bool> auto_mode;
  IS_DEFINE_MSG(percent, degree, auto_mode);
};

struct Saturation {
  optional<float> percent;
  optional<float> value;
  optional<bool> auto_mode;
  IS_DEFINE_MSG(percent, value, auto_mode);
};

struct Gamma {
  optional<float> percent;
  optional<float> value;
  optional<bool> auto_mode;
  IS_DEFINE_MSG(percent, value, auto_mode);
};

struct Shutter {
  optional<float> percent;
  optional<float> ms;
  optional<bool> auto_mode;
  IS_DEFINE_MSG(percent, ms, auto_mode);
};

struct Gain {
  optional<float> percent;
  optional<float> db;
  optional<bool> auto_mode;
  IS_DEFINE_MSG(percent, db, auto_mode);
};

struct WhiteBalance {
  optional<unsigned int> red;
  optional<unsigned int> blue;
  optional<bool> auto_mode;
  IS_DEFINE_MSG(red, blue, auto_mode);
};

struct Configuration {
  optional<ColorSpace> color_space;
  optional<Resolution> resolution;
  optional<RegionOfInterest> roi;

  optional<SamplingRate> sampling_rate;
  optional<Delay> delay;

  optional<ImageFormat> format;

  optional<Brightness> brightness;
  optional<Exposure> exposure;
  optional<Sharpness> sharpness;
  optional<Hue> hue;
  optional<Saturation> saturation;
  optional<Gamma> gamma;
  optional<Shutter> shutter;
  optional<Gain> gain;
  optional<WhiteBalance> white_balance;

  IS_DEFINE_MSG(color_space, resolution, roi, sampling_rate, delay, format, brightness, exposure,
                sharpness, hue, saturation, gamma, shutter, gain, white_balance);
};

}  // ::camera
}  // ::msg
}  // ::is

#endif  // __IS_MSG_CAMERA_HPP__
