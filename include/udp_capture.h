#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <thread>

#include "practical_socket.h"  // For UDPSocket and SocketException

class UdpCapture {
 public:
  explicit UdpCapture(int port, int buffer_size = 262144);
  ~UdpCapture();

  bool read(cv::Mat& frame);
  inline UdpCapture& operator>>(cv::Mat& img) {
    read(img);
    return *this;
  }

 private:
  int port_;
  int buffer_size_;
  char* buffer_;
  cv::Mat frame_;

  std::mutex image_lock_;
  std::thread* stream_th_;

  void openThread();
};