#include "uvs/util/latency_tester.h"

#include <numeric>

LatencyTester::LatencyTester() { prev_tick_ = cv::getTickCount(); };

LatencyTester::~LatencyTester() { cv::destroyAllWindows(); };

void LatencyTester::feed(const cv::Mat& frame) {
  cv::Mat img;
  cv::cvtColor(frame, img, cv::COLOR_BGR2GRAY);

  frame_number_++;
  bool is_now_dark = cv::mean(img)[0] < threshold_;
  //  std::cout << cv::mean(img)[0] << " " << is_dark_ << " " << is_now_dark
  //            << std::endl;
  if (is_dark_ != is_now_dark) {
    is_dark_ = is_now_dark;
    auto curr_tick = cv::getTickCount();
    auto curr_latency =
        static_cast<double>(curr_tick - prev_tick_) / cv::getTickFrequency();
    printf("Latency: %.3f sec, %d frames\n", curr_latency,
           frame_number_ - prev_change_frame_);
    prev_tick_ = curr_tick;
    prev_change_frame_ = frame_number_;
    latency_list.push_back(curr_latency);
  }

  int fill_color = is_dark_ ? 255 : 0;
  cv::Mat show(im_height_, im_width_, CV_8UC3,
               cv::Scalar(fill_color, fill_color, fill_color));
  cv::imshow("Test", show);
  auto ret = cv::waitKey(1);
  if (ret == 'r') {
    latency_list.clear();
  } else if (ret == 'p' && !latency_list.empty()) {
    printf("Avg Latency: %.3f sec\n",
           std::accumulate(latency_list.begin(), latency_list.end(), 0.0) /
               latency_list.size());
  }
}
