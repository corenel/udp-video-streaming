#pragma once

#include <cstdlib>   // For atoi()
#include <iostream>  // For std::cout and std::cerr
#include <mutex>

#include "config.h"
#include "opencv2/opencv.hpp"
#include "practical_socket.h"  // For UDPSocket and SocketException

class UdpWriter {
 public:
  explicit UdpWriter(std::string addr, int port, int packet_size = PACK_SIZE);
  ~UdpWriter() = default;

  inline UdpWriter& operator<<(const cv::Mat& frame) {
    write(frame);
    return *this;
  };
  void write(const cv::Mat& frame);

 private:
  std::string addr_;
  int port_;
  int packet_size_;
  int jpeg_quality_ = ENCODE_QUALITY;  // Compression Parameter

  UDPSocket sock_;
  std::vector<uchar> buffer_;
  std::mutex image_lock_;

  void sendPacketsByTotalNumber();
  void sendPacketsByFlag();
};
