#include "udp_writer.h"

#include <utility>

UdpWriter::UdpWriter(std::string addr, int port, int packet_size)
    : addr_(std::move(addr)), port_(port), packet_size_(packet_size), sock_() {}

void UdpWriter::write(const cv::Mat& frame) {
  try {
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
    int total_pack = 1 + (buffer_.size() - 1) / packet_size_;

    // send buffer
    int ibuf[1];
    ibuf[0] = total_pack;
    sock_.sendTo(ibuf, sizeof(int), addr_, port_);
    for (int i = 0; i < total_pack; i++)
      sock_.sendTo(&buffer_[i * packet_size_], packet_size_, addr_, port_);

  } catch (SocketException& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }
}
