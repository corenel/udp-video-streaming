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
  UdpCapture(int port, int buffer_size = 262144);
  ~UdpCapture();

  cv::Mat getMat();
  inline UdpCapture& operator>>(cv::Mat& img) {
    img = getMat();
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