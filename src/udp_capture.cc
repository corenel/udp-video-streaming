#include "udp_capture.h"

#include <iostream>
#include <regex>

#include "config.h"

UdpCapture::UdpCapture(int port, int buffer_size)
    : port_(port), buffer_size_(buffer_size) {
  buffer_ = new char[buffer_size_];  // Buffer for echo string
  stream_th_ = new std::thread(&UdpCapture::openThread, this);
}

UdpCapture::~UdpCapture() { delete buffer_; }

void UdpCapture::openThread() {
  try {
    UDPSocket sock(port_);

    int recvMsgSize;            // Size of received message
    std::string sourceAddress;  // Address of datagram source
    unsigned short sourcePort;  // Port of datagram source
                                //    clock_t last_cycle = clock();

    while (true) {
      // block until receive message from a client
      do {
        recvMsgSize =
            sock.recvFrom(buffer_, buffer_size_, sourceAddress, sourcePort);
      } while (static_cast<unsigned>(recvMsgSize) > sizeof(int));
      int total_pack = ((int*)buffer_)[0];

      // receive packats
      std::cout << "expecting length of packs:" << total_pack << std::endl;
      char* longbuf = new char[PACK_SIZE * total_pack];
      for (int i = 0; i < total_pack; i++) {
        recvMsgSize =
            sock.recvFrom(buffer_, buffer_size_, sourceAddress, sourcePort);
        if (recvMsgSize != PACK_SIZE) {
          std::cerr << "Received unexpected size pack:" << recvMsgSize
                    << std::endl;
          continue;
        }
        memcpy(&longbuf[i * PACK_SIZE], buffer_, PACK_SIZE);
      }

      //      std::cout << "Received packet from " << sourceAddress << ":" <<
      //      sourcePort
      //                << std::endl;
      cv::Mat rawData = cv::Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);

      // decode frame
      cv::Mat frame = imdecode(rawData, cv::IMREAD_COLOR);
      if (frame.size().width == 0) {
        std::cerr << "decode failure!" << std::endl;
        continue;
      }
      free(longbuf);

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

cv::Mat UdpCapture::getMat() {
  if (frame_.empty()) {
    return cv::Mat();
  }
  std::lock_guard<std::mutex> lock(image_lock_);
  return frame_;
}
