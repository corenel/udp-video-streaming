#include "udp_capture.h"

#include <iostream>
#include <regex>

UdpCapture::UdpCapture(int port, int buffer_size, int packet_size)
    : port_(port),
      buffer_size_(buffer_size),
      packet_size_(packet_size),
      sock_(port_) {
  buffer_ = new char[buffer_size_];  // Buffer for echo string
  stream_th_ = new std::thread(&UdpCapture::openThread, this);
}

UdpCapture::~UdpCapture() { delete buffer_; }

void UdpCapture::openThread() {
  try {
    //    clock_t last_cycle = clock();

    while (true) {
      // recv and decode frame
      cv::Mat frame;
      if (!recvPacketsByTotalNumber(frame)) {
        continue;
      }

      // convert frame to OpenCV matrix
      {
        std::lock_guard<std::mutex> lock(image_lock_);
        frame_ = frame;
      }

      //      clock_t next_cycle = clock();
      //      double duration = static_cast<double>(next_cycle - last_cycle) /
      //                        static_cast<double>(CLOCKS_PER_SEC);
      //      std::cout << "\teffective FPS:" << (1 / duration) << " \tMbps:"
      //                << (PACK_SIZE * total_pack / duration / 1024 / 1024 * 8)
      //                << std::endl;
      //      last_cycle = next_cycle;
    }
  } catch (SocketException& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
}

bool UdpCapture::read(cv::Mat& frame) {
  if (frame_.empty()) {
    frame = cv::Mat();
    return false;
  }
  std::lock_guard<std::mutex> lock(image_lock_);
  frame = frame_;
  return true;
}

bool UdpCapture::recvPacketsByTotalNumber(cv::Mat& frame) {
  int recvMsgSize;            // Size of received message
  std::string sourceAddress;  // Address of datagram source
  unsigned short sourcePort;  // Port of datagram source

  // block until receive message from a client
  do {
    recvMsgSize =
        sock_.recvFrom(buffer_, buffer_size_, sourceAddress, sourcePort);
  } while (static_cast<unsigned>(recvMsgSize) > sizeof(int));
  int total_pack = ((int*)buffer_)[0];

  // receive packats
  //      std::cout << "expecting length of packs:" << total_pack <<
  //      std::endl;
  char* longbuf = new char[packet_size_ * total_pack];
  for (int i = 0; i < total_pack; i++) {
    recvMsgSize =
        sock_.recvFrom(buffer_, buffer_size_, sourceAddress, sourcePort);
    if (recvMsgSize != packet_size_) {
      std::cerr << "Received unexpected size pack:" << recvMsgSize << std::endl;
      continue;
    }
    memcpy(&longbuf[i * packet_size_], buffer_, packet_size_);
  }

  //      std::cout << "Received packet from " << sourceAddress << ":" <<
  //      sourcePort
  //                << std::endl;

  cv::Mat rawData = cv::Mat(1, packet_size_ * total_pack, CV_8UC1, longbuf);

  frame = imdecode(rawData, cv::IMREAD_COLOR);
  if (frame.size().width == 0) {
    std::cerr << "decode failure!" << std::endl;
    return false;
  }

  free(longbuf);
  return true;
}

bool UdpCapture::recvPacketsByFlag(cv::Mat& frame) { return false; }
