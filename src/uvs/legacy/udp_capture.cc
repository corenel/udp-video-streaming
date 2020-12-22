#include "uvs/legacy/udp_capture.h"

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
#if PACKET_SPLIT_METHOD == PACKET_SPLIT_BEGIN_END_FLAG
      if (!recvPacketsByFlag(frame)) {
#else
      if (!recvPacketsByTotalNumber(frame)) {
#endif
        continue;
      }

      // convert frame to OpenCV matrix
      {
        std::lock_guard<std::mutex> lock(image_lock_);
        frame_ = frame;
        got_iamge_ = true;
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
  std::lock_guard<std::mutex> lock(image_lock_);
  if (frame_.empty() || !got_iamge_) {
    frame = cv::Mat();
    return false;
  }
  frame = frame_;
  return true;
}

bool UdpCapture::recvPacketsByTotalNumber(cv::Mat& frame) {
  int recvMsgSize;            // Size of received message
  int total_pack;             // Number of packets to receive
  std::string sourceAddress;  // Address of datagram source
  unsigned short sourcePort;  // Port of datagram source

  // block until receive message from a client
  do {
    recvMsgSize =
        sock_.recvFrom(buffer_, buffer_size_, sourceAddress, sourcePort);
    total_pack = atoi(buffer_);
  } while (static_cast<unsigned>(recvMsgSize) > 4 * sizeof(char) ||
           total_pack < 0 ||
           total_pack > 10);  // FIXME just for resolution 640*320
  //  int total_pack = ((int*)buffer_)[0];
  //  std::cout << "expecting length of packs:" << total_pack << std::endl;

  // receive packets
  char* longbuf = new char[packet_size_ * total_pack];
  int actual_pack = 0;
  for (int i = 0; i < total_pack; i++) {
    recvMsgSize =
        sock_.recvFrom(buffer_, buffer_size_, sourceAddress, sourcePort);
    if (recvMsgSize != packet_size_ && i != total_pack - 1) {
      std::cerr << "Received unexpected " << i
                << "-th size pack:" << recvMsgSize << std::endl;
      continue;
    }
    //    std::cerr << "Received expected size pack:" << recvMsgSize <<
    //    std::endl;
    memcpy(&longbuf[i * packet_size_], buffer_, packet_size_);
    actual_pack += 1;
  }

  //  std::cout << "Received " << actual_pack << " packets from " <<
  //  sourceAddress
  //            << ":" << sourcePort << std::endl;

  return decodeImageBuffer(longbuf, packet_size_ * total_pack, frame);
}

bool UdpCapture::recvPacketsByFlag(cv::Mat& frame) {
  int recvMsgSize;            // Size of received message
  std::string sourceAddress;  // Address of datagram source
  unsigned short sourcePort;  // Port of datagram source

  // block until receive begining message from a client
  std::string begin_flag(PACKET_SPLIT_BEGIN_FLAG),
      end_flag(PACKET_SPLIT_END_FLAG);
  do {
    recvMsgSize =
        sock_.recvFrom(buffer_, buffer_size_, sourceAddress, sourcePort);
    std::string recv_str(buffer_);
    if (recv_str.rfind(begin_flag, 0) == 0) {
      //      std::cout << "Received string:" << recv_str << std::endl;
      break;
    }
  } while (static_cast<unsigned>(recvMsgSize) >= begin_flag.length());

  // receive data
  int total_pack = 1000;
  char* longbuf = new char[packet_size_ * total_pack];
  for (int i = 0; i < total_pack; i++) {
    recvMsgSize =
        sock_.recvFrom(buffer_, buffer_size_, sourceAddress, sourcePort);
    if (recvMsgSize != packet_size_) {
      // stop when receiving end flag
      if (static_cast<unsigned>(recvMsgSize) >= end_flag.length()) {
        std::string recv_str(buffer_);
        if (recv_str.rfind(end_flag, 0) == 0) {
          //          std::cout << "Received string:" << recv_str << std::endl;
          break;
        }
      } else {
        std::cerr << "Received unexpected size pack:" << recvMsgSize
                  << std::endl;
        continue;
      }
    }
    memcpy(&longbuf[i * packet_size_], buffer_, packet_size_);
  }

  return decodeImageBuffer(longbuf, packet_size_ * total_pack, frame);
}

bool UdpCapture::decodeImageBuffer(char* longbuf, int longbuf_size,
                                   cv::Mat& frame) {
  cv::Mat rawData = cv::Mat(1, longbuf_size, CV_8UC1, longbuf);

  frame = imdecode(rawData, cv::IMREAD_COLOR);
  if (frame.size().width == 0) {
    std::cerr << "decode failure!" << std::endl;
    return false;
  }

  free(longbuf);
  return true;
}
