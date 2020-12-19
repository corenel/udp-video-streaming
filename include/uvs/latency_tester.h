#pragma once

#include "opencv2/opencv.hpp"

class LatencyTester {
 public:
  LatencyTester();
  ~LatencyTester();

  void feed(const cv::Mat& frame);

 private:
  int threshold_ = 150;
  int im_width_ = 1280;
  int im_height_ = 720;
  int frame_number_ = 0;
  int prev_change_frame_ = 0;
  bool is_dark_ = true;
  int64 prev_tick_;

  std::vector<double> latencies_;
};
