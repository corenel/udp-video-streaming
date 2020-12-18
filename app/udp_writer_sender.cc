#include <chrono>

#include "opencv2/opencv.hpp"
#include "udp_writer.h"

int main(int argc, char* argv[]) {
  if ((argc < 3) || (argc > 3)) {  // Test for correct number of arguments
    std::cerr << "Usage: " << argv[0] << " <Server Addr> <Server Port>\n";
    exit(1);
  }
  std::string servAddress = argv[1];
  unsigned short servPort = Socket::resolveService(argv[2], "udp");

  cv::namedWindow("send", cv::WINDOW_AUTOSIZE);
  try {
    UdpWriter writer(servAddress, servPort);
    cv::Mat frame = cv::Mat(1920, 1080, CV_8UC3);
    while (true) {
      randu(frame, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count();
      cv::putText(frame, std::to_string(ms),
                  cv::Point(10, frame.rows / 2),  // top-left position
                  cv::FONT_HERSHEY_DUPLEX, 2.0, CV_RGB(255, 255, 255), 5);
      cv::imshow("send", frame);
      writer << frame;
      cv::waitKey(FRAME_INTERVAL);
    }
  } catch (SocketException& e) {
    std::cerr << e.what() << std::endl;
    exit(1);
  }

  return 0;
}
