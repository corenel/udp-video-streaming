#pragma once

#include <iostream>
#include <memory>
#include <mutex>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <thread>

#include "config.h"
#include "practical_socket.h"  // For UDPSocket and SocketException

class UdpCapture {
 public:
  explicit UdpCapture(int port, int buffer_size = 262144,
                      int packet_size = PACK_SIZE);
  ~UdpCapture();

  bool read(cv::Mat& frame);
  inline UdpCapture& operator>>(cv::Mat& img) {
    read(img);
    return *this;
  }

 private:
  int port_;
  int buffer_size_;
  int packet_size_;

  UDPSocket sock_;
  char* buffer_;
  cv::Mat frame_;

  std::mutex image_lock_;
  std::thread* stream_th_;
  std::atomic<bool> got_iamge_;

  void openThread();
  bool recvPacketsByTotalNumber(cv::Mat& frame);
  bool recvPacketsByFlag(cv::Mat& frame);
  bool decodeImageBuffer(char* buffer, int buffer_size, cv::Mat& frame);
};