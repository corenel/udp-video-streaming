#include "uvs/udp_writer.h"

#include <utility>

UdpWriter::UdpWriter(std::string addr, int port, int packet_size)
    : addr_(std::move(addr)), port_(port), packet_size_(packet_size), sock_() {}

void UdpWriter::write(const cv::Mat& frame) {
  try {
    std::lock_guard<std::mutex> lock(image_lock_);

    // simple integrity check; skip erroneous data
    if (frame.size().width == 0) return;

    // pre-process
    cv::Mat send;
    resize(frame, send, cv::Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0,
           cv::INTER_LINEAR);

    // encode frame
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(jpeg_quality_);
    imencode(".jpg", send, buffer_, compression_params);
    imshow("send", send);

    // send buffer
#if PACKET_SPLIT_METHOD == PACKET_SPLIT_BEGIN_END_FLAG
    sendPacketsByFlag();
#else
    sendPacketsByTotalNumber();
#endif
  } catch (SocketException& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
}

void UdpWriter::sendPacketsByTotalNumber() {
  int total_pack = 1 + (buffer_.size() - 1) / packet_size_;
  //  int ibuf[1];
  //  ibuf[0] = total_pack;
  //  sock_.sendTo(ibuf, sizeof(int), addr_, port_);
  std::string total_pack_str = std::to_string(total_pack);
  sock_.sendTo(total_pack_str.c_str(), total_pack_str.length(), addr_, port_);
  for (int i = 0; i < total_pack; i++)
    sock_.sendTo(&buffer_[i * packet_size_], packet_size_, addr_, port_);
}

void UdpWriter::sendPacketsByFlag() {
  int total_pack = 1 + (buffer_.size() - 1) / packet_size_;
  std::string begin_flag(PACKET_SPLIT_BEGIN_FLAG),
      end_flag(PACKET_SPLIT_END_FLAG);
  sock_.sendTo(begin_flag.c_str(), begin_flag.length(), addr_, port_);
  for (int i = 0; i < total_pack; i++)
    sock_.sendTo(&buffer_[i * packet_size_], packet_size_, addr_, port_);
  sock_.sendTo(end_flag.c_str(), end_flag.length(), addr_, port_);
}
